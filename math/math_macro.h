	//
// math_macro.H 
//
// Copyright 2004 by Taesoo Kwon.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//

#pragma once


#define TRUE    1
#define FALSE   0

#define MAX(x,y) ( ((x)>(y)) ? (x) : (y) )
#define MIN(x,y) ( ((x)<(y)) ? (x) : (y) )
#define ABS(x)   ( ((x)>0.0) ? (x) :(-1.0*(x)) )
#define ACOS(x)  ( ((x)>1.0) ? (0) : ( ((x)<-1.0) ? (M_PI) : (acos(x)) ) )
#define ASIN(x)  ( ((x)>1.0) ? (M_PI/2.0) : ( ((x)<-1.0) ? (-M_PI/2.0) : (asin(x)) ) )
#define SQR(x)   ( (x)*(x) )
#define SHIFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define ROUND(x) ((int)floor((x)+0.5))
#define TO_RADIAN(degree) ((degree) * (M_PI / 180.0f))
#define TO_DEGREE(radian) ((radian)/(M_PI/180.f))

template<class T>
inline void SWAP(T &a, T &b)
{T dum=a; a=b; b=dum;}

template <class T>
inline const T CUBIC(const T x){return x*x*x;}

inline double MAX3(double a,double b, double c) {
	if(a>b)
	{
		if(a>c) return a;
		else return c;
	}
	else
	{
		if(c>b) return c;
		else return b;
	}
}

inline double MIN3(double a,double b, double c) {
	if(a<b)
	{
		if(a<c) return a;
		else return c;
	}
	else
	{
		if(c<b) return c;
		else return b;
	}
}

inline double CLAMP(double a, double i1, double i2)
{
	if(a<i1) return i1;
	if(a>i2) return i2;
	return a;
}

inline int CLAMP(int a, int i1, int i2)
{
	if(a<i1) return i1;
	if(a>i2) return i2;
	return a;
}

inline bool isSimilar(double a, double b)
{
	return ABS(a-b)<0.0001;
}
