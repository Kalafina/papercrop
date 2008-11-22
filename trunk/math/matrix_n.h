//
// matrix_n.h 
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
#include "vector_n.h"
#include <list>
#include <vector>
class matrixn;
class matrixnView;
class intmatrixn;
class intmatrixnView;

namespace m2
{	
	struct _op
	{
		virtual void calc(matrixn& c, const matrixn& a, const matrixn& b) const {ASSERT(0);}
	};
}

namespace m1
{
	struct _op
	{
		virtual void calc(matrixn& c, const matrixn& a) const {ASSERT(0);}
	};
}

namespace m0
{
	struct _op
	{
		virtual void calc(matrixn& c) const {ASSERT(0);}
	};
}

#include "template_matrix.h"
//! 2D matrix 클래스
/*! conventions:
	1. 그 자체로 매트릭스를 표현하거나,
	2. multi-dimensinal vector의 시간축 나열 형태 (각 row벡터가 multi-dimensional vector, 즉, 시간축이 row 방향이 된다. )
	3. single-dimension signal 여러개. (시간축이 column방향이 된다.)
	사실 2와 3은 구별이 명확하지 않지만, 서로 독립적인 singal은 보통 3번 convention을 택하는 것이 각각 독립적으로 
	smoothing하거나 그래프를 그릴수 있어 편한 반면(ex: 왼손 speed signal, 오른손 speed signal)
	포지션 같은 독립적이지 않은 signal은 vector의 나열인 2번 convention을 택하는 것이 좋겠다.
	\ingroup group_math
*/

class intmatrixn : public _tmat<int>
{
protected:
	intmatrixn(int* _ptr, int _n, int _m, int _stride) :_tmat<int>(_ptr, _n, _m, _stride){}
public:
	// copy constructors : copy values
	intmatrixn (const _tmat<int>& other):_tmat<int>()		{ assign(other);	}
	intmatrixn (const intmatrixn& other):_tmat<int>()		{ assign(other);	}
	intmatrixn():_tmat<int>(){}
	virtual ~intmatrixn(){}

	intmatrixn& operator=(const _tmat<int>& other)	{ assign(other); return *this;}
	intmatrixn& operator=(const intmatrixn& other)	{ assign(other); return *this;}	
	intmatrixn& operator=(const intmatrixnView& other);//{ assign(other); return *this;}	

	// if not found, return -1 else return the found row-index.
	int findRow(intvectorn const& row) const;	

	/* already defined in _tmat<int>
	int     rows()    const			{ return n; }
	int     cols() const			{ return m; }

	void	  pushBack(const intvectorn& rowVec);
	void	  popBack(intvectorn* pOut=NULL);*/

	intmatrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX);
	const intmatrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX) const;	

	// L-value로 사용될수 있는 reference vector 를 return한다.
	intvectornView		row(int i)const			{ return _row<intvectornView>(i);}
	intvectornView		column(int i)const		{ return _column<intvectornView>(i);}
};

class intmatrixnView : public intmatrixn
{
public:
	// copy constructors : get reference
	intmatrixnView(const _tmat<int>& other)			{ _assignRef(other);	}
	intmatrixnView(const intmatrixn& other)			{ _assignRef(other);	}
	intmatrixnView(const intmatrixnView& other)		{ _assignRef(other);	}

	// L-value로 사용할때는 copy
	intmatrixn& operator=(const _tmat<int>& other)	{ _tmat<int>::assign(other); return *this;}
	intmatrixn& operator=(intmatrixn const& other)			{ __super::assign(other); return *this;};
	intmatrixn& operator=(const intmatrixnView& other)	{ assign(other); return *this;}	

	// L-value로 사용될수 있는, reference matrix로 만든다. 
	intmatrixnView (int* ptr, int nrow, int ncol, int stride);
	virtual ~intmatrixnView ();

};

class matrixn : public _tmat<double>
{
protected:
	matrixn(double* _ptr, int _n, int _m, int _stride) :_tmat<double>(_ptr, _n, _m, _stride){}
public:
	matrixn():_tmat<double>(){}
	matrixn ( int x, int y);

	// copy constructors : copy values
	matrixn (const _tmat<double>& other):_tmat<double>()	{ _tmat<double>::assign(other);	}
	matrixn (const matrixn& other):_tmat<double>()		{ assign(other);	}
	matrixn (const matrixnView& other);//:_tmat<double>{ assign(other);	}

	virtual ~matrixn();

	// reference가 value로 카피된다.
	matrixn& operator=(const _tmat<double>& other)	{ __super::assign(other); return *this;}
	matrixn& operator=(matrixn const& other)			{ __super::assign(other); return *this;};
	matrixn& operator=(const matrixnView& other);//{ assign(other); return *this;}	

	vector3 row3(int row) const						{ return vector3(value(row,0), value(row,1), value(row, 2));}

	// L-value로 사용될수 있는 reference vector 를 return한다.
	vectornView		row(int i)const					{ return _row<vectornView>(i);}
	vectornView		column(int i)const				{ return _column<vectornView>(i);}

	matrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX);
	inline const matrixnView range(int startRow, int endRow, int startColumn=0, int endColumn=INT_MAX) const;	

	// 모든 원소가 valid한지 검사한다. NaN등이 있으면 false return.
	bool isValid() const;

	
	//////////////////////////////////////////////////////////////////////
	// void operations
	//////////////////////////////////////////////////////////////////////

	matrixn&  identity(int n);
	
	matrixn&  assign( matrixn const& other)	{ __super::assign(other); return *this;}
	matrixn&  assign( matrix4 const&, bool bOnly3x3=false);

	// slow binary operations, I recommend you to use original function.
	friend matrixn operator+( matrixn const& a, matrixn const& b);
	friend matrixn operator-( matrixn const& a, matrixn const& b);
	friend matrixn operator*( matrixn const& a, matrixn const& b);
	friend matrixn operator/( matrixn  const& a, double b);	
	friend matrixn operator*( matrixn const& a, double b );	
	friend matrixn operator*( double b , matrixn const& a);	

	// n-ary operations
	matrixn&	concatColumns( std::list<matrixn*> matrixes);
	
	void	  pushBack3(const vector3& rowVec);
	
	void normalize(const vectorn &min, const vectorn&max);
	void toVector(vectorn& vec) const;			//!< concat all rows of this matrix into one large vector.
	vectorn toVector() const	{vectorn v; toVector(v); return v;}
	matrixn& fromVector(const vectorn& vec, int column);
	
	// sum of diagonal elements.
	double trace() const;

	//////////////////////////////////////////////////////////////////
	// followings are deprecated (use NR, m, m1 or m2 namespaces)
	//////////////////////////////////////////////////////////////////
	matrixn& align() ;

    matrixn&  derivative(matrixn const& positions);	// ex) velocities.delta(positions);

	// followings are deprecated (please refer to operatorTemplate.hpp)
	matrixn& each2(double (*s2_func)(double,double), matrixn const& a, matrixn const& b);
	matrixn& each2(double (*s2_func)(double,double), matrixn const& a, double b);
	matrixn& each2(double (*s2_func)(double,double), double a, matrixn const& b);
	matrixn& each1(void (*s1_func)(double&,double), matrixn const& a);
	matrixn& each1(const v1::_op& op, vectorn const& a);
	matrixn& each1(const v1::_op& op, matrixn const& a);
	matrixn& each0(const v0::_op& op);
	matrixn& each0(vectorn& (vectorn::*func)());

	matrixn& op0(const m0::_op & op)											{ op.calc(*this); return *this;}
	matrixn& op0(const m1::_op & op) ;
	matrixn& op1(const m1::_op & op, const matrixn& other)						{ op.calc(*this, other); return *this;}
	matrixn& op2(const m2::_op & op, matrixn const& a, matrixn const& b)		{ op.calc(*this, a,b); return *this;}
	//matrixn& each2(vectorn& (vectorn::*func)(vectorn const&, vectorn const& ), matrixn const& a, matrixn const& b);
	matrixn& each2(_tvectorn<double>& (_tvectorn<double>::*func)(_tvectorn<double> const&, _tvectorn<double> const& ), 
		matrixn const& a, matrixn const& b);
	matrixn& each2(_tvectorn<double>& (_tvectorn<double>::*func)(_tvectorn<double> const&, _tvectorn<double> const& ), 
		matrixn const& a, vectorn const& b);

	// return by value. (slow)
	matrixn Each2(double (*s2_func)(double,double), matrixn const& b) const;

	
private:
	
};


// matrixUtil
/*
namespace matrixUtil
{
	matrixn&  resample(matrixn const& mat, int nSample);	// When num row is the number of data, it uniformly samples the data so that num row becomes nSample.
	matrixn&  linearResample(matrixn const& mat, vectorn const& samplingIndex);

	matrixn&  mergeUpDown( matrixn const&, matrixn const& );
	matrixn&  mergeLeftRight( matrixn const&, matrixn const& );
	void      splitUpDown( matrixn&, matrixn& );
	void      splitLeftRight( matrixn&, matrixn& );
	void      splitUpDown( matrixn&, matrixn&, int );
	void      splitLeftRight( matrixn&, matrixn&, int );

	void      LUdecompose( int* ) ;
	void	  LUsubstitute( int*, vectorn& );
	void      LUinverse( matrixn& ) ;
	void	spectrum(const vectorn& input, int windowSize);

void resample(int nSample);	// When num row is the number of data, it uniformly resamples this data so that num row becomes nSample.
}*/


class matrixnView : public matrixn
{
public:
	// copy constructors : get reference
	matrixnView(const _tmat<double>& other)		{ _assignRef(other);	}
	matrixnView(const matrixn& other)			{ _assignRef(other);	}
	matrixnView(const matrixnView& other)		{ _assignRef(other);	}

	// L-value로 사용할때는 copy
	matrixn& operator=(const _tmat<double>& other)	{ _tmat<double>::assign(other); return *this;}
	matrixn& operator=(matrixn const& other)			{ __super::assign(other); return *this;};
	matrixn& operator=(const matrixnView& other)	{ assign(other); return *this;}	

	// L-value로 사용될수 있는, reference matrix로 만든다. 
	matrixnView (double* ptr, int nrow, int ncol, int stride);
	virtual ~matrixnView ();

};
