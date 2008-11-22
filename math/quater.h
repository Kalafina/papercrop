//
// quater.H 
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
#include "vector.h"
//! quaternion class
/*! 
	\ingroup group_math

	Definition:
	(axis*sin(theta/2), cos(theta/2))
	

주의 사항: 
quaternion은 q1*q2*v형태로 정의상 오른쪽께 먼저 점에 곱해진다고 생각해야 한다.
openGL matrix는 quaternion과 마찮가지로 R1*r2*v형태로 오른쪽에 점이 곱해진다.
하지만 D3DXMATRIX는 v*R1*R2형태로 점이 왼쪽에 곱해지고, 마찬가지로 D3DXQUATERNION은 q1*q2가 원래 정의의 q2*q1과 일치하도록 
곱하기순서가 바뀌어 있다. 즉 D3DXMATRIX와 D3DXQUATERNION모두 왼쪽에 있는 것이 로컬, 가장 오른쪽에 있는 것이 글로벌이다.
반면 quater클래스와 matrix4클래스는 openGL의 convention을 따른다. 가장 오른쪽에 있는것이 로컬 트랜스폼이다.
*/
class quater
{
public:
	double w, x, y, z;
	
	//
    // constructors
	//
    quater() {};
    quater( double ww, double xx, double yy, double zz )			{ w=ww; x=xx; y=yy; z=zz;}
    quater( double a[4] )										{ w=a[0]; x=a[1]; y=a[2]; z=a[3]; }
	quater(const vector3& vec, double ww)						{ x=vec.x; y=vec.y; z=vec.z; w=ww;}
	quater(double angle, const vector3& axis)					{ setRotation(axis, angle);}
	quater(const vector3& vec)									{ x=vec.x; y=vec.y; z=vec.z; w=0;}
	quater(const quater& q)										{ memcpy(this,&q,sizeof(quater)); };
#ifdef DIRECT3D_VERSION
	quater(const D3DXQUATERNION& q)								{ x=q.x; y=q.y; z=q.z; w=q.w;}//memcpy(this,q,sizeof(D3DXQUATERNION)); };
#endif
    // binary operations
	// 이경우 lvalue가 output이 된다. 즉 c=a+b -> c.add(a,b);
	void add(quater const& a, quater const& b);
	void subtract(quater const& a, quater const& b);
	void mult(quater const& a, double b);
	void mult(quater const& a, quater const& b);// cross product
    void pow( vector3 const&, double );
    void slerp( quater const&, quater const&, double );
	void safeSlerp( quater const&, quater const&, double );// align이 안되어 있어도 동작
    void interpolate( double, quater const&, quater const& );
	void unitAxisToUnitAxis2(const vector3& vFrom, const vector3& vTo);
	void axisToAxis( const vector3& vFrom, const vector3& vTo);
	

	// Decompose a quaternion into q= rotAxis_Y * offset. (Same as the decomposeTwistTimesNoTwist function when rkAxis=(0,1,0))
	void decompose(quater& rotAxis_y, quater& offset) const;

	// Decompose a quaternion into q = q_twist * q_no_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_no_twist represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    void decomposeTwistTimesNoTwist (const vector3& rkAxis, quater& rkTwist, quater& rkNoTwist) const;

    // Decompose a quaternion into q = q_no_twist * q_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_no_twist represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    void decomposeNoTwistTimesTwist (const vector3& rkAxis, quater& rkNoTwist, quater& rkTwist) const;

	void scale(double s);
		
	// derivatives
	void difference(quater const& q1, quater const& q2);			//!< quaternion representation of "angular velocity w" : q2inv(q1);
	void derivative(quater const& q1, quater const& q2);	//!< q'(t)=(1/2) w q; or q'(t)=q2-q1; -> both produces almost same result
	
	void toAxisAngle(vector3& axis, double& angle) const;
	void bezier(const quater& q0, const quater& q1, const quater& q2, const quater& q3, double t);
	void hermite(const quater& qa, const vector3& wa, const quater& qb, const vector3& wb, double t);
		
	double    operator%( quater const&) const;    // dot product
	double	distance( quater const& ) const;
	// 아래 함수들은 느리므로 사용을 자제.
    quater    operator+( quater const& b) const { quater c;c.add(*this,b); return c;}
    quater    operator-( quater const& b) const { quater c;c.subtract(*this,b); return c;}
    quater    operator*( quater const& b) const { quater c;c.mult(*this,b); return c;}
	quater    operator*( double b) const			{ quater c;c.mult(*this,b); return c;}
	quater    operator/( double b) const			{ quater c;c.mult(*this,1/b);return c;}
	friend quater    operator*(double a, quater const& b) { quater c;c.mult(b,a); return c;}
	
	// unary operations
	//! quaternion exp
	/*!
	Given a pure quaternion defined by:
	Q = (0, theta/2 * v); -> this means theta/2 rotation vector

	This method calculates the exponential result.
	exp(Q) = (cos(theta/2), sin(theta/2) * v) -> theta rotation
	*/
    void exp( vector3 const& );
	vector3 log() const					{ vector3 o; o.ln(*this); return o;}
    void inverse(const quater& a)		{ w=a.w; x=a.x*-1.f; y=a.y*-1.f; z=a.z*-1.f;}
	void negate(quater const& a);
	void setRotation(const matrix4& a);
	void setRotation(const char* aChannel, double *aValue, bool bRightToLeft=false);	//!< from euler angle. aChannel="YXZ" or something like that.
	void getRotation(const char* aChannel, double *aValue, bool bRightToLeft=false) const;	//!< to euler angle. aChannel="YXZ" or something like that.
	void setRotation(const vector3& axis, double angle);
	void setRotation(const vector3& rotationVector)	{ exp(rotationVector/2.f); }
	void setAxisRotation(const vector3& vecAxis, const vector3& front, const vector3& vecTarget);
	void leftMult(quater const& a)			{ quater copy(*this); mult(a, copy);}
	void normalize(const quater& a);
	void align(const quater& other)			{ quater& c=*this; if(c%other<0) c*=-1;}
	void operator=(quater const& a)			{ setValue(a.w, a.x, a.y, a.z);}
	void operator+=(quater const& a)		{ add(*this,a);}
	void operator-=(quater const& a)		{ subtract(*this,a);}
	void operator*=(quater const& a)		{ quater copy=*this; mult(copy,a);}
	void operator*=(double a)				{ x*=a; y*=a; z*=a; w*=a;}
	void operator/=(double a)				{ double inv=1.f/a; (*this)*=inv;}

	// 아래함수는 느리다.
	quater   inverse() const	{ quater c; c.inverse(*this); return c;};
	quater   operator-() const  { quater c; c.negate(*this); return c;};
	vector3 rotationVector() const { vector3 c; c.rotationVector(*this); return c;}
	/*
    // transform with matrix
    friend matrix    Quater2Matrix( quater const& );
    friend quater    Matrix2Quater( matrix const& );
    friend vector3    Quater2EulerAngle( quater const& );
    friend quater    EulerAngle2Quater( vector3 const& );
	*/
	// void operations
	
	void normalize();
	void negate()	{ negate(*this);}
	void identity()	{ w=1.f; x=0.f; y=0.f; z=0.f;}
	double rotationAngle() const	{ vector3 temp; temp.rotationVector(*this); return temp.length();}
	double rotationAngle(const vector3& axis) const { vector3 temp; temp.rotationVector(*this); if(temp%axis<0) return temp.length()*-1.f; else return temp.length();}
	double rotationAngleAboutAxis(const vector3& axis) const;
	// stream
 //  friend ostream& operator<<( ostream&, quater const& );
   // friend istream& operator>>( istream&, quater& );

	//
    // inquiry functions
	//
	double	real() const		{ return w; }
	// This is slow, so I recommend vector3::imagenaries function instead.
	vector3 imaginaries() const { vector3 out; out.x=x; out.y=y; out.z=z; return out;}
	double  length() const;
	double& operator[] (int i)				{ switch(i) { case 0: return w; case 1: return x; case 2: return y; case 3: return z; } ASSERT(0); return x;}
    const double& operator[] (int i) const 	{ switch(i) { case 0: return w; case 1: return x; case 2: return y; case 3: return z; } ASSERT(0); return x;}
	double getValue(int i)const	{ switch(i) { case 0: return w; case 1: return x; case 2: return y; case 3: return z; } ASSERT(0); return x;}
	void getValue( double d[4] )			{ d[0]=w; d[1]=x; d[2]=y; d[3]=z; }
    void setValue( double d[4] )			{ w=d[0]; x=d[1]; y=d[2]; z=d[3]; }
	void setValue( double ww,double xx, double yy, double zz ){ w=ww; x=xx; y=yy; z=zz;}
	void setValue( const vector3& vec, double ww)			  { x=vec.x; y=vec.y; z=vec.z; w=ww;}

	TString output(bool rotationVector=false);
};


