#include "TheOtherBarry.h"

namespace barry {
	constexpr const int32_t TILE_SIZE = 8;

	struct alignas(16) RVector4 {
		// xyz taken from TPos, w is 1/z
		float x, y, z, w;

		static RVector4 fromVertex(const Vertex* v) {
			return RVector4 { v->PX / v->RZ, v->PY / v->RZ, 1.0f / v->RZ, v->RZ };
		}
	};

	struct Tile {
		int x, y;

		float a0, dadx, dady; // , dadxdy;
		float b0, dbdx, dbdy; // , dbdxdy;
	};

	using Triangle = RVector4[3];

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

		void apply(const barry::Tile& tile) {
			float t0_dudy_f = (V[v1]->U - V[v0]->U) * tile.dady + (V[v2]->U - V[v0]->U) * tile.dbdy;
			float t0_dvdy_f = (V[v1]->V - V[v0]->V) * tile.dady + (V[v2]->V - V[v0]->V) * tile.dbdy;

			float t0_dudx_f = (V[v1]->U - V[v0]->U) * tile.dadx + (V[v2]->U - V[v0]->U) * tile.dbdx;
			float t0_dvdx_f = (V[v1]->V - V[v0]->V) * tile.dadx + (V[v2]->V - V[v0]->V) * tile.dbdx;

			float a0 = tile.a0;
			float b0 = tile.b0;

			float t0_u0_f = V[v0]->U + t0_dudy_f * (tile.y * TILE_SIZE - V[v0]->PY) + t0_dudx_f * (tile.x * TILE_SIZE - V[v0]->PX);
			float t0_v0_f = V[v0]->V + t0_dvdy_f * (tile.y * TILE_SIZE - V[v0]->PY) + t0_dvdx_f * (tile.x * TILE_SIZE - V[v0]->PX);

			int32_t t0_dudy = (int32_t)(65536.0 * t0_dudy_f * t0.UScaleFactor);
			int32_t t0_dvdy = (int32_t)(65536.0 * t0_dvdy_f * t0.VScaleFactor);

			int32_t t0_dudx = (int32_t)(65536.0 * t0_dudx_f * t0.UScaleFactor);
			int32_t t0_dvdx = (int32_t)(65536.0 * t0_dvdx_f * t0.VScaleFactor);

			int32_t t0_umask = ((1 << t0.LogWidth) - 1) << 16;
			int32_t t0_vmask = ((1 << t0.LogHeight) - 1) << 16;

			int32_t t0_u0 = (int32_t)(65536.0 * t0_u0_f * t0.UScaleFactor);
			int32_t t0_v0 = (int32_t)(65536.0 * t0_v0_f * t0.VScaleFactor);

			byte* scanline = dstSurface + tile.y * TILE_SIZE * bpsl;
			for (size_t j = 0; j != TILE_SIZE; ++j) {
				float a = a0;
				float b = b0;
				int32_t t0_u = t0_u0;
				int32_t t0_v = t0_v0;
				dword* span = ((dword*)scanline) + tile.x * TILE_SIZE;
				for (size_t i = 0; i != TILE_SIZE; ++i) {
					auto t0_offset = ((t0_u & t0_umask) >> 16) + (((t0_v & t0_vmask) >> 16) << t0.LogWidth);
					if (a >= 0 && b >= 0 && a + b < 1) {
						span[i] = t0.TextureAddr[t0_offset];
					}

					a += tile.dadx;
					b += tile.dbdx;
					t0_u += t0_dudx;
					t0_v += t0_dvdx;
				}
				a0 += tile.dady;
				b0 += tile.dbdy;
				t0_u0 += t0_dudy;
				t0_v0 += t0_dvdy;
				scanline += bpsl;
			}
		}
	};

	template <typename TTileRasterizer>
	void rasterize_triangle(TTileRasterizer rasterizer, const RVector4& v1, const RVector4& v2, const RVector4& v3) {
		const int tile_mx = rasterizer.clampedX(std::min({ v1.x * v1.w, v2.x * v2.w, v3.x * v3.w })) / TILE_SIZE;
		const int tile_Mx = rasterizer.clampedX(std::max({ v1.x * v1.w, v2.x * v2.w, v3.x * v3.w })) / TILE_SIZE;
		const int tile_my = rasterizer.clampedY(std::min({ v1.y * v1.w, v2.y * v2.w, v3.y * v3.w })) / TILE_SIZE;
		const int tile_My = rasterizer.clampedY(std::max({ v1.y * v1.w, v2.y * v2.w, v3.y * v3.w })) / TILE_SIZE;

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
