#include "TheOtherBarry.h"

#include <intrin.h>
#include <immintrin.h>
#include <simd/vectorclass.h>
#include <cassert>
#include <array>
#include "SimdHelpers.h"

namespace barry {
	constexpr const int32_t TILE_SIZE = 8;
	using TScreenCoord = int32_t;

	struct alignas(16) RVector4 {
		// screen space x, y
		// z is z in view space
		// w is 1/z in view space
		float x, y, z, w;

		static RVector4 fromVertex(const Vertex* v) {
			return RVector4 { v->PX, v->PY, 1.0f / v->RZ, v->RZ };
		}
	};

	struct TileTxtrInfo {
		float uz0, vz0, rz0;
		float r0, g0, b0, a0;
		float uz1, vz1, rz1;
		float uz2, vz2, rz2;
		float uz3, vz3, rz3;
	};

	struct Tile {
		int x, y;

		TScreenCoord a0, dadx, dady;
		TScreenCoord b0, dbdx, dbdy;
		TScreenCoord c0, dcdx, dcdy;

		float rz0;
		TileTxtrInfo t0;
	};

	using Triangle = RVector4[3];

	enum class TInterpolationType {
		AFFINE,
		QUADRATIC
	};

	enum class TBlendMode {
		XOR,
		OVERWRITE,
		TRANSPARENT,
	};

	// block-tiling adjustment functions
	// Example for 256x256 texture
	//    3         2         1         0
	//   10987654321098765432109876543210
	// U 0000UUUUUU00000000uu0fffffffffff
	// V 0000000000VVVVVVvv000fffffffffff

	uint32_t tile_vmask(uint32_t vmask) {
		return 0x7ff | (vmask << 14);
	}

	uint32_t tile_v(uint32_t v, uint32_t vmask) {
		return (v & 0x7ff) | ((v << 3) & (vmask << 14));
	}

	uint32_t tile_dv(uint32_t v, uint32_t vmask) {
		return tile_v(v, vmask) | 0x3800;
	}
	
	uint32_t tile_umask(uint32_t vbits, uint32_t umask) {
		return 0x37ff | ((umask >> 2) << (14 + vbits));
	}

	uint32_t tile_u(uint32_t u, uint32_t vbits, uint32_t umask) {
		return (u & 0x7ff) | ((u & 0x1800) << 1) | ((u << (1 + vbits)) & ((umask >> 2) << (14 + vbits)));
	}

	uint32_t tile_du(uint32_t u, uint32_t vbits, uint32_t umask) {
		return tile_u(u, vbits, umask) | 0x800 | (((1 << vbits) - 1) << 14);
	}

	struct TileRasterizer {
		TileRasterizer(Vertex **V, byte* dstSurface, int32_t bpsl, int32_t xres, int32_t yres, Texture* Txtr, int miplevel)
			: V(V)
			, dstSurface(dstSurface)
			, bpsl(bpsl)
			, xres(xres)
			, yres(yres) {

			t0.LogWidth = Txtr->LSizeX - miplevel;
			t0.LogHeight = Txtr->LSizeY - miplevel;
			t0.TextureAddr = (dword *)Txtr->Mipmap[miplevel];

			t0.UScaleFactor = (1 << t0.LogWidth);
			t0.VScaleFactor = (1 << t0.LogHeight);
		}

		Vertex** V;
		byte* dstSurface;
		int32_t bpsl;
		int32_t xres;
		int32_t yres;
		struct TextureInfo {
			const dword* TextureAddr;
			int32_t LogWidth;
			int32_t LogHeight;
			float UScaleFactor;
			float VScaleFactor;
			float duzdx, duzdy;
			float dvzdx, dvzdy;
		};
		float drzdx, drzdy;
		float dadx, dady;
		float drdx, drdy;
		float dgdx, dgdy;
		float dbdx, dbdy;

		uint32_t umask;// = (1 << t0.LogWidth) - 1);
		uint32_t vmask;// = (1 << t0.LogHeight) - 1);
		TextureInfo t0;
		//size_t v1 = 0 , v2 = 0, v3 = 0;
		//void setVertexIndexes(size_t v1, size_t v2, size_t v3) {
		//	this->v1 = v1;
		//	this->v2 = v2;
		//	this->v3 = v3;
		//}

		int32_t clampedX(int32_t x) {
			return std::min(std::max(x, 0), xres - 1);
		}
		int32_t clampedY(int32_t y) {
			return std::min(std::max(y, 0), yres - 1);
		}

		template <TBlendMode BlendMode = TBlendMode::OVERWRITE>
		void apply_exact(const barry::Tile& tile) {
			auto scanline = dstSurface + tile.y * TILE_SIZE * bpsl;
			auto zscanline = dstSurface + PageSize + tile.y * TILE_SIZE * XRes * 2;
			auto span = ((uint32_t*)scanline) + tile.x * TILE_SIZE;
			auto zspan = ((uint16_t*)zscanline) + tile.x * TILE_SIZE;
			auto bpsl_u32 = bpsl / sizeof(uint32_t);

			TScreenCoord a0 = tile.a0;
			TScreenCoord b0 = tile.b0;
			TScreenCoord c0 = tile.c0;

			Vec8i p_a = v8_from_arith_seq(a0, tile.dadx);
			Vec8i p_b = v8_from_arith_seq(b0, tile.dbdx);
			Vec8i p_c = v8_from_arith_seq(c0, tile.dcdx);

			int32_t t0_umask = (1 << t0.LogWidth) - 1;
			int32_t t0_vmask = (1 << t0.LogHeight) - 1;
			int32_t t0_umask_swizzled = swizzle_umask(t0.LogHeight, t0_umask);
			Vec8f p_rz = v8_from_arith_seq(tile.rz0, drzdx);
			Vec8f p_uz = v8_from_arith_seq(tile.t0.uz0, t0.duzdx);
			Vec8f p_vz = v8_from_arith_seq(tile.t0.vz0, t0.dvzdx);

			// we need two 256 registers to handle coloring 8 pixels
			// 256 / 16 bit per channel / 4 channels = 4 pixels
			// 0..15	16..31	32..47	48..63	64..79	80..95	96..111	112..127	128..143	144..159	160..175	176..191	192..207	208..223	224..239	240..255
			// r		g		b		a		r		g		b		a			r			g			b			a			r			g			b			a
			
			auto color = v32_from_arith_seq(
				{ int16_t(tile.t0.r0 * 63.0f), int16_t(tile.t0.g0 * 63.0f), int16_t(tile.t0.b0 * 63.0f), int16_t(tile.t0.a0 * 63.0f) },
				{ int16_t(drdx * 63.0f),	   int16_t(dgdx * 63.0f),		int16_t(dbdx * 63.0f),		 int16_t(dadx * 63.0f) });
			
			//Vec16s rg
			for (int32_t y = 0; y != TILE_SIZE; ++y, a0 += tile.dady, b0 += tile.dbdy, c0 += tile.dcdy, span += bpsl_u32, zspan += XRes) {
				auto p_mask = (p_a | p_b | p_c) >= 0;
				if (horizontal_or(p_mask)) {

					// TODO? if mask is all zeroed, continue
					Vec8f p_z = approx_recipr(p_rz);

					auto z_candidate = (Vec8ui(0xFF80) - static_cast<Vec8ui>(roundi(g_zscale * p_z)));
					Vec8us z_existing_c;
					z_existing_c.load_a(zspan);
					auto z_existing = extend(z_existing_c);

					auto zmask = z_candidate > z_existing;

					p_mask &= zmask;

					if (horizontal_or(p_mask)) {

						*(__m128i*)zspan = _mm_blendv_epi8(*(__m128i*)zspan, compress(z_candidate), compress(p_mask));

						Vec8i u = roundi(p_uz * p_z * t0.UScaleFactor);
						Vec8i v = roundi(p_vz * p_z * t0.VScaleFactor);

						Vec8i tu = packed_tile_u(u, t0.LogHeight, t0_umask_swizzled);
						Vec8i tv = packed_tile_v(v, t0_vmask);

						auto p_offset = tu + tv;

						const auto texture_samples = colorize(gather(p_offset, t0.TextureAddr, p_mask), color);

						if constexpr (BlendMode == TBlendMode::TRANSPARENT) {
							Vec32uc dst;
							dst.load_a(span);
							texture_samples = (texture_samples >> 1) + (dst >> 1)
						}

						_mm256_maskstore_ps((float*)span, *(__m256i*)(&p_mask), *(__m256*)(&texture_samples));
					}
				}

				p_rz += Vec8f(drzdy);
				p_uz += Vec8f(t0.duzdy);
				p_vz += Vec8f(t0.dvzdy);

				color += Vec32sFromVec4s({ int16_t(drdy*63.0f), int16_t(dgdy * 63.0f), int16_t(dbdy * 63.0f), int16_t(dady * 63.0f) });

				p_a += Vec8i(tile.dady);
				p_b += Vec8i(tile.dbdy);
				p_c += Vec8i(tile.dcdy);
			}
		}

		uint32_t quantize_z(float z) {
			constexpr const float M = 0xff80 * 1024;
			float zrem = fmod(z * g_zscale * 1024.0f, M);
			if (zrem < 0.0f) {
				zrem += M;
			}
			return M - zrem;
		}

		uint32_t quantize_dz(float z) {
			constexpr const float M = 0xff80 * 1024;
			float zrem = fmod(z * g_zscale * 1024.0f, M);
			if (zrem < 0.0f) {
				zrem += M;
			}
			return -zrem;
		}


		template <TInterpolationType IType = TInterpolationType::QUADRATIC, TBlendMode BlendMode = TBlendMode::OVERWRITE>
		void apply(const barry::Tile& tile) {
			auto scanline = dstSurface + tile.y * TILE_SIZE * bpsl;
			auto zscanline = dstSurface + PageSize + tile.y * TILE_SIZE * XRes * 2;
			auto span = ((uint32_t *)scanline) + tile.x * TILE_SIZE;
			auto zspan = ((uint16_t*)zscanline) + tile.x * TILE_SIZE;
			auto bpsl_u32 = bpsl / sizeof(uint32_t);

			TScreenCoord a0 = tile.a0;
			TScreenCoord b0 = tile.b0;
			TScreenCoord c0 = tile.c0;

			float rz0 = tile.rz0;
			float uz0 = tile.t0.uz0;
			float vz0 = tile.t0.vz0;
			float cr0 = tile.t0.r0;
			float cg0 = tile.t0.g0;
			float cb0 = tile.t0.b0;

			uint32_t t0_umask = (1 << t0.LogWidth) - 1;
			uint32_t t0_vmask = (1 << t0.LogHeight) - 1;
			uint32_t t0_umask_tiled = tile_umask(t0.LogHeight, t0_umask);
			uint32_t t0_vmask_tiled = tile_vmask(t0_vmask);

			auto au = int32_t(uz0 / rz0 * 2048.0f * t0.UScaleFactor);
			auto av = int32_t(vz0 / rz0 * 2048.0f * t0.VScaleFactor);
			auto bu = int32_t((uz0 + t0.duzdx * 8.0f) / (rz0 + drzdx * 8.0f) * 2048.0f * t0.UScaleFactor);
			auto bv = int32_t((vz0 + t0.dvzdx * 8.0f) / (rz0 + drzdx * 8.0f) * 2048.0f * t0.VScaleFactor);
			auto cu = int32_t((uz0 + t0.duzdy * 8.0f) / (rz0 + drzdy * 8.0f) * 2048.0f * t0.UScaleFactor);
			auto cv = int32_t((vz0 + t0.dvzdy * 8.0f) / (rz0 + drzdy * 8.0f) * 2048.0f * t0.VScaleFactor);
			auto du = int32_t((uz0 + t0.duzdx * 8.0f + t0.duzdy * 8.0f) / (rz0 + drzdx * 8.0f + drzdy * 8.0f) * 2048.0f * t0.UScaleFactor);
			auto dv = int32_t((vz0 + t0.dvzdx * 8.0f + t0.dvzdy * 8.0f) / (rz0 + drzdx * 8.0f + drzdy * 8.0f) * 2048.0f * t0.VScaleFactor);

			auto az = 1.0f / rz0;
			auto bz = 1.0f / (rz0 + drzdx * 8.0f);
			auto cz = 1.0f / (rz0 + drzdy * 8.0f);
			auto dz = 1.0f / (rz0 + drzdx * 8.0f + drzdy * 8.0f);


			auto ar = uint32_t(cr0 * 2048.0f);
			auto ag = uint32_t(cg0 * 2048.0f);
			auto ab = uint32_t(cb0 * 2048.0f);
			auto br = uint32_t((cr0 + drdx * 8.0f) * 2048.0f);
			auto bg = uint32_t((cg0 + dgdx * 8.0f) * 2048.0f);
			auto bb = uint32_t((cb0 + dbdx * 8.0f) * 2048.0f);
			auto cr = uint32_t((cr0 + drdy * 8.0f) * 2048.0f);
			auto cg = uint32_t((cg0 + dgdy * 8.0f) * 2048.0f);
			auto cb = uint32_t((cb0 + dbdy * 8.0f) * 2048.0f);
			auto dr = uint32_t((cr0 + drdx * 8.0f + drdy * 8.0f) * 2048.0f);
			auto dg = uint32_t((cg0 + dgdx * 8.0f + dgdy * 8.0f) * 2048.0f);
			auto db = uint32_t((cb0 + dbdx * 8.0f + dbdy * 8.0f) * 2048.0f);

			auto aut = tile_u(au, t0.LogHeight, t0_umask);
			auto avt = tile_v(av, t0_vmask);

			auto au00 = au;
			auto au10 = (bu - au) / 8;
			auto au01 = (cu - au) / 8;
			auto av00 = av;
			auto av10 = (bv - av) / 8;
			auto av01 = (cv - av) / 8;

			auto az00 = quantize_z(az);
			auto az10 = quantize_dz((bz - az) / 8.0f);
			auto az01 = quantize_dz((cz - az) / 8.0f);

			auto ar00 = ar;
			auto ar10 = (br - ar) / 8;
			auto ar01 = (cr - ar) / 8;

			auto ag00 = ag;
			auto ag10 = (bg - ag) / 8;
			auto ag01 = (cg - ag) / 8;

			auto ab00 = ab;
			auto ab10 = (bb - ab) / 8;
			auto ab01 = (cb - ab) / 8;

			int32_t au11, av11, az11, ar11, ag11, ab11;
			if constexpr (IType == TInterpolationType::QUADRATIC) {
				au11 = (du - bu - cu + au) / 64;
				av11 = (dv - bv - cv + av) / 64;
				ar11 = (dr - br - cr + ar) / 64;
				ag11 = (dg - bg - cg + ag) / 64;
				ab11 = (db - bb - cb + ab) / 64;
			}
			//az11 = quantize_ddz((dz - bz - cz + az) / 64.0f);
			az11 = 0;

			auto dux0 = tile_du(au10, t0.LogHeight, t0_umask);
			auto dvx0 = tile_dv(av10, t0_vmask);
			auto duy = tile_du(au01, t0.LogHeight, t0_umask);
			auto dvy = tile_dv(av01, t0_umask);
			uint32_t dzx0 = az10;
			uint32_t dzy = az01;

			uint32_t drx0 = ar10;
			uint32_t dry = ar01;
			uint32_t dgx0 = ag10;
			uint32_t dgy = ag01;
			uint32_t dbx0 = ab10;
			uint32_t dby = ab01;

			uint32_t dduxy, ddvxy, ddzxy, ddrxy, ddgxy, ddbxy;
			if constexpr (IType == TInterpolationType::QUADRATIC) {
				// we can't use tile_d? here as that would mess up the carry trick
				dduxy = tile_u(0, t0.LogHeight, t0_umask);
				ddvxy = tile_v(0, t0_vmask);
				ddrxy = ddgxy = ddbxy = 0;
			}
			ddzxy = 0;

			uint32_t u0 = aut;
			uint32_t v0 = avt;
			//uint32_t z0 = az00;
			uint32_t r0_ = ar;
			uint32_t g0_ = ag;
			uint32_t b0_ = ab;
			for (int32_t y = 0; y != TILE_SIZE; ++y, a0 += tile.dady, b0 += tile.dbdy, c0 += tile.dcdy, span += bpsl_u32, zspan += XRes) {
				TScreenCoord a = a0;
				TScreenCoord b = b0;
				TScreenCoord c = c0;

				auto u = u0;
				auto v = v0;
				//auto z = z0;
				auto rz = rz0;
				auto r_ = r0_;
				auto g_ = g0_;
				auto b_ = b0_;
				auto dux = dux0;
				auto dvx = dvx0;
				auto dzx = dzx0;
				auto drx = drx0;
				auto dgx = dgx0;
				auto dbx = dbx0;

				uint16_t* pz = zspan;
				for (uint32_t* p = span; p != span + TILE_SIZE; ++p, ++pz,a += tile.dadx, b += tile.dbdx, c += tile.dcdx) {
					if ((a | b | c) >= 0) {
						auto z = quantize_z(1.0f / rz);
						auto wz = (z >> 10) & 0xffff;
						if (wz > *pz)
						{
							*pz = wz;
							auto offset = (u + v) >> 12;
							auto output = t0.TextureAddr[offset];
							byte* color = (byte *)&output;
							color[0] = ((uint16_t(color[0]) * (uint16_t((r_) >> 11) & 0xff))) >> 8;
							color[1] = ((uint16_t(color[1]) * (uint16_t((g_) >> 11) & 0xff))) >> 8;
							color[2] = ((uint16_t(color[2]) * (uint16_t((b_) >> 11) & 0xff))) >> 8;


							if constexpr (BlendMode == TBlendMode::XOR) {
								*p ^= output;
							} else {
								*p = output;
							}
						}
					}

					if constexpr (IType == TInterpolationType::QUADRATIC) {
						dux += dduxy;
						dux &= t0_umask_tiled;
						dvx += ddvxy;
						dvx &= t0_vmask_tiled;
						drx += ddrxy;
						dgx += ddgxy;
						dbx += ddbxy;
					}
					dzx += ddzxy;
					u += dux;
					u &= t0_umask_tiled;
					v += dvx;
					v &= t0_vmask_tiled;
					//z += dzx;
					rz += drzdx;
					r_ += drx;
					g_ += dgx;
					b_ += dbx;
				}

				u0 += duy;
				u0 &= t0_umask_tiled;
				v0 += dvy;
				v0 &= t0_vmask_tiled;
				//z0 += dzy;
				rz0 += drzdy;
				r0_ += dry;
				g0_ += dgy;
				b0_ += dby;
				if constexpr (IType == TInterpolationType::QUADRATIC) {
					dduxy += au11;
					ddvxy += av11;
					ddrxy += ar11;
					ddgxy += ag11;
					ddbxy += ab11;
				}
				ddzxy += az11;
			}
		}
	};

	constexpr const int8_t SUBPIXEL_BITS = 8;
	constexpr const float SUBPIXEL_MULT = 256.0f;

	TScreenCoord orient2d(
		TScreenCoord ax, TScreenCoord ay,
		TScreenCoord bx, TScreenCoord by,
		TScreenCoord cx, TScreenCoord cy)
	{
		return (int64_t(bx - ax) * int64_t(cy - ay) - int64_t(by - ay) * int64_t(cx - ax)) >> SUBPIXEL_BITS;
	}

	template <typename TTileRasterizer, TBlendMode BlendMode>
	void rasterize_triangle(TTileRasterizer &rasterizer, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		// FIXME: raster conventions (it is doing floor right now)
		const int tile_mx = rasterizer.clampedX(std::min({ v1.PX, v2.PX, v3.PX })) / TILE_SIZE;
		const int tile_Mx = rasterizer.clampedX(std::max({ v1.PX, v2.PX, v3.PX })) / TILE_SIZE;
		const int tile_my = rasterizer.clampedY(std::min({ v1.PY, v2.PY, v3.PY })) / TILE_SIZE;
		const int tile_My = rasterizer.clampedY(std::max({ v1.PY, v2.PY, v3.PY })) / TILE_SIZE;

		TScreenCoord v1x = TScreenCoord(v1.PX * SUBPIXEL_MULT + 0.5);
		TScreenCoord v1y = TScreenCoord(v1.PY * SUBPIXEL_MULT + 0.5);
		TScreenCoord v2x = TScreenCoord(v2.PX * SUBPIXEL_MULT + 0.5);
		TScreenCoord v2y = TScreenCoord(v2.PY * SUBPIXEL_MULT + 0.5);
		TScreenCoord v3x = TScreenCoord(v3.PX * SUBPIXEL_MULT + 0.5);
		TScreenCoord v3y = TScreenCoord(v3.PY * SUBPIXEL_MULT + 0.5);

		TScreenCoord x0 = tile_mx * TILE_SIZE << SUBPIXEL_BITS;
		TScreenCoord y0 = tile_my * TILE_SIZE << SUBPIXEL_BITS;
		TScreenCoord _a0 = orient2d(v2x, v2y, v1x, v1y, x0, y0);
		TScreenCoord _b0 = orient2d(v3x, v3y, v2x, v2y, x0, y0);
		TScreenCoord _c0 = orient2d(v1x, v1y, v3x, v3y, x0, y0);

		TScreenCoord dadx = (v2y - v1y);
		TScreenCoord dady = (v1x - v2x);
		TScreenCoord dbdx = (v3y - v2y);
		TScreenCoord dbdy = (v2x - v3x);
		TScreenCoord dcdx = (v1y - v3y);
		TScreenCoord dcdy = (v3x - v1x);

		// flat without tiling
		//for (int y = tile_my * TILE_SIZE; y <= tile_My * TILE_SIZE + TILE_SIZE - 1; ++y) {
		//	byte* scanline = rasterizer.dstSurface + y * rasterizer.bpsl;
		//	for (int x = tile_mx * TILE_SIZE; x <= tile_Mx * TILE_SIZE + TILE_SIZE - 1; ++x) {
		//		TScreenCoord alpha = orient2d(v2x, v2y, v1x, v1y, x << SUBPIXEL_BITS, y << SUBPIXEL_BITS);
		//		TScreenCoord beta = orient2d(v3x, v3y, v2x, v2y, x << SUBPIXEL_BITS, y << SUBPIXEL_BITS);
		//		TScreenCoord gamma = orient2d(v1x, v1y, v3x, v3y, x << SUBPIXEL_BITS, y << SUBPIXEL_BITS);
		//		uint32_t& pixel = ((uint32_t*)scanline)[x];
		//		if (alpha >= 0 && beta >= 0 && gamma >= 0) {
		//			pixel = 0xcdefab;
		//		} /*else if (pixel == 0) {
		//			pixel = 0x123456;
		//		}*/
		//	}
		//}
//		/*
		// this is constant across entire triangle
		int i = 0;
		float zoltek = 1.0f / (_a0 + _b0 + _c0);
		for (int y = tile_my; y <= tile_My; ++y, _a0 += TILE_SIZE * dady, _b0 += TILE_SIZE * dbdy, _c0 += TILE_SIZE * dcdy, ++i) {
			TScreenCoord a0 = _a0;
			TScreenCoord b0 = _b0;
			TScreenCoord c0 = _c0;
			for (int x = tile_mx; x <= tile_Mx; ++x, a0 += TILE_SIZE * dadx, b0 += TILE_SIZE * dbdx, c0 += TILE_SIZE * dcdx, ++i) {
				TScreenCoord max_a = a0 + ((dadx > 0) ? dadx * TILE_SIZE : 0) + ((dady > 0) ? dady * TILE_SIZE : 0);
				TScreenCoord max_b = b0 + ((dbdx > 0) ? dbdx * TILE_SIZE : 0) + ((dbdy > 0) ? dbdy * TILE_SIZE : 0);
				TScreenCoord max_c = c0 + ((dcdx > 0) ? dcdx * TILE_SIZE : 0) + ((dcdy > 0) ? dcdy * TILE_SIZE : 0);

				if ((max_a | max_b | max_c) >= 0) {
					// FIXME: define outside and maintain
					Tile tile = {
						.x = x,
						.y = y,
						.a0 = a0,
						.dadx = dadx,
						.dady = dady,
						.b0 = b0,
						.dbdx = dbdx,
						.dbdy = dbdy,
						.c0 = c0,
						.dcdx = dcdx,
						.dcdy = dcdy,
						.rz0 = (v1.RZ + (x * TILE_SIZE - v1.PX) * rasterizer.drzdx + (y * TILE_SIZE - v1.PY) * rasterizer.drzdy),
						//.rz0 = (v1.RZ * b0 + v2.RZ * c0 + v3.RZ * a0) * zoltek,
						.t0 = {
							.uz0 = (v1.UZ + (x * TILE_SIZE - v1.PX) * rasterizer.t0.duzdx + (y * TILE_SIZE - v1.PY) * rasterizer.t0.duzdy),
							.vz0 = (v1.VZ + (x * TILE_SIZE - v1.PX) * rasterizer.t0.dvzdx + (y * TILE_SIZE - v1.PY) * rasterizer.t0.dvzdy),
							//.uz0 = (v1.UZ * b0 + v2.UZ * c0 + v3.UZ * a0) * zoltek,
							//.vz0 = (v1.VZ * b0 + v2.VZ * c0 + v3.VZ * a0) * zoltek,
							.r0 = (v1.LR + (x * TILE_SIZE - v1.PX) * rasterizer.drdx + (y * TILE_SIZE - v1.PY) * rasterizer.drdy),
							.g0 = (v1.LG + (x * TILE_SIZE - v1.PX) * rasterizer.dgdx + (y * TILE_SIZE - v1.PY) * rasterizer.dgdy),
							.b0 = (v1.LB + (x * TILE_SIZE - v1.PX) * rasterizer.dbdx + (y * TILE_SIZE - v1.PY) * rasterizer.dbdy),
							.a0 = (v1.LA + (x * TILE_SIZE - v1.PX) * rasterizer.dbdx + (y * TILE_SIZE - v1.PY) * rasterizer.dady),
						}
					};

					//if ((((x ^ y) ) & 1)) 
					{
						rasterizer.apply_exact<BlendMode>(tile);
					}
					//rasterizer.apply<TInterpolationType::QUADRATIC, TBlendMode::OVERWRITE>(tile);
					//rasterizer.apply<TInterpolationType::QUADRATIC, TBlendMode::XOR>(tile);
					//for (int py = y * TILE_SIZE; py <= (y + 1) * TILE_SIZE - 1; ++py) {
					//	byte* scanline = rasterizer.dstSurface + py * rasterizer.bpsl;
					//	for (int px = x * TILE_SIZE; px <= (x + 1) * TILE_SIZE - 1; ++px) {
					//		uint32_t& pixel = ((uint32_t*)scanline)[px];
					//		pixel += 0x2c1cde;
					//	}
					//}
				}
			}
		} //*/

		/*const float m[4] = {
			(v2.x - v1.x) / TILE_SIZE, (v2.y - v1.y) / TILE_SIZE,
			(v3.x - v1.x) / TILE_SIZE, (v3.y - v1.y) / TILE_SIZE
		};
		const float det = m[0] * m[3] - m[1] * m[2];
		const float r[4] = {
			 m[3] / det, -m[1] / det,
			-m[2] / det,  m[0] / det
		};

		// interpolants for alpha*w (alpha/z)
		const float w_dx = r[0] * (v2.w - v1.w) + r[1] * (v3.w - v1.w);
		const float w_dy = r[2] * (v2.w - v1.w) + r[3] * (v3.w - v1.w);
		const float aw_dx = r[0] * v2.w;
		const float aw_dy = r[2] * v2.w;
		const float bw_dx = r[1] * v3.w;
		const float bw_dy = r[3] * v3.w;

		// set the origin at (mx, my) for convenience
		float w_0 = v1.w + (tile_mx - v1.x / TILE_SIZE) * w_dx + (tile_my - v1.y / TILE_SIZE) * w_dy;
		float aw_0 = (tile_mx - v1.x / TILE_SIZE) * aw_dx + (tile_my - v1.y / TILE_SIZE) * aw_dy;
		float bw_0 = (tile_mx - v1.x / TILE_SIZE) * bw_dx + (tile_my - v1.y / TILE_SIZE) * bw_dy;

		// per-pixel deltas

		for (int y = tile_my; y <= tile_My; ++y, w_0 += w_dy, aw_0 += aw_dy, bw_0 += bw_dy) {
			// tile vertices are numbered like this
			// 0 1
			// 2 3
			// in each iteration on the x axis, we move the values for 1, 3 into 0, 2 and calc new 1, 3
			float a1 = aw_0 / w_0;
			float b1 = bw_0 / w_0;
			float a3 = (aw_0 + aw_dy) / (w_0 + w_dy);
			float b3 = (bw_0 + bw_dy) / (w_0 + w_dy);
			int x = tile_mx; float w = w_0 + w_dx; float aw = aw_0 + aw_dx; float bw = bw_0 + bw_dx;
			for (
				;
				x <= tile_Mx;
				++x, w += w_dx, aw += aw_dx, bw += bw_dx
				) {
				const float a0 = a1;
				const float b0 = b1;
				const float a2 = a3;
				const float b2 = b3;
				a1 = aw / w;
				b1 = bw / w;
				a3 = (aw + aw_dy) / (w + w_dy);
				b3 = (bw + bw_dy) / (w + w_dy);
				Tile tile = {
					.x = x,
					.y = y,
					.a0 = a0,
					.dadx = (a1 - a0) / TILE_SIZE,
					.dady = (a2 - a0) / TILE_SIZE,
					.b0 = b0,
					.dbdx = (b1 - b0) / TILE_SIZE,
					.dbdy = (b2 - b0) / TILE_SIZE
				};
				float min_a = a0 + ((tile.dadx < 0) ? tile.dadx * TILE_SIZE : 0) + ((tile.dady < 0) ? tile.dady * TILE_SIZE : 0);
				float max_a = a0 + ((tile.dadx > 0) ? tile.dadx * TILE_SIZE : 0) + ((tile.dady > 0) ? tile.dady * TILE_SIZE : 0);
				float min_b = b0 + ((tile.dbdx < 0) ? tile.dbdx * TILE_SIZE : 0) + ((tile.dbdy < 0) ? tile.dbdy * TILE_SIZE : 0);
				float max_b = b0 + ((tile.dbdx > 0) ? tile.dbdx * TILE_SIZE : 0) + ((tile.dbdy > 0) ? tile.dbdy * TILE_SIZE : 0);

				if (max_a > 0 && max_b > 0 && min_a + min_b < 1) {
					rasterizer.apply(tile);
				}
			}
		}*/
	}

} // namespace barry

template <TBlendMode BlendMode>
void TheOtherBarry(Face* F, Vertex** V, dword numVerts, dword miplevel) {
	//for (dword i = 0; i < numVerts; ++i) {
	//	float z = 1.0f / V[i]->RZ;
	//	V[i]->U = V[i]->UZ * z;
	//	V[i]->V = V[i]->VZ * z;
	//}
	barry::TileRasterizer r(V, VPage, VESA_BPSL, XRes, YRes, F->Txtr->Txtr, miplevel);

	for (dword i = 2; i < numVerts; ++i) {
		//r.setVertexIndexes(0, i - 1, i);

		const auto& v1 = *(V[0]);
		const auto& v2 = *(V[i - 1]);
		const auto& v3 = *(V[i]);

		float m[4] = {
			v2.PX - v1.PX, v2.PY - v1.PY,
			v3.PX - v1.PX, v3.PY - v1.PY
		};
		const float det = m[0] * m[3] - m[1] * m[2];
		if (fabs(det) <= 0.01f) continue;
		const float im[4] = {
			 m[3] / det, -m[1] / det,
			-m[2] / det,  m[0] / det
		};
		r.drzdx = im[0] * (v2.RZ - v1.RZ) + im[1] * (v3.RZ - v1.RZ);
		r.drzdy = im[2] * (v2.RZ - v1.RZ) + im[3] * (v3.RZ - v1.RZ);
		r.t0.duzdx = im[0] * (v2.UZ - v1.UZ) + im[1] * (v3.UZ - v1.UZ);
		r.t0.duzdy = im[2] * (v2.UZ - v1.UZ) + im[3] * (v3.UZ - v1.UZ);
		r.t0.dvzdx = im[0] * (v2.VZ - v1.VZ) + im[1] * (v3.VZ - v1.VZ);
		r.t0.dvzdy = im[2] * (v2.VZ - v1.VZ) + im[3] * (v3.VZ - v1.VZ);

		r.dadx = (im[0] * (float(v2.LA) - float(v1.LA)) + im[1] * (float(v3.LA) - float(v1.LA)));
		r.dady = (im[2] * (float(v2.LA) - float(v1.LA)) + im[3] * (float(v3.LA) - float(v1.LA)));
		r.drdx = (im[0] * (float(v2.LR) - float(v1.LR)) + im[1] * (float(v3.LR) - float(v1.LR)));
		r.drdy = (im[2] * (float(v2.LR) - float(v1.LR)) + im[3] * (float(v3.LR) - float(v1.LR)));
		r.dgdx = (im[0] * (float(v2.LG) - float(v1.LG)) + im[1] * (float(v3.LG) - float(v1.LG)));
		r.dgdy = (im[2] * (float(v2.LG) - float(v1.LG)) + im[3] * (float(v3.LG) - float(v1.LG)));
		r.dbdx = (im[0] * (float(v2.LB) - float(v1.LB)) + im[1] * (float(v3.LB) - float(v1.LB)));
		r.dbdy = (im[2] * (float(v2.LB) - float(v1.LB)) + im[3] * (float(v3.LB) - float(v1.LB)));
		r.umask = (1 << r.t0.LogWidth) - 1;
		r.vmask = (1 << r.t0.LogHeight) - 1;

		barry::rasterize_triangle<barry::TileRasterizer>(r, v1, v2, v3);
	}
}
