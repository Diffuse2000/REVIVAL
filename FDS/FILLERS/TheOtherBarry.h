#pragma once


namespace barry {
constexpr const int TILE_SIZE = 8;

struct alignas(16) RVector4 {
	// xyz taken from TPos, w is 1/z
	float x, y, z, w;
};

struct Tile {
	int x, y;
	float a0, dadx, dady;
	float b0, dbdx, dbdy;
};

using Triangle = RVector4[3];

template <typename TTileRasterizer>
void rasterize_triangle(TTileRasterizer rasterizer, RVector4 a, RVector4 b, RVector4 c) {
	int tile_mx = std::min(a.x * a.w, b.x * b.w, c.x * c.w) / TILE_SIZE;
	int tile_Mx = std::max(a.x * a.w, b.x * b.w, c.x * c.w) / TILE_SIZE;
	int tile_my = std::min(a.y * a.w, b.y * b.w, c.y * c.w) / TILE_SIZE;
	int tile_My = std::max(a.y * a.w, b.y * b.w, c.y * c.w) / TILE_SIZE;

	// (az0, bz0) is at (mx, my)
	// d?zd? are per tile
	float az0, dazdx, dazdy;
	float bz0, dbzdx, dbzdy;


}
} // namespace barry