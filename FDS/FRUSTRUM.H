#pragma once

#define CLIPPER_MAXVERTS 48

class FrustumClipper {
public:
	FrustumClipper() {
		Init();
	}

	void InitViewport(Scene* Sc);
	void Clip(Face *F);

private:
	void Init();
	void Calc_Flags(Vertex* V);
	void Calc_YFlags(Vertex* V);
	void Swap();
	void Near();
	void Far();
	void FInterpolator(Vertex* V, float t);
	void Left();
	void Right();
	void Up();
	void Down();
	void CorrectCWOrder();
	void YSort(Vertex** Prim, Vertex** Scnd, mword nVerts);
	void MiplevelClipper(Face *F);
	float PolyArea();

	dword g_MipLevel;

	Viewport C_VP;
	mword C_numVerts;
	Vertex C_Verts[CLIPPER_MAXVERTS];
	Vertex* C_Ptr[3][CLIPPER_MAXVERTS];
	Vertex** C_Prim, ** C_Scnd, ** C_Tetr;

	Vertex* _IA, * _IB;
	float _IVal;
	dword C_Flags;
};


void viewportInit(Viewport& vp, const Scene* Sc);
void viewportCalcFlags(const Viewport& vp, Vertex* v);
void viewportCalcYFlags(const Viewport& vp, Vertex* v);
