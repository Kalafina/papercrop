//
// vectorn_n.cpp
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

#include "stdafx.h"
#include "mathclass.h"
#include "float.h"
#include "filter.h"
#include "operatorTemplate.hpp"
//double* allocate(int n);
//void deallocate(double *b);

intvectornView::intvectornView(const intvectorn& other)
{
	assignRef(other);
}

intvectornView intvectorn::range(int start, int end, int step)
{
	return _range<intvectornView >(start, end, step);
}

const intvectornView intvectorn::range(int start, int end, int step) const	{ return ((intvectorn*)this)->range(start, end, step);}

intvectorn& intvectornView::operator=(const intvectorn& other){assign(other);return *this;}	

intvectorn& intvectorn ::operator=(const intvectornView& other){ assign(other);return *this;}

vectorn& vectorn::operator=(const vectorn& other)		{ assign(other);return *this;}	
vectorn& vectorn::operator=(const vectornView& other)	{ assign(other);return *this;}


intvectorn::intvectorn(const intvectornView& other)
:_tvectorn<int>()
{	
	assign(other);
}


intvectornView ::intvectornView (int* ptrr, int size, int str)
:intvectorn(ptrr,size,str)
{
}





intvectorn::intvectorn( int n, int x, ...)	// n dimensional vector	(ex) : vectorn(3, 1.0, 2.0, 3.0);
:_tvectorn<int>()
{
	va_list marker;
	va_start( marker, x);     /* Initialize variable arguments. */

	setSize(n);
	setValue(0, x);
	for(int i=1; i<n; i++)
	{
		setValue(i, va_arg( marker, int));
	}
	va_end( marker );              /* Reset variable arguments.      */	
}

intvectorn&  intvectorn::setAt(intvectorn const& columnIndex, _tvectorn<int> const& value)
{
	ASSERT(value.size()==columnIndex.size());
	for(int i=0; i<columnIndex.size(); i++)
	{
		(*this)[columnIndex[i]]=value[i];
	}
	return *this;
}

intvectorn&  intvectorn::setAt( intvectorn const& columnIndex, int value)
{
	for(int i=0; i<columnIndex.size(); i++)
	{
		(*this)[columnIndex[i]]=value;
	}
	return *this;
}

vectorn intvectorn::toVectorn()
{
	vectorn c;
	c.setSize(size());

	for(int i=0; i<size(); i++)
		c[i]=(double)value(i);
	return c;
}



int intvectorn::minimum() const
{
	const intvectorn& v=*this;
	int min=v[0];
	for(int i=1; i<size(); i++)
	{
		if(v[i]<min) min=v[i];
	}
	return min;
}

int intvectorn::maximum() const
{
	const intvectorn& v=*this;
	int max=v[0];
	for(int i=1; i<size(); i++)
	{
		if(v[i]>max) max=v[i];
	}
	return max;
}

int intvectorn::sum() const
{
	const intvectorn& v=*this;
	int sum=0;
	for(int i=0; i<size(); i++)
	{
		sum+=v[i];
	}
	return sum;	
}

intvectorn&  intvectorn::makeSamplingIndex(int nLen, int numSample)
{
	setSize(numSample);

	// simple sampling
	float len=(float)nLen;

	float factor=1.f/(float)numSample;
	for(int i=0; i<numSample; i++)
	{
		float position=((float)i+0.5f)*factor;
		(*this)[i]=(int)(position*len);
	}
	return *this;
}

intvectorn&  intvectorn::makeSamplingIndex2(int nLen, int numSample)
{
	// 첫프레임과 마지막 프레임은 반드시 포함하고 나머지는 그 사이에서 uniform sampling
	if(numSample<2 || nLen<3)
		return makeSamplingIndex(nLen, numSample);

	setSize(numSample);
	(*this)[0]=0;
	(*this)[numSample-1]=nLen-1;

	if(numSample>2)
	{
		intvectorn samplingIndex;
		samplingIndex.makeSamplingIndex(nLen-2, numSample-2);
		samplingIndex+=1;

		for(int i=1; i<numSample-1; i++)
		{
			this->value(i)=samplingIndex[i-1];
		}
	}
	return *this;
}

intvectorn&  intvectorn::colon(int start, int end, int stepSize)
{
	int nSize;

	/* buggy
	if(stepSize==1)
		nSize=(end-start);
	else
		nSize=(end-start+1)/stepSize;
	*/

	nSize=0;
	for(int i=start; i<end; i+=stepSize)
		nSize++;

	setSize(nSize);
	int currPos=0;
	for(int i=start; i<end; i+=stepSize)
	{
		(*this)[currPos]=i;
		currPos++;
	}
	ASSERT(currPos==nSize);
	return *this;
}

class info
{
public:
	info(){}
	info(const vectorn * pIn, int index){m_pInput=pIn; m_index=index;};
	~info(){}
	const vectorn * m_pInput;
	int m_index;
	static int compareInfo(const void** ppA, const void** ppB)
	{
		info& a=*((info*)*ppA);
		info& b=*((info*)*ppB);
		double valA=a.m_pInput->getValue(a.m_index);
		double valB=b.m_pInput->getValue(b.m_index);
		if(valA<valB)
			return -1;
		if(valA==valB)
			return 0;
		return 1;
	}
	static info* factory(void* param, int index)	{ return new info((const vectorn *) param, index);};
};




int intvectorn::count(int (*s2_func)(int,int), int value, int start, int end)
{
	int count=0;
	if(start<0) start=0;
	if(end>size()) end=size();

	for(int i=start; i<end; i++)
		if(s2_func((*this)[i], value)) count++;

	return count;
}



vectornView ::vectornView (double* ptrr, int size, int str)
:vectorn(ptrr,size,str)
{
}

/////////////////////////////////////////////////////////////////////////////////


vectorn::vectorn( int n, double x)
:_tvectorn<double>()
{
	ASSERT(n==1);
	setSize(n);
	value(0)=x;
}

vectorn::vectorn( int n, double x, double y)
:_tvectorn<double>()
{
	ASSERT(n==2);
	setSize(n);
	value(0)=x;
	value(1)=y;
}
vectorn::vectorn( int n, double x, double y, double z)
:_tvectorn<double>()
{
	ASSERT(n==3);
	setSize(n);
	value(0)=x;
	value(1)=y;
	value(2)=z;
}


vectorn::vectorn( int n, double x, double y, double z, double w,...)	// n dimensional vector	(ex) : vectorn(3, 1.0, 2.0, 3.0);
:_tvectorn<double>()
{
	va_list marker;
	va_start( marker, w);     /* Initialize variable arguments. */

	setSize(n);
	setValue(0, x);
	setValue(1, y);
	setValue(2, z);
	setValue(3, w);
	for(int i=4; i<n; i++)
	{
		setValue(i, va_arg( marker, double));
	}
	va_end( marker );              /* Reset variable arguments.      */	
}

vectornView vectorn::range(int start, int end, int step)
{
	return _range<vectornView >(start,end,step);
}

const vectornView vectorn::range(int start, int end, int step) const	{ return ((vectorn*)this)->range(start, end, step);}


vectorn& vectorn::concaten(vectorn const& a)					
{
	vectorn& c=*this;
	int prev_size=c.size();
	c.resize(c.size()+a.size());
	c.range(prev_size, prev_size+a.size())= a;
	return c;
}


/////////////////////////////////////////////////////////////////////////////////
vectorn::vectorn()
:_tvectorn<double>()
{
}


vectorn::vectorn(const vector3& other)
:_tvectorn<double>()
{
	assign(other);
}

vectorn::vectorn(const quater& other)
:_tvectorn<double>()
{
	assign(other);
}

void vectorn::minimum(matrixn const& a)
{
	v::for_each_column(*this, a, sv::minimum<vectorn, double>);
}
void vectorn::maximum(matrixn const& a)
{
	v::for_each_column(*this, a, sv::maximum<vectorn, double>);
}

double vectorn::minimum() const
{
	return sv::minimum<vectorn, double>(*this);
}
double vectorn::maximum() const
{
	return sv::maximum<vectorn, double>(*this);
}


// 값을 카피해서 받아온다.	
vectorn::vectorn(const _tvectorn<double>& other)
:_tvectorn<double>()
{
	_tvectorn<double>::assign(other);
}	

vectorn::vectorn(const vectorn& other)
:_tvectorn<double>()
{
	assign(other);
}	

vectorn::vectorn(const vectornView& other)
:_tvectorn<double>()
{
	assign(other);
}	

matrixnView vectorn::column() const
{
	return _column<matrixnView >();
}

matrixnView vectorn::row() const		// return 1 by n matrix, which can be used as L-value (reference matrix)
{
	return _row<matrixnView >();
}

vectorn& vectorn::assign(const vector3& other)
{
	setSize(3);
	for(int i=0; i<3; i++)
		value(i)=other.getValue(i);
	return *this;
}

vectorn& vectorn::assign(const quater& other)
{
    setSize(4);
	for(int i=0; i<4; i++)
		value(i)=other.getValue(i);
	return *this;
}


vectorn& vectorn::each1(void (*cOP)(double&,double), vectorn const& a)
{
	vectorn& c=*this;
	c.setSize(a.size());
	for(int i=0; i<a.size(); i++)
		cOP(c[i], a[i]);
	return c;
}

vectorn& vectorn::each1(void (*cOP)(double&,double), double a)
{
	vectorn& c=*this;
	for(int i=0; i<c.size(); i++)
		cOP(c[i], a);
	return c;
}

vectorn& vectorn::each2(const sv2::_op& op, const matrixn& a, const matrixn& b)
{
	vectorn& c=*this;
	c.setSize(a.rows());
	for(int i=0; i<a.rows(); i++)
		c[i]=op.calc(a.row(i), b.row(i));
	return c;
}

vectorn& vectorn::each2(const sv2::_op& op, const matrixn& a, const vectorn& b)
{
	vectorn& c=*this;
	c.setSize(a.rows());
	for(int i=0; i<a.rows(); i++)
		c[i]=op.calc(a.row(i), b);
	return c;
}

vectorn&
vectorn::negate()
{
    vectorn &c = (*this);
    for( int i=0; i<c.size(); i++ )
        c[i] = -c[i];
    return c;
}

double
operator%( vectorn const& a, vectorn const& b )
{
    assert( a.size()==b.size() );

    double c=0;
    for( int i=0; i<a.size(); i++ )
        c += a[i] * b[i];
    return c;
}



bool operator<(vectorn const& a, vectorn const& b)
{
	for(int i=0; i<a.size(); i++)
	{
		if(a[i]>=b[i]) return false;
	}
	return true;
}

bool operator>(vectorn const& a, vectorn const& b)
{
	for(int i=0; i<a.size(); i++)
	{
		if(a[i]<=b[i]) return false;
	}
	return true;
}


double
vectorn::length() const
{
    double c=0;
    for( int i=0; i<size(); i++ )
        c += this->value(i)*this->value(i);
    return sqrt(c);
}


vectorn&
vectorn::normalize()
{
    vectorn &c = (*this);

    double invl = 1/this->length();
    (*this)*=invl;
    return c;
}

vectorn& vectorn::normalize(vectorn const& a)
{
	vectorn &c = (*this);

    double invl = 1/a.length();
    c.mult(a, invl);
    return c;
}

vectorn& vectorn::normalize(vectorn const& min, vectorn const& max)
{
	ASSERT(size()==min.size());
	ASSERT(size()==max.size());

	for(int j=0; j<size(); j++)
	{
		double value=getValue(j);
		value=(value-min[j])/(max[j]-min[j]);
		setValue(j,value);
	}
	return *this;
}



/*
ostream& operator<<( ostream& os, vectorn const& a )
{
    os << "( ";
    for( int i=0; i< a.size()-1; i++ )
        os << a.v[i] << " , ";
    os << a.v[a.size()-1] << " )";
    return os;
}

istream& operator>>( istream& is, vectorn& a )
{
	static char	buf[256];
    //is >> "(";
	is >> buf;
    for( int i=0; i< a.size()-1; i++ )
	{
		//is >> a.v[i] >> ",";
		is >> a.v[i] >> buf;
	}
	//is >> a.v[a.size()-1] >> ")";
	is >> a.v[a.size()-1] >> buf;
    return is;
}
*/

double	vectorn::cosTheta(vectorn const& b) const
{
	vectorn const& a=*this;
	// a dot b= |a||b|cosTheta
	return (a)%(b)/(a.length()*b.length());
}

// calc angle between 0 to pi
double	vectorn::angle(vectorn const& b) const 
{
	vectorn const& a=*this;
	return (double)(ACOS(a.cosTheta(b)));
}

vectorn& vectorn::fromMatrix(matrixn const& mat)
{
	mat.toVector(*this); 
	return *this;
}


void vectorn::load(const char* filename, bool bLoadFromBinaryFile)
{
	assert(0);
	/*
	if(bLoadFromBinaryFile)
	{
		TFile file;
		file.OpenReadFile(filename);
		int nsize=file.UnpackInt();
		setSize(nsize);
		file.UnpackArray(v, size(), sizeof(double));
		file.CloseFile();
	}
	else
	{
		ASSERT(0);
	}*/
}


void vectorn::save(const char* filename, bool bSaveIntoBinaryFile)
{
	assert(0);
	/*
	if(bSaveIntoBinaryFile)
	{
		TFile file;
		file.OpenWriteFile(filename);
		file.PackInt(size());
		file.PackArray(v, size(), sizeof(double));
		file.CloseFile();
	}
	else
	{
		FILE* file=fopen(filename,"w");

		fprintf(file,"size %d \n", size());
		for(int i=0; i<size(); i++)
		{
			fprintf(file,"%f ",(*this)[i]);
		}
        fprintf(file,"\n");
		fclose(file);
	}*/
}



vectorn&  vectorn::each2(double (*s2_func)(double,double), vectorn const& a, vectorn const& b)
{
	vectorn& c=*this;
	ASSERT(a.size()==b.size());
	c.setSize(a.size());
	for(int i=0; i<a.size(); i++)
		c[i]=s2_func(a[i],b[i]);
	return *this;
}


vectorn& vectorn::derivative(vectorn const& a)					
{
	vectorn& c=*this;
	c.setSize(a.size());
	ASSERT(c.size()>2);
	for(int i=1; i<a.size()-1; i++)
	{
		c[i]= (a[i+1]- a[i-1])/2.f;
	}
	c[0]=c[1];
	c[a.size()-1]=c[a.size()-2];
	return c;
}


vectorn operator+( vectorn const& a, vectorn const& b)	{ vectorn c; c.add(a,b); return c;};
vectorn operator-( vectorn const& a, vectorn const& b)	{ vectorn c; c.sub(a,b); return c;};
vectorn operator*( vectorn const& a, double b )			{ vectorn c; c.mult(a,b); return c;};
vectorn operator*( matrixn const& a, vectorn const& b )		{ vectorn c; c.multmat(a,b); return c;};
vectorn operator*( vectorn const& a, vectorn const& b )	{ vectorn c; c.mult(a,b); return c;};
vectorn  operator/( vectorn const& a, double b)			{ vectorn c; c.div(a,b); return c;};



vectorn& vectorn::op0(const v0::_op& c)
{
	c.calc(*this);
	return *this;
}

// calc angle between 0 to 2pi
double	vectorn::angle2D(vectorn const& b) const
{
	vectorn const& a=*this;

	double rad=a.angle(b);
	// length 가 0인경우 발생.
	ASSERT(!_isnan(rad));	
	if(a.sinTheta(b)<0)
		return (double)(2.0*M_PI-rad);

	return rad;
}

double	vectorn::sinTheta(vectorn const& b) const
{
	vectorn const& a=*this;

	// |a cross b| = |a||b|sinTheta
	ASSERT(a.size()==2);
	ASSERT(b.size()==2);

	vectorn a3,b3;
	a3.setSize(3);
	b3.setSize(3);

	a3.x()=a.getValue(0);
	a3.y()=a.getValue(1);
	a3.z()=0;
	b3.x()=b.getValue(0);
	b3.y()=b.getValue(1);
	b3.z()=0;
	double sinTheta;
	if(a%b>0)
	{
		vector3 crs;
		crs.cross(a3.toVector3(),b3.toVector3());
		sinTheta=crs.z/(a.length()*b.length());
	}
	else
	{
		vector3 crs;
		crs.cross(b3.toVector3(),-a3.toVector3());
		sinTheta=crs.z/(a.length()*b.length());
	}
	ASSERT(-1<=sinTheta);
	ASSERT(sinTheta<=1);

	return sinTheta;
}

// namespace vectorUtil

/*

int vectorn::argNearest(double value) const
{
	int argNearest=-1;
	double min=FLT_MAX;

	double dist;
	for(int i=0; i<size(); i++)
	{
		if((dist=(double)(ABS((*this)[i]-value))) < min)
		{
			min=dist;
			argNearest=i;
		}
	}
	return argNearest;
}

int vectorn::argMax(int_vectorn const& columns) const
{
	int argMax=-1;
	double max=-FLT_MAX;
	for(int i=0; i<columns.size(); i++)
	{
		if((*this)[columns[i]]>max)
		{
			max=(*this)[columns[i]];
			argMax=columns[i];
		}
	}
	return argMax;
}


vectorn&  vectorn::resample(vectorn const& vec, int numSample)
{
	setSize(numSample);

	int_vectorn iv;
	iv.makeSamplingIndex(vec.size(), numSample);

	for(int i=0; i<numSample; i++)
	{
		(*this)[i]=vec[iv[i]];
	}
	return *this;
}
vectorn&  vectorn::normalizeSignal(double min, double max)
{
double sigMin=minimum();
double sigMax=maximum();

for(int i=0; i<size(); i++)
value(i)=(value(i)-sigMin)/(sigMax-sigMin)*(max-min)+min;

return *this;
}

vectorn&
vectorn::solve( matrixn const& a, vectorn const& b, int num, double tolerance, double damp )
{
vectorn &c = (*this);
assert( a.rows()==a.cols() );
assert( a.rows()==b.size() );
c.setSize( b.size() );

int flag = TRUE;
for( int i=0; i<num && flag; i++ )
{
flag = FALSE;
for( int j=0; j<a.rows(); j++ )
{
double r = b[j] - a[j]%c;
c[j] += damp*r/a[j][j];
if ( r>tolerance ) flag = TRUE;
}
}

return c;
}


vectorn&
vectorn::solve( matrixn const& a, vectorn const& b )
{
vectorn &c = (*this);
assert( a.rows()==a.cols() );
assert( a.rows()==b.size() );

int n = b.size();
c.setSize( n );
c.assign( b );

static matrixn mat; mat.setSize( n, n );
mat.assign( a );

static int* index;
static int index_count = 0;
if ( index_count<n )
{
if ( index_count>0 ) delete[] index;
index_count = n;
if ( index_count>0 ) index = new int[index_count];
}

mat.LUdecompose( index );
mat.LUsubstitute( index, c );

return c;
}


vectorn&
vectorn::solve( matrixn const& a, vectorn const& b, double tolerance )
{
int m = a.rows();
int n = a.cols();

assert( m >= n );
assert( b.size()==m );

vectorn &c = (*this);
c.setSize( n );

static matrixn u; u.setSize( m, n );
static vectorn w; w.setSize( n );
static matrixn v; v.setSize( n, n );

u.assign( a );
NR_OLD::SVdecompose( u, w, v );

int i, j;
double s;
static vectorn tmp; tmp.setSize( n );

double wmax = 0.0f;
for( j=0; j<n; j++ )
if ( w[j] > wmax ) wmax = w[j];

for( j=0; j<n; j++ )
if ( w[j] < wmax * tolerance ) w[j] = 0.0f;

for( j=0; j<n; j++ )
{
s = 0.0f;
if ( w[j] )
{
for( i=0; i<m; i++ )
s += u[i][j] * b[i];
s /= w[j];
}
tmp[j] = s;
}

for ( j=0; j<n; j++ )
{
s = 0.0;
for ( i=0; i<n; i++ )
s += v[j][i] * tmp[i];
c[j] = s;
}

return c;
}


vectorn&  vectorn::colon(double start, double stepSize, int nSize)
{
if(nSize!=-1)
setSize(nSize);

double cur=start;
for(int i=0; i<size(); i++)
{
v[i]=cur;
cur+=stepSize;
}
return *this;
}

vectorn& vectorn::linspace(double x1, double x2, int nSize)
{
if(nSize!=-1)
setSize(nSize);

// simple sampling
double len=x2-x1;

double factor=1.f/((double)size()-1);
for(int i=0; i<size(); i++)
{
// position : increases from 0 to 1
double position=((double)i)*factor;
(*this)[i]=position*len+x1;
}
return *this;
}

vectorn&  vectorn::uniform(double x1, double x2, int nSize)
{
if(nSize!=-1)
setSize(nSize);

// simple sampling
double len=x2-x1;
double factor=1.f/((double)size());

for(int i=0; i<size(); i++)
{
double position=((double)(i+(i+1)))*factor/2.f;
(*this)[i]=position*len+x1;
}
return *this;
}


vectorn&  vectorn::makeSamplingIndex(int nLen, int numSample)
{
setSize(numSample);

// simple sampling
double len=(double)nLen;

double factor=1.f/(double)numSample;
for(int i=0; i<numSample; i++)
{
double position=((double)i+0.5f)*factor;
(*this)[i]=(double)(position*len);
}
return *this;
}

vectorn& vectorn::interpolate(vectorn const& a, vectorn const& b, double t)
{
ASSERT(a.size()==b.size());
setSize(a.size());

for(int i=0; i<n; i++)
{
(*this)[i]=a[i]*(1-t)+b[i]*t;
}
return *this;
}


vectorn& vectorn::sort(vectorn const& source, int_vectorn& sortedIndex)
{
sortedIndex.sortedOrder(source);
return extract(source, sortedIndex);
}

*/


intvectorn&  intvectorn::sortedOrder(vectorn const & input)
{
	//!< input[0]<input[2]<input[1]<input[3]인경우 결과는 [0213]

	ASSERT(0);
	return *this;
}

intvectorn& intvectorn::findIndex(intvectorn const& source, int value)
{
	int count=0;
	for(int i=0; i<source.size(); i++)
	{
		if(source[i]==value)
			count++;
	}
	setSize(count);
	count=0;
	for(int i=0; i<source.size(); i++)
	{
		if(source[i]==value)
		{
			(*this)[count]=i;
			count++;
		}
	}
	return *this;
}

int intvectorn::findFirstIndex(int value) const
{
	for(int i=0; i<size(); i++)
		if((*this)[i]==value)
			return i;
	return -1;
}




vectorn&  vectorn::colon(double start, double stepSize, int nSize)
{
	vectorn& v=*this;
	if(nSize!=-1)
		setSize(nSize);

	double cur=start;
	for(int i=0; i<size(); i++)
	{
		v[i]=cur;
		cur+=stepSize;
	}
	return *this;
}

vectorn& vectorn::linspace(double x1, double x2, int nSize)
{
	if(nSize!=-1)
		setSize(nSize);

	// simple sampling
	double len=x2-x1;

	double factor=1.f/((double)size()-1);
	for(int i=0; i<size(); i++)
	{
		// position : increases from 0 to 1
		double position=((double)i)*factor;
		(*this)[i]=position*len+x1;
	}
	return *this;
}

vectorn&  vectorn::uniform(double x1, double x2, int nSize)
{
	if(nSize!=-1)
		setSize(nSize);

	// simple sampling
	double len=x2-x1;
	double factor=1.f/((double)size());

	for(int i=0; i<size(); i++)
	{
		double position=((double)(i+(i+1)))*factor/2.f;
		(*this)[i]=position*len+x1;
	}
	return *this;
}

void vectorn::findMin(double& min_v, int& min_index,int start,int end) const
{
	const vectorn& v=*this;
	min_v=FLT_MAX;

	min_index=0;
	if(end>size())end=size();
	assert(start>=0&&end>start);
	for(int i=start; i<end; i++)
	{
		if(v[i]<min_v)
		{
			min_v=v[i];
			min_index=i;
		}
	}
}


void  vectorn::findMax(double& max_v, int& max_index,int start,int end) const
{
	const vectorn& v=*this;
	max_v=-FLT_MAX;
	if(end>size())end=size();
	assert(start>=0&&end>start);
	for(int i=start; i<end; i++)
	{
		if(v[i]>max_v)
		{
			max_v=v[i];
			max_index=i;
		}
	}
}

void v::linspace(vectorn& out, double x1, double x2, int nSize)
{
	if(nSize!=-1)
	out.setSize(nSize);

	// simple sampling
	double len=x2-x1;

	double factor=1.f/((double)out.size()-1);
	for(int i=0; i<out.size(); i++)
	{
		// position : increases from 0 to 1
		double position=((double)i)*factor;
		out[i]=position*len+x1;
	}
}


void v::findMin(const vectorn& v, double& min_v, int& min_index) 
{
	min_v=FLT_MAX;

	min_index=0;

	for(int i=0; i<v.size(); i++)
	{
		if(v[i]<min_v)
		{
			min_v=v[i];
			min_index=i;
		}
	}
}

void v::findMax(const vectorn& v, double& max_v, int& max_index) 
{
	max_v=-FLT_MAX;

	for(int i=0; i<v.size(); i++)
	{
		if(v[i]>max_v)
		{
			max_v=v[i];
			max_index=i;
		}
	}
}

intvectorn v::colon(int start, int end)
{
	intvectorn c; c.colon(start, end); return c;	
}


vectorn operator-( vectorn const& a)							{ vectorn neg(a);neg.negate();return neg;}
vectorn vectorn::Each(void (*cOP)(double&,double)) const					{ vectorn c; c.assign(*this); return c.each1(cOP,*this);}
vectorn vectorn::Each(double (*cOP)(double,double), vectorn const& b)	const { vectorn c; return c.each2(cOP, *this,b);}