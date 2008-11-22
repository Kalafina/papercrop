//
// vector_n.h
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
class matrixn;
class bitvectorn;
#include "vector.h"
#include "quater.h"
#include <typeinfo.h>
class Metric;
class matrixnView;
class matrixn;
class cmplxvectorn ;
class intvectorn;
class intvectornView;
class vectorn;
class vectornView;
#include "template_math.h"


class intvectorn : public _tvectorn<int>
{
protected:
	intvectorn(int* ptrr, int size, int stride):_tvectorn<int>(ptrr,size,stride){}	// reference
public:
	intvectorn():_tvectorn<int>(){}

	explicit intvectorn(int n):_tvectorn<int>()				{setSize(n);}
	explicit intvectorn( int, int x, ...);	// n dimensional vector	(ex) : vectorn(3, 1, 2, 3);
	// copy constructor : 항상 카피한다.
	intvectorn(const _tvectorn<int>& other):_tvectorn<int>()		{ assign(other);}
	intvectorn(const intvectorn& other):_tvectorn<int>()			{ assign(other);}
	intvectorn(const intvectornView& other);

	// 값을 copy한다.
	intvectorn& operator=(const _tvectorn<int>& other)		{ _tvectorn<int>::assign(other);return *this;}
	intvectorn& operator=(const intvectorn& other)			{ assign(other);return *this;}	
	intvectorn& operator=(const intvectornView& other);

	friend bool operator==(intvectorn const& a, intvectorn const& b) 
	{
		if(a.size()!=b.size()) return false;
		for(int i=0; i<a.size(); i++)
			if(a[i]!=b[i]) return false;
		return true;
	}

	// L-value로 사용될수 있는, reference array를 만들어 return 한다. 
	// ex) v.range(0,2).setValues(2, 1.0, 2.0);
	intvectornView	range(int start, int end, int step=1);
	const intvectornView	range(int start, int end, int step=1) const;

	~intvectorn(){}

	int count(int (*s2_func)(int ,int ), int value, int start=0, int end=INT_MAX);
	intvectorn&  colon(int start, int end, int stepSize=1);
	intvectorn&  sortedOrder(vectorn const & input);
	intvectorn&  makeSamplingIndex(int nLen, int numSample);
	// 첫프레임과 마지막 프레임은 반드시 포함하고 나머지는 그 사이에서 uniform sampling
	intvectorn&  makeSamplingIndex2(int nLen, int numSample);

	intvectorn& findIndex(intvectorn const& source, int value);
	// return -1 if not found.
	int findFirstIndex(int value) const;

	intvectorn&  setAt( intvectorn const& columnIndex, _tvectorn<int> const& value);
	intvectorn&  setAt( intvectorn const& columnIndex, int value);

	vectorn toVectorn();

	int minimum() const;
	int maximum() const;
	int sum() const;
};

// reference로 받아온다.
class intvectornView :public intvectorn
{
public:
	// L-value로 사용될수 있는, reference array로 만든다. 
	intvectornView (int* ptrr, int size, int stride);	
	// copy constructor : get reference.
	intvectornView(const _tvectorn<int>& other)				{ assignRef(other);}
	intvectornView(const intvectorn& other);//				{ assignRef(other);}
	intvectornView(const intvectornView& other)				{ assignRef(other);}
	~intvectornView (){}

	// L-value로 사용되는 경우, 값을 copy한다.
	intvectorn& operator=(const _tvectorn<int>& other)		{ _tvectorn<int>::assign(other);return *this;}
	intvectorn& operator=(const intvectorn& other);//		{ assign(other);return *this;}	
	intvectorn& operator=(const intvectornView& other)		{ assign(other);return *this;}

};

namespace v0
{
	// v0 namespace에는 doSomething(c); 형태의 class나 function이 정의된다. (operatorTemplate)
	struct abstractClass 	// 상속할 필요 없음. 
	{
		void operator()(vectorn& c){}
	};

	// deprecated
	struct _op
	{
		virtual void calc(vectorn& c) const {ASSERT(0);}
	};
}

namespace v1
{
	// v1 namespace에는 doSomething(c,a); 형태의 class나 function이 정의된다. (operatorTemplate)
	struct abstractClass 	// 상속할 필요 없음. 
	{
		void operator()(vectorn& c, const vectorn& a){}
	};

	// deprecated
	struct _op
	{
		virtual void calc(vectorn& c, const vectorn& a) const {ASSERT(0);}
	};
}

namespace v2
{
	// v2 namespace에는 doSomething(c,a,b); 형태의 class나 function이 정의된다. (operatorTemplate)
	struct abstractClass	// 상속할 필요 없음.
	{
		void operator()(vectorn& c, const vectorn& a, const vectorn& b){}
	};
	// deprecated
	struct _op
	{
		virtual void calc(vectorn& c, const vectorn& a, const vectorn& b) const {ASSERT(0);}
	};
}

// deprecated
namespace sv2
{
	struct _op
	{
		virtual double calc(const vectorn& a, const vectorn& b) const {ASSERT(0);return 0.0;}
	};
}


class vectorn : public _tvectorn<double>
{	
protected:
	vectorn(double* ptrr, int size, int stride):_tvectorn<double>(ptrr,size,stride){}
public:
	vectorn();		
	vectorn(const vector3& other);
	vectorn(const quater& other);

	// 값을 카피해서 받아온다.	
	vectorn(const _tvectorn<double>& other);	
	vectorn(const vectorn& other);
	vectorn(const vectornView& other);

	explicit vectorn( int x):_tvectorn<double>() { setSize(x);}

	// n dimensional vector	(ex) : vectorn(3, 1.0, 2.0, 3.0);
	explicit vectorn( int n, double x);		
	explicit vectorn( int n, double x, double y);		
	explicit vectorn( int n, double x, double y, double z);		
	explicit vectorn( int n, double x, double y, double z, double w, ...);	// n dimensional vector	(ex) : vectorn(3, 1.0, 2.0, 3.0);

	~vectorn(){}

	matrixnView column() const;	// return n by 1 matrix, which can be used as L-value (reference matrix)
	matrixnView row() const;	// return 1 by n matrix, which can be used as L-value (reference matrix)
	

	// L-value로 사용될수 있는, reference array를 만들어 return 한다. 
	// ex) v.range(0,2).setValues(2, 1.0, 2.0);
	vectornView range(int start, int end, int step=1);
	const vectornView range(int start, int end, int step=1) const	;

	//////////////////////////////////////////////////////////////////////
	// binary operations. 
	//////////////////////////////////////////////////////////////////////
	vectorn&  normalize(vectorn const& min, vectorn const& max);				//!< scale data so that min->0, max->1
	
	// slow binary operations, I recommend you to use _tvectorn<T>::add function, instead.
	friend vectorn operator+( vectorn const& a, vectorn const& b);
	friend vectorn operator-( vectorn const& a, vectorn const& b);
	friend vectorn operator*( vectorn const& a, double b );
	friend vectorn operator*( matrixn const& a, vectorn const& b );
	friend vectorn operator*( vectorn const& a, vectorn const& b );
	friend vectorn  operator/( vectorn const& a, double b);

	//////////////////////////////////////////////////////////////////////
	// binary scalar operations
	//////////////////////////////////////////////////////////////////////
	double angle(vectorn const& other) const;			// calc angle between 0 to pi
	double angle2D(vectorn const& b) const;			// calc angle between 0 to 2pi
	double cosTheta(vectorn const& other) const;			// cosTheta for every dimension
	double	sinTheta(vectorn const& b) const;				// sinTheta for 2 dimension
	
	friend double operator%( vectorn const&, vectorn const& );		// dot product
	friend bool operator<(vectorn const& a, vectorn const& b);
	friend bool operator>(vectorn const& a, vectorn const& b);
	friend bool operator<=(vectorn const& a, vectorn const& b) { return !(a>b);};
	friend bool operator>=(vectorn const& a, vectorn const& b) { return !(a<b);};

	//////////////////////////////////////////////////////////////////////
	// unary operations
	//
	// - binary operation이 unary로 사용가능하기에, 많이 생략되었음. 
	//     ex) a.add(a,b)
	//////////////////////////////////////////////////////////////////////
	vectorn& assign(const vector3& other);
	vectorn& assign(const quater& other);
	vectorn& normalize(vectorn const& a);
	vectorn& assign(const vectorn& other) { __super::assign(other);	return *this;}
	vectorn& resample(vectorn const& vec, int numSample);
	vectorn& concaten(vectorn const& a);

	// 카피해서 받아온다.

	// 카피해서 받아온다.

	vectorn& operator=(const _tvectorn<double>& other)	{ _tvectorn<double>::assign(other);return *this;}
	vectorn& operator=(const vectorn& other);//		{ assign(other);return *this;}	
	vectorn& operator=(const vectornView& other);//	{ assign(other);return *this;}

    vectorn& operator=( vector3 const& other)		{ return assign(other);};
	vectorn& operator=( quater const& other)		{ return assign(other);};
	vectorn& derivative(vectorn const& a);
	vectorn Extract(const intvectorn& columns)	const { vectorn c; c.extract(*this, columns); return c;}
	
	// slow unary operations (negation)
	friend vectorn operator-( vectorn const& a);

	//////////////////////////////////////////////////////////////////////
	// void operations
	//////////////////////////////////////////////////////////////////////
	vectorn& op0(const v0::_op&);
	vectorn& normalize();
	vectorn& negate();


	void findMax(double& max_v, int& max_index,int start=0,int end=INT_MAX) const;
	void findMin(double& min_v, int& min_index,int start=0,int end=INT_MAX) const;
	vectorn& colon(double start, double stepSize, int nSize=-1);
	vectorn& linspace(double x1, double x2, int nSize=-1);
	//!< uniform sampling : sample centers of intervals in linspace of size n+1; eg> uniform (0,3, size=3) -> (  0.5, 1.5, 2.5 ).
	vectorn& uniform(double x1, double x2, int nSize=-1);


	double minimum() const;
	double maximum() const;
	void minimum(matrixn const& a);
	void maximum(matrixn const& a);
	///////////////////////////////////////////////////////////////////////
	// Utility functions
	//////////////////////////////////////////////////////////////////////

	void	  load(const char* filename, bool bLoadFromBinaryFile=true);
	void      save(const char* filename, bool bSaveIntoBinaryFile=true);
	
	vectorn& fromMatrix(matrixn const& mat);
	
	void setVec3( int start, const vector3& src)	{ for(int i=start; i<start+3; i++) (*this)[i]=src.getValue(i-start);}
	void setQuater( int start, const quater& src)	{ for(int i=start; i<start+4; i++) (*this)[i]=src.getValue(i-start);}

	vector3 toVector3(int startIndex=0)	const	{ vector3 out; for(int i=0; i<3; i++) out[i]=getValue(i+startIndex); return out;};
	quater toQuater(int startIndex=0) const		{ quater out; for(int i=0; i<4; i++) out[i]=getValue(i+startIndex); return out;};

	friend class matrixn;

	// deprecated
	void getVec3( int start, vector3& src) const	{ for(int i=start; i<start+3; i++) src[i-start]=value(i);}
	void getQuater( int start, quater& src) const	{ for(int i=start; i<start+4; i++) src[i-start]=value(i);}

	int getSize() const	{ return size();}


	// deprecated - v::for_each, v::for_each1, v::for_each2 로 바꾸는 중.(operatorTemplate.hpp)

	// vector의 각 value들에 scalar binary를 적용한다.
	// ex) v.each(s2::MINIMUM, a, b);

	vectorn& each2(double (*s2_func)(double,double), vectorn const& a, vectorn const& b);
	vectorn& each2(const sv2::_op& op, const matrixn& a, const matrixn& b);
	vectorn& each2(const sv2::_op& op, const matrixn& a, const vectorn& b);
	vectorn& each1(void (*s1_func)(double&,double), vectorn const& a);
	vectorn& each1(void (*s1_func)(double&,double), double a);
	vectorn& each0(void (*s1_func)(double&,double))							{ return each1(s1_func, *this);}

	void op2(const v2::_op& op, const vectorn& a, const vectorn& b)			{ op.calc(*this, a,b);	}	
	void op1(const v1::_op& op, const vectorn& a)							{ op.calc(*this, a);	}

	vectorn Each(void (*s1_func)(double&,double)) const;
	vectorn Each(double (*s2_func)(double,double), vectorn const& b) 
		const;

	
	double length() const;

};



class vectornView :public vectorn
{
public:
	// L-value로 사용될수 있는, reference array로 만든다. 
	vectornView (double* ptrr, int size, int stride);	
	// 값을 reference로 받아온다.
	vectornView(const _tvectorn<double>& other)		{ assignRef(other);}	
	vectornView(const vectorn& other)				{ assignRef(other);}	
	vectornView(const vectornView& other)			{ assignRef(other);}

	~vectornView (){}

	// L-value로 사용되는 경우, 값을 copy한다.
	vectorn& operator=(const vectorn & other)			{ assign(other);return *this;}
	vectorn& operator=(const _tvectorn<double>& other)	{ _tvectorn<double>::assign(other);return *this;}
	vectorn& operator=(const vectornView& other)		{ assign(other);return *this;}
	vectorn& operator=( vector3 const& other)		{ return assign(other);};
	vectorn& operator=( quater const& other)		{ return assign(other);};

};



/*
namespace math
{
	void makeSamplingIndex(vectorn& out, int nLen, int numSample);		 //!< a simplified uniform sampling
	void colon(vectorn& out, double stepSize, int nSize=-1);//!< start부터 stepSize간격으로 size()만큼 채움. (start, start+stepSize, start+stepSize*2,...)
	void linspace(vectorn& out, double x1, double x2, int nSize=-1);		 //!< generates a vector of linearly equally spaced points between X1 and X2; eg> linspace(0,3, size=4) -> (0,   1,   2,   3)
	void uniform(vectorn& out, double x1, double x2, int nSize=-1);		 //!< uniform sampling : sample centers of intervals in linspace of size n+1; eg> uniform (0,3, size=3) -> (  0.5, 1.5, 2.5 ).
	void findMin(vectorn const& in, double& min, int& min_index) const;
	void findMax(double& max, int& max_index) const;
	int argMin(vectorn const& in)			{ double min; int min_index; findMin(min, min_index); return min_index;}
	int argMax(vectorn const& in)			{ double max; int max_index; findMax(max, max_index); return max_index;}
	int argNearest(vectorn const& in, double value) ;
	vectorn& interpolate(vectorn const& a, vectorn const& b, double t);

}*/


namespace v
{
	void eig(vectorn& eigenvalues, const matrixn& mat);
	void linspace(vectorn& out, double x1, double x2, int nSize=-1);		 //!< generates a vector of linearly equally spaced points 
	void findMin(const vectorn& v, double& min_v, int& min_index) ;
	void findMax(const vectorn& v, double& max_v, int& max_index) ;
	intvectorn colon(int start, int end);
}