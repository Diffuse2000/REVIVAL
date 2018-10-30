#ifndef _F4VEC_H_INCLUDED_
#define _F4VEC_H_INCLUDED_

class F4Vec
{
public:
	float f1, f2, f3, f4;
//	F4Vec(void);
//	F4Vec(float _f1, float _f2, float _f3, float _f4);
//	F4Vec(F4Vec &Vec);
//	~F4Vec(void);
	void assign(float _f1, float _f2, float _f3, float _f4);
	//F4Vec & operator =(F4Vec &Vec);
	friend F4Vec operator +(F4Vec Vec1, F4Vec &Vec2);
	friend F4Vec operator -(F4Vec Vec1, F4Vec &Vec2);
	friend F4Vec operator *(F4Vec Vec1, F4Vec &Vec2);
	friend F4Vec operator /(F4Vec Vec1, F4Vec &Vec2);
	//F4Vec & operator =(float _f1);
	friend F4Vec operator +(F4Vec Vec1, float _f1);
	friend F4Vec operator -(F4Vec Vec1, float _f1);
	friend F4Vec operator *(F4Vec Vec1, float _f1);
	friend F4Vec operator /(F4Vec Vec1, float _f1);
	F4Vec & operator +=(F4Vec &Vec);
	F4Vec & operator -=(F4Vec &Vec);
	F4Vec & operator *=(F4Vec &Vec);
	F4Vec & operator /=(F4Vec &Vec);
	F4Vec & operator +=(float _f1);
	F4Vec & operator -=(float _f1);
	F4Vec & operator *=(float _f1);
	F4Vec & operator /=(float _f1);

};

#endif  // _F4VEC_H_INCLUDED_