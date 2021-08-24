#include "TheOtherBarry.h"

#include <intrin.h>
#include <immintrin.h>
#include <SIMD/vectorclass.h>

namespace barry {
	constexpr const int32_t TILE_SIZE = 8;

	struct alignas(16) RVector4 {
		// xyz taken from TPos, w is 1/z
		float x, y, z, w;

		static RVector4 fromVertex(const Vertex* v) {
			return RVector4 { v->PX , v->PY , 1.0f / v->RZ, v->RZ };
		}
	};

	struct Tile {
		int x, y;

		float a0, dadx, dady; // , dadxdy;
		float b0, dbdx, dbdy; // , dbdxdy;
	};

	using Triangle = RVector4[3];

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

	static const auto arith_seq_mult = Vec8f{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };

	__m256 m256_from_arith_seq(float x_, float d_) {
		auto x = Vec8f { x_ };
		auto d = Vec8f { d_ };
		return mul_add(d, arith_seq_mult, x);
		//return {
		//	x,
		//	x + d,
		//	x + d + d,
		//	x + d + d + d,
		//	x + d + d + d + d,
		//	x + d + d + d + d + d,
		//	x + d + d + d + d + d + d,
		//	x + d + d + d + d + d + d + d,
		//};
	}

	static inline Vec8ui gather(const Vec8ui index, void const* table, Vec8ib mask) {
#if INSTRSET >= 8
		return _mm256_mask_i32gather_epi32((const int*)table, static_cast<__m256i>(index), static_cast<__m256i>(mask), 4);
#else
		auto t = (const uint32_t *)table;
		uint32_t ind[8];
		index.store(ind);
		uint32_t m[8];
		mask.store(m);

		//return Vec8ui(t[ind[0]], t[ind[1]], t[ind[2]], t[ind[3]],
		//			  t[ind[4]], t[ind[5]], t[ind[6]], t[ind[7]]); // ignore mask

		return Vec8ui(m[0] ? t[ind[0]] : 0, m[1] ? t[ind[1]] : 0, m[2] ? t[ind[2]] : 0, m[3] ? t[ind[3]] : 0,
					  m[4] ? t[ind[4]] : 0, m[5] ? t[ind[5]] : 0, m[6] ? t[ind[6]] : 0, m[7] ? t[ind[7]] : 0);
#endif
	}


	Vec8ui m256i_from_arith_seq_tiled(uint32_t x0, uint32_t dx, uint32_t mask) {
		const uint32_t x1 = (x0 + dx) & mask;
		const uint32_t x2 = (x1 + dx) & mask;
		const uint32_t x3 = (x2 + dx) & mask;
		const uint32_t x4 = (x3 + dx) & mask;
		const uint32_t x5 = (x4 + dx) & mask;
		const uint32_t x6 = (x5 + dx) & mask;
		const uint32_t x7 = (x6 + dx) & mask;
		return Vec8ui{ x0, x1, x2, x3, x4, x5, x6, x7 };
		//return _mm256_set_epi32(x7, x6, x5, x4, x3, x2, x1, x0);
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
			long LogWidth;
			long LogHeight;
			float UScaleFactor;
			float VScaleFactor;
		};
		TextureInfo t0;
		size_t v0 = 0 , v1 = 0, v2 = 0;

		void setVertexIndexes(size_t v0, size_t v1, size_t v2) {
			this->v0 = v0;
			this->v1 = v1;
			this->v2 = v2;
		}

		int32_t clampedX(int32_t x) {
			return std::min(std::max(x, 0), xres - 1);
		}
		int32_t clampedY(int32_t y) {
			return std::min(std::max(y, 0), yres - 1);
		}

		// reference impl
		//void apply(const barry::Tile& tile) {
		//	float t0_dudy_f = (V[v1]->U - V[v0]->U) * tile.dady + (V[v2]->U - V[v0]->U) * tile.dbdy;
		//	float t0_dvdy_f = (V[v1]->V - V[v0]->V) * tile.dady + (V[v2]->V - V[v0]->V) * tile.dbdy;

		//	float t0_dudx_f = (V[v1]->U - V[v0]->U) * tile.dadx + (V[v2]->U - V[v0]->U) * tile.dbdx;
		//	float t0_dvdx_f = (V[v1]->V - V[v0]->V) * tile.dadx + (V[v2]->V - V[v0]->V) * tile.dbdx;

		//	float a0 = tile.a0;
		//	float b0 = tile.b0;

		//	float t0_u0_f = V[v0]->U + t0_dudy_f * (tile.y * TILE_SIZE - V[v0]->PY) + t0_dudx_f * (tile.x * TILE_SIZE - V[v0]->PX);
		//	float t0_v0_f = V[v0]->V + t0_dvdy_f * (tile.y * TILE_SIZE - V[v0]->PY) + t0_dvdx_f * (tile.x * TILE_SIZE - V[v0]->PX);

		//	int32_t t0_dudy = (int32_t)(2048.0 * t0_dudy_f * t0.UScaleFactor);
		//	int32_t t0_dvdy = (int32_t)(2048.0 * t0_dvdy_f * t0.VScaleFactor);

		//	int32_t t0_dudx = (int32_t)(2048.0 * t0_dudx_f * t0.UScaleFactor);
		//	int32_t t0_dvdx = (int32_t)(2048.0 * t0_dvdx_f * t0.VScaleFactor);

		//	int32_t t0_u0 = (int32_t)(2048.0 * t0_u0_f * t0.UScaleFactor);
		//	int32_t t0_v0 = (int32_t)(2048.0 * t0_v0_f * t0.VScaleFactor);

		//	int32_t t0_umask = (1 << t0.LogWidth) - 1;
		//	int32_t t0_vmask = (1 << t0.LogHeight) - 1;

		//	uint32_t t0_u0_bt = tile_u(t0_u0, t0.LogHeight, t0_umask);
		//	uint32_t t0_v0_bt = tile_v(t0_v0, t0_vmask);

		//	uint32_t t0_dudx_bt = tile_du(t0_dudx, t0.LogHeight, t0_umask);
		//	uint32_t t0_dvdx_bt = tile_dv(t0_dvdx, t0_vmask);

		//	uint32_t t0_dudy_bt = tile_du(t0_dudy, t0.LogHeight, t0_umask);
		//	uint32_t t0_dvdy_bt = tile_dv(t0_dvdy, t0_vmask);

		//	uint32_t t0_umask_bt = tile_umask(t0.LogHeight, t0_umask);
		//	uint32_t t0_vmask_bt = tile_vmask(t0_vmask);

		//	byte* scanline = dstSurface + tile.y * TILE_SIZE * bpsl;
		//	for (size_t j = 0; j != TILE_SIZE; ++j) {
		//		float a = a0;
		//		float b = b0;
		//		uint32_t t0_u_bt = t0_u0_bt;
		//		uint32_t t0_v_bt = t0_v0_bt;
		//		dword* span = ((dword*)scanline) + tile.x * TILE_SIZE;
		//		for (size_t i = 0; i != TILE_SIZE; ++i) {
		//			auto t0_offset = (t0_v_bt >> 12); //(t0_u_bt + t0_v_bt) >> 12;
		//			if (a >= 0 && b >= 0 && a + b < 1) {
		//				span[i] = t0.TextureAddr[t0_offset];
		//			}

		//			a += tile.dadx;
		//			b += tile.dbdx;
		//			t0_u_bt += t0_dudx_bt;
		//			t0_v_bt += t0_dvdx_bt;
		//			t0_u_bt &= t0_umask_bt;
		//			t0_v_bt &= t0_vmask_bt;
		//		}
		//		a0 += tile.dady;
		//		b0 += tile.dbdy;
		//		t0_u0_bt += t0_dudy_bt;
		//		t0_v0_bt += t0_dvdy_bt;
		//		t0_u0_bt &= t0_umask_bt;
		//		t0_v0_bt &= t0_vmask_bt;

		//		scanline += bpsl;
		//	}
		//}

		void apply(const barry::Tile& tile) {
			float a0 = tile.a0;
			float b0 = tile.b0;

			float t0_dudy_f = (V[v1]->U - V[v0]->U) * tile.dady + (V[v2]->U - V[v0]->U) * tile.dbdy;
			float t0_dvdy_f = (V[v1]->V - V[v0]->V) * tile.dady + (V[v2]->V - V[v0]->V) * tile.dbdy;

			float t0_dudx_f = (V[v1]->U - V[v0]->U) * tile.dadx + (V[v2]->U - V[v0]->U) * tile.dbdx;
			float t0_dvdx_f = (V[v1]->V - V[v0]->V) * tile.dadx + (V[v2]->V - V[v0]->V) * tile.dbdx;

			float t0_u0_f = V[v0]->U + t0_dudy_f * (tile.y * TILE_SIZE - V[v0]->PY) + t0_dudx_f * (tile.x * TILE_SIZE - V[v0]->PX);
			float t0_v0_f = V[v0]->V + t0_dvdy_f * (tile.y * TILE_SIZE - V[v0]->PY) + t0_dvdx_f * (tile.x * TILE_SIZE - V[v0]->PX);

			int32_t t0_dudy = (int32_t)(2048.0 * t0_dudy_f * t0.UScaleFactor);
			int32_t t0_dvdy = (int32_t)(2048.0 * t0_dvdy_f * t0.VScaleFactor);

			int32_t t0_dudx = (int32_t)(2048.0 * t0_dudx_f * t0.UScaleFactor);
			int32_t t0_dvdx = (int32_t)(2048.0 * t0_dvdx_f * t0.VScaleFactor);

			int32_t t0_u0 = (int32_t)(2048.0 * t0_u0_f * t0.UScaleFactor);
			int32_t t0_v0 = (int32_t)(2048.0 * t0_v0_f * t0.VScaleFactor);

			int32_t t0_umask = (1 << t0.LogWidth) - 1;
			int32_t t0_vmask = (1 << t0.LogHeight) - 1;

			uint32_t t0_u0_tiled = tile_u(t0_u0, t0.LogHeight, t0_umask);
			uint32_t t0_v0_tiled = tile_v(t0_v0, t0_vmask);

			uint32_t t0_dudx_tiled = tile_du(t0_dudx, t0.LogHeight, t0_umask);
			uint32_t t0_dvdx_tiled = tile_dv(t0_dvdx, t0_vmask);

			uint32_t t0_dudy_tiled = tile_du(t0_dudy, t0.LogHeight, t0_umask);
			uint32_t t0_dvdy_tiled = tile_dv(t0_dvdy, t0_vmask);

			uint32_t t0_umask_tiled = tile_umask(t0.LogHeight, t0_umask);
			uint32_t t0_vmask_tiled = tile_vmask(t0_vmask);

			// 8-pixel deltas are currently unused because TILE_SIZE = 8
			//int32_t t0_dudx_8 = (int32_t)(2048.0 * t0_dudx_f * t0.UScaleFactor * 8.0);
			//int32_t t0_dvdx_8 = (int32_t)(2048.0 * t0_dvdx_f * t0.VScaleFactor * 8.0);
			//uint32_t t0_dudx_8_tiled = tile_du(t0_dudx_8, t0.LogHeight, t0_umask);
			//uint32_t t0_dvdx_8_tiled = tile_dv(t0_dvdx_8, t0_vmask);

			__m256 v_a0 = m256_from_arith_seq(a0, tile.dadx);
			__m256 v_b0 = m256_from_arith_seq(b0, tile.dbdx);

			const __m256 v_zero = _mm256_setzero_ps();
			const __m256 v_one = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

			__m256 v_dady = _mm256_set1_ps(tile.dady);
			__m256 v_dbdy = _mm256_set1_ps(tile.dbdy);

			auto v_t0_u0_tiled = m256i_from_arith_seq_tiled(t0_u0_tiled, t0_dudx_tiled, t0_umask_tiled);
			auto v_t0_v0_tiled = m256i_from_arith_seq_tiled(t0_v0_tiled, t0_dvdx_tiled, t0_vmask_tiled);

			auto v_t0_dudy_tiled = Vec8ui(t0_dudy_tiled);
			auto v_t0_dvdy_tiled = Vec8ui(t0_dvdy_tiled);

			auto v_t0_umask_tiled = Vec8ui(t0_umask_tiled);
			auto v_t0_vmask_tiled = Vec8ui(t0_vmask_tiled);

			byte* scanline = dstSurface + tile.y * TILE_SIZE * bpsl;
			for (size_t j = 0; j != TILE_SIZE; ++j) {
				__m256 v_a = v_a0;
				__m256 v_b = v_b0;
				auto v_t0_u_tiled = v_t0_u0_tiled;
				auto v_t0_v_tiled = v_t0_v0_tiled;
				auto span = ((Vec8ui*)scanline) + tile.x * TILE_SIZE / 8;

				for (size_t i = 0; i != TILE_SIZE; i += 8) {
					//const __m256i v_t0_offsets_frac = v_t0_v_tiled;
					const auto v_t0_offsets_frac = v_t0_u_tiled + v_t0_v_tiled;
					const auto v_t0_offsets = v_t0_offsets_frac >> 12;
					const __m256 v_ab = _mm256_add_ps(v_a, v_b);
					const __m256 pass0 = _mm256_cmp_ps(v_a, v_zero, _CMP_NLE_UQ);
					const __m256 pass1 = _mm256_cmp_ps(v_b, v_zero, _CMP_NLE_UQ);
					const __m256 pass2 = _mm256_cmp_ps(v_ab, v_one, _CMP_NGE_UQ);

					const __m256 pass = _mm256_and_ps(_mm256_and_ps(pass0, pass1), pass2);
					const auto pass_mask = *(Vec8ib*)(&pass);

					const auto texture_samples = gather(v_t0_offsets, t0.TextureAddr, pass_mask); // _mm256_i32gather_epi32((const int*)t0.TextureAddr, v_t0_offsets, 4);
					//uint32_t texture_samples_i32[8] = {
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 0)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 1)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 2)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 3)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 4)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 5)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 6)],
					//	t0.TextureAddr[_mm256_extract_epi32(v_t0_offsets, 7)]
					//};
					//__m256i texture_samples = _mm256_loadu_epi32(texture_samples_i32);

					//__m256i output = _mm256_set1_epi32(0xffffff);
					auto output = texture_samples;

					//Vec8ui prior;
					//prior.load_a(span);
					//auto result = select(pass_mask, output, prior);
					//auto result = _mm256_blendv_epi8(prior, output, pass_mask);
//					auto result = output;

					//result.store_a(span); // Fucking A!
					_mm256_maskstore_ps((float*)span, *(__m256i*)(&pass), *(__m256*)(&output));
//					_mm256_store_si256(span, result);
					//if (a >= 0 && b >= 0 && a + b < 1) {
					//	span[i] = 0xffffff; //t0.TextureAddr[t0_offset];
					//}
					// NOTE: removing per-8-pixel interpolation, we'll be stuck with TILE_SIZE = 8 for now
				}
				v_a0 = _mm256_add_ps(v_a0, v_dady);
				v_b0 = _mm256_add_ps(v_b0, v_dbdy);

				//v_t0_u0_tiled = _mm256_add_epi32(v_t0_u0_tiled, v_t0_dudy_tiled);
				//v_t0_v0_tiled = _mm256_add_epi32(v_t0_v0_tiled, v_t0_dvdy_tiled);
				//v_t0_u0_tiled = _mm256_and_si256(v_t0_u0_tiled, v_t0_umask_tiled);
				//v_t0_v0_tiled = _mm256_and_si256(v_t0_v0_tiled, v_t0_vmask_tiled);
				v_t0_u0_tiled += v_t0_dudy_tiled;
				v_t0_v0_tiled += v_t0_dvdy_tiled;
				v_t0_u0_tiled &= v_t0_umask_tiled;
				v_t0_v0_tiled &= v_t0_vmask_tiled;


				scanline += bpsl;
			}
		}
	};

	template <typename TTileRasterizer>
	void rasterize_triangle(TTileRasterizer rasterizer, const RVector4& v1, const RVector4& v2, const RVector4& v3) {
		const int tile_mx = rasterizer.clampedX(std::min({ v1.x, v2.x, v3.x})) / TILE_SIZE;
		const int tile_Mx = rasterizer.clampedX(std::max({ v1.x, v2.x, v3.x})) / TILE_SIZE;
		const int tile_my = rasterizer.clampedY(std::min({ v1.y, v2.y, v3.y})) / TILE_SIZE;
		const int tile_My = rasterizer.clampedY(std::max({ v1.y, v2.y, v3.y})) / TILE_SIZE;

		const float m[4] = {
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
		}
	}

} // namespace barry


void TheOtherBarry(Face* F, Vertex** V, dword numVerts, dword miplevel) {
	for (dword i = 0; i < numVerts; ++i) {
		float z = 1.0f / V[i]->RZ;
		V[i]->U = V[i]->UZ * z;
		V[i]->V = V[i]->VZ * z;
	}
	barry::TileRasterizer r(V, VPage, VESA_BPSL, XRes, YRes, F->Txtr->Txtr, miplevel);
	for (dword i = 2; i < numVerts; ++i) {
		r.setVertexIndexes(0, i - 1, i);
		auto v0 = barry::RVector4::fromVertex(V[0]);
		auto v1 = barry::RVector4::fromVertex(V[i - 1]);
		auto v2 = barry::RVector4::fromVertex(V[i]);
		barry::rasterize_triangle(r, v0, v1, v2);
	}
}
