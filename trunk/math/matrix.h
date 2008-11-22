//
// matrix.H 
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

class vector3;
class quater;
/// never make virtual functions. 
/** 이유: 이것의 주소는 direct하게 D3DXMATRIX의 주소로 바뀔수 있다. virtual function을 만들면 VTable때문에 이것이 안된다.
다만, 컨벤션은 opengl을 따른다. 주의사항 참고.

주의 사항: 
quaternion은 q1*q2*v형태로 정의상 오른쪽께 먼저 점에 곱해진다고 생각해야 한다.
openGL matrix는 quaternion과 마찮가지로 R1*r2*v형태로 오른쪽에 점이 곱해진다.
하지만 D3DXMATRIX는 v*R1*R2형태로 점이 왼쪽에 곱해지고, 마찬가지로 D3DXQUATERNION은 q1*q2가 원래 정의의 q2*q1과 일치하도록 
곱하기순서가 바뀌어 있다. 즉 D3DXMATRIX와 D3DXQUATERNION모두 왼쪽에 있는 것이 로컬, 가장 오른쪽에 있는 것이 글로벌이다.
반면 quater클래스와 matrix4클래스는 openGL의 convention을 따른다. 즉 점이 오른쪽에서 곱해진다.
양쪽의 convention을 맞추기 위해서 toDXmat과 fromDXmat함수를 보면 각각 transpose를 해주는 것을 볼 수 있다.
\ingroup group_math
*/

class matrix4
{
public:

	union {
        struct {
            double        _11, _12, _13, _14;
            double        _21, _22, _23, _24;
            double        _31, _32, _33, _34;
            double        _41, _42, _43, _44;

        };
        double m[4][4];
    };

#ifdef DIRECT3D_VERSION
	matrix4(const D3DXMATRIX& mat)				{ fromDXmat(mat);};
#endif
	matrix4(const matrix4& mat)					{ memcpy(this, &mat, sizeof(matrix4));}
	matrix4();
	~matrix4();

	void identity()	{ setIdentityRot(); setTranslation(vector3(0.0, 0.0, 0.0));}

	// n-ary operators
	void lookAtLH(const vector3& eye, const vector3& at, const vector3& up);
	void lookAtRH(const vector3& eye, const vector3& at, const vector3& up);
	void setScaling(double sx, double sy, double sz);
	void setTranslation(const vector3& tx);		//!< 다른 set계열 함수와 달리 rotation파트는 건드리지 않는다. 	
	void setTranslationMat(const vector3& tx)	{ setIdentityRot(); setTranslation(tx);}
	void setTransform(const quater& rot, const vector3& trans);
	void setRotation(const quater& q);
	void setRotation(const vector3& axis, double angle);
	void setRotationX(double angle);
	void setRotationY(double angle);
	void setRotationZ(double angle);
	void setRotation(const char* aChannel, double *aValue, bool bRightToLeft=false);	//!< from euler angle. aChannel="YXZ" or something like that.
	void setAxisRotation(const vector3& vecAxis, const vector3& front, const vector3& vecTarget); 	//!< front벡터를 vecAxis를 중심으로 회전해서 vecTarget과 vecAxis가 이루는 평면에 놓이도록 만드는 Matrix를 구한다.
	void setIdentityRot();

	void transpose();
	// right multiplication. eg) a=a*matScale
	void leftMult(double scalar);
	void leftMultScaling(double sx, double sy, double sz);
	void leftMultRotation(const quater& b);
	void leftMultRotation(const vector3& axis, double angle);
	void leftMultTranslation(const vector3& vec);
	void mult(const matrix4& a, const matrix4& b);
	void mult(const matrix4& a, const quater& b);
	void mult(const quater& a, const matrix4& b);

	// unary operators
	void adjoint(const matrix4& a);
	void inverse(const matrix4& a);
	void extractRot(const matrix4& a);
	void transpose(const matrix4& a);
	void leftMult(const matrix4& a);	//!< this=a*this;
	void operator*=(const matrix4& a);	//!< this=this*a;
	
	// inquiry functions
	void decomposeLH(vector3& eye, vector3& at, vector3& up);
	void decomposeRH(vector3& eye, vector3& at, vector3& up);

	void setValue( double x00, double x01, double x02,
				   double x10, double x11, double x12,		// 나머지는 0,1으로 초기화 
				   double x20, double x21, double x22 )	
	{ m[0][0]=x00, m[0][1]=x01, m[0][2]=x02,  m[1][0]=x10, m[1][1]=x11, m[1][2]=x12,  
m[2][0]=x20, m[2][1]=x21, m[2][2]=x22,  m[0][3]=0,m[1][3]=0,m[2][3]=0,m[3][0]=0,m[3][1]=0,m[3][2]=0,m[3][3]=1;}

	void setValue( double x00, double x01, double x02, double x03,
				   double x10, double x11, double x12, double x13,
				   double x20, double x21, double x22, double x23,
				   double x30, double x31, double x32, double x33)	
	{ m[0][0]=x00, m[0][1]=x01, m[0][2]=x02,  m[0][3]=x03,
	  m[1][0]=x10, m[1][1]=x11, m[1][2]=x12,  m[1][3]=x13,
	  m[2][0]=x20, m[2][1]=x21, m[2][2]=x22,  m[2][3]=x23,
	  m[3][0]=x30, m[3][1]=x31, m[3][2]=x32,  m[3][3]=x33;}

	double determinant() const;
	double minor(const size_t r0, const size_t r1, const size_t r2, 
				const size_t c0, const size_t c1, const size_t c2) const;


#ifdef DIRECT3D_VERSION

#ifdef MATH_DOUBLE_PRECISION
   void fromDXmat( const D3DXMATRIX &mat);
   void toDXmat(D3DXMATRIX& mat) const;
#else
	void fromDXmat( const D3DXMATRIX &mat)  { D3DXMatrixTranspose(*this, &mat);};
	void toDXmat(D3DXMATRIX& mat) const		{ D3DXMatrixTranspose(&mat,*this);};
private:
	//아래 함수는 D3DX가 제공하는 optimize된 곱하기 함수등을 사용하기 위한 것이다. 즉 내부 용도로만 사용되어야 한다.
	// 외부에서 사용하는 함수는 위에 fromDXmat, toDXmat처럼 transpose를 수반한다.
	inline operator D3DXMATRIX*()	const	{ return (D3DXMATRIX*)this;};	
#endif
#endif
};

