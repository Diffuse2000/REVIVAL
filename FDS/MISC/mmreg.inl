//#define MMREG_MMX // this has to be precompiled w/ or w/o MMX support

#ifdef MMREG_MMX
// implementation uses MMX instruction set
inline void mmreg::addb(mmreg &r1, mmreg &r2)
{
	__asm
	{
		mov esi, [r1]
		mov edi, [r2]
		mov eax, [this]
		movq mm0, [esi]
		movq mm1, [edi]
		paddb mm0, mm1
		movq [eax], mm0
	}
}

inline void mmreg::addw(mmreg &r)
{
	__asm
	{
		mov esi, [r]
		mov edi, [this]
		movq mm0, [esi]
		movq mm1, [edi]
		paddw mm0, mm1
		movq [edi], mm0
	}
}

inline void mmreg::addw(mmreg &r1, mmreg &r2)
{
	__asm
	{
		mov esi, [r1]
		mov edi, [r2]
		mov eax, [this]
		movq mm0, [esi]
		movq mm1, [edi]
		paddw mm0, mm1
		movq [eax], mm0
	}
}

inline void mmreg::subw(mmreg &r1, mmreg &r2)
{
	__asm
	{
		mov esi, [r1]
		mov edi, [r2]
		mov eax, [this]
		movq mm0, [esi]
		movq mm1, [edi]
		psubw mm0, mm1
		movq [eax], mm0
	}
}

inline void mmreg::cbw()
{
	__asm
	{
		mov eax, [this]
		movq mm0, [eax]
		pxor mm1, mm1
		punpcklbw mm0, mm1
		movq [eax], mm0
	}
}

inline void mmreg::cwb()
{
	__asm
	{
		mov eax, [this]
		movq mm0, [eax]
		pxor mm1, mm1
		packuswb mm0, mm1
		movq [eax], mm0
	}
}

inline void mmreg::mulh(mmreg &r)
{
	__asm
	{
		mov esi, [r]
		mov edi, [this]
		
		movq mm0, [esi]
		movq mm1, [edi]
		pmulhuw mm0, mm1
		movq [edi], mm0
	}
}

inline void mmreg::shrw(uint64 c)
{	
	__asm
	{
		mov edi, [this]
		movq mm0, [edi]
		psrlw mm0, [c]
		movq [edi], mm0
	}
}

inline void mmreg::clearstate()
{
	__asm
	{
		emms
	}
}
#else //MMREG_MMX
// implementation written in C, quite slow
inline void mmreg::addb(mmreg &r1, mmreg &r2)
{
	for(mword i=0; i<8; i++)
		_d8[i] = r1._d8[i]+r2._d8[i];
}

inline void mmreg::addw(mmreg &r)
{
	for(mword i=0; i<4; i++)
		_d16[i] += r._d16[i];
}

inline void mmreg::addw(mmreg &r1, mmreg &r2)
{
	for(mword i=0; i<4; i++)
		_d16[i] = r1._d16[i]+r2._d16[i];
}

inline void mmreg::subw(mmreg &r1, mmreg &r2)
{
	for(mword i=0; i<4; i++)
		_d16[i] = r1._d16[i]-r2._d16[i];
}

inline void mmreg::cbw()
{
	for(smword i=3; i>=0; i--)
		_d16[i] = _d8[i];
}

inline void mmreg::cwb()
{
	for(mword i=0; i<4; i++)
		_d8[i] = (byte)_d16[i];
}

inline void mmreg::mulh(mmreg &r)
{
	for(mword i=0; i<4; i++)
		_d16[i] = _d16[i] * r._d16[i] >> 16;
}

inline void mmreg::shrw(uint64 c)
{	
	for(mword i=0; i<4; i++)
		_d16[i] >>= c;
}

inline void mmreg::sarw(uint64 c)
{	
	for(mword i=0; i<4; i++)
		_sd16[i] >>= c;
}

inline void mmreg::shlw(uint64 c)
{
	for(mword i=0; i<4; i++)
		_d16[i] <<= c;
}

inline void mmreg::clearstate()
{
}
#endif //MMREG_MMX