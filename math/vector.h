//
// vector.h 
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
#include "../utility/typestring.h"
class quater;
class matrix4;
class vectorn;
class matrixn;
/// never make virtual functions. 
/** 이유: 이것의 주소는 direct하게 D3DXVECTOR3의 주소로 바뀔수 있다. virtual function을 만들면 VTable때문에 이것이 안된다.
	\ingroup group_math
*/

#pragma message("Compiling vector.h - this should happen just once per project.\n")
class vector3
{
	public:
 
	double x, y, z;

    // constructors
    vector3() {}
    vector3( double xx, double yy, double zz )				{ x=xx; y=yy; z=zz;}
    vector3( double a[3] )								{ x=a[0]; y=a[1]; z=a[2]; }
#ifdef DIRECT3D_VERSION
	vector3( const D3DXVECTOR3& vec)					{ x=vec.x; y=vec.y; z=vec.z;}
#endif
	// binary operations	
	// this가 output이다. 
	void add(const vector3&, const vector3&);
	void sub(const vector3&, const vector3&);
	void multadd(const vector3&, double);	//!< c.multadd(b,w) -> c+=b*w;
	void mult(const vector3& ,double);
	void mult(const matrix4& mat, const vector3& v);
	void divide(const vector3& v, double a)	{ mult(v, 1/a);};
	void cross(const vector3&, const vector3&);
	vector3 cross(const vector3& other) const		{ vector3 c; c.cross(*this, other); return c;}
	void normalize( const vector3 & );
	vector3 dir()	{ vector3 temp; temp.normalize(*this); return temp; }
	void negate(const vector3&);
	void interpolate( double, vector3 const&, vector3 const& );
	void lerp( vector3 const& a, vector3 const& b, double t)	{ interpolate(t, a, b);}
	void blend(const vectorn& weight, const matrixn& aInputVector);
	//! quaternion ln
	/*!	A unit quaternion, is defined by:
	Q == (cos(theta), sin(theta) * v) where |v| = 1
	The natural logarithm of Q is, ln(Q) = (0, theta * v)*/
    
	void ln( const quater& q);
	void log( const quater& q)		{ ln(q);}
	quater exp() const;	//!< 느리므로 q.exp(v)를 사용할 것.
	void rotate( const quater& q);
	void rotate( const quater& q, vector3 const& in);
	void angularVelocity( quater const& q1, quater const& q2);
	void linearVelocity(vector3 const& v1, vector3 const& v2);
	inline void difference(vector3 const& v1, vector3 const& v2)	{ linearVelocity(v1, v2);}
	
    double    operator%( vector3 const&) const;
    double    operator/( vector3 const&) const;
    // 아래함수는 느리다.
	vector3    operator+( vector3 const& ) const;
    vector3    operator-( vector3 const& ) const;
    vector3    operator*( vector3 const& ) const;
    vector3    operator*( double ) const;
    vector3    operator/( double ) const;
	friend vector3    operator*( double, vector3 const& ) ;

	// unary operations
	void add(double value);
	void add(const vector3& other);
	void sub(double value);
	void sub(const vector3& other);
	void column(int col, const matrix4& other);	//!< extract a column from the rotation part in a matrix
	void row(int col, const matrix4& other);		//!< extract a row from the rotation part in a matrix
	void translation(const matrix4& other);		//!< extract translation vector from a matrix
	void imaginaries(const quater& in);			//!< extract imagenaries from a quaternion
	double distance(const vector3& other) const;
	double squaredDistance(const vector3& other) const;
	void leftMult( const matrix4& mat);		//!< 당연히 this=mat*this; 로 정의된다.
	void rotationVector(const quater& in);
	quater quaternion() const;	//!< rotation vector를 quaternion으로 바꾼다.	{ quater q; q.setRotation(*this); }

	void operator+=( vector3 const& );
    void operator-=( vector3 const& );
    void operator*=( double );	
    void operator/=( double );
	void operator=(vector3 const& );
	void hermite(const vector3& p1, const vector3& t1, const vector3& p2, const vector3& t2, double t);	//!< hermite interpolation of p1 and p2. 0<=t<=1
	//! 이 함수는 느리다.
	vector3  operator-() const	{ vector3 c; c.negate(*this); return c;};

	// functions
	void normalize();
	double length() const;
	// calc 0 to pi angle
    double angle( vector3 const& ) const;
	double cosTheta(vector3 const&) const;
	double sinTheta(vector3 const& b) const;
	// calc 0 to 2pi angle assuming z=0 (plane constraint)
	double angle2d(vector3 const& b) const;
	// calc -pi to pi angle assuming z=0 (plane constraint)
	double angle2ds(vector3 const& b) const;

	// calc -pi to pi angle assuming axis=0 (plane constraint): axis 0:x, 1: y, 2:z
	double angle2ds(vector3 const& b, int axis) const;

    // stream
//   friend ostream& operator<<( ostream&, vector3 const& );
  //  friend istream& operator>>( istream&, vector3& );

    double& operator[] (int i)						{ switch(i) { case 0: return x; case 1: return y; case 2: return z; } ASSERT(0); return x;}
	const double& operator[] (int i) const			{ switch(i) { case 0: return x; case 1: return y; case 2: return z; } ASSERT(0); return x;}
    void   getValue( double d[3] )					{ d[0]=x; d[1]=y; d[2]=z;}
	double  getValue( int n ) const					{ switch(n)			{ case 0: return x; case 1: return y; case 2: return z;} return 0;}
    void   setValue( double d[3] )					{ x=d[0]; y=d[1]; z=d[2]; }
	void   setValue(double xx, double yy, double zz )	{ x=xx; y=yy; z=zz;}
	void   setValue(const vectorn& other, int start=0);

	TString output();
#ifdef DIRECT3D_VERSION
	void   setValue(const D3DXVECTOR3& vec)			{ x=vec.x; y=vec.y; z=vec.z;}

#ifndef MATH_DOUBLE_PRECISION
	inline operator D3DXVECTOR3*()	const	{ return (D3DXVECTOR3*)this;};
	inline operator D3DXVECTOR3&()	const	{ return *((D3DXVECTOR3*)this);};
#else
	//inline operator D3DXVECTOR3() const			{ return D3DXVECTOR3((FLOAT)x, (FLOAT)y, (FLOAT)z);}
	inline D3DXVECTOR3 dx() const			{ return D3DXVECTOR3((FLOAT)x, (FLOAT)y, (FLOAT)z);}
#endif
#endif

	inline operator const double*() const		{ return &x;}
	inline operator double*()					{ return &x;}


};

