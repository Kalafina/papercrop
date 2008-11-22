#include "stdafx.h"
#include "limits.h"
#include "mathclass.h"
#include "operator.h"
#include "filter.h"
#include "../utility/operatorstring.h"
namespace sop
{
	int interpolateInt(double t, int s, int e)
	{
		double f=double(s)*(1.0-t)+double(e)*t;
		return ROUND(f);
	}

	double interpolate(double t, double s, double e)
	{
		double f=s*(1.0-t)+e*t;
		return f;
	}

	double map(double t, double min, double max, double v1, double v2)
	{
		t=(t-min)/(max-min);
		return interpolate(t, v1, v2);
	}

	double clampMap(double t, double min, double max, double v1, double v2)
	{
		if(t<min) t=min;
		if(t>max) t=max;

		t=(t-min)/(max-min);
		return interpolate(t, v1, v2);
	}

	double smoothTransition(double a)
	{ return ((double)-2.0)*a*a*a+((double)3.0)*a*a;} // y=-2x^3+3x^2
}


index2 m::argMin(matrixn const& a)
{
	index2 argMinV;
	double minV=DBL_MAX;
	for(int i=0; i<a.rows(); i++)
	{
		for(int j=0; j<a.cols(); j++)
		{
			if(a(i,j)<minV)
			{
				minV=a(i,j);
				argMinV=index2(i,j);
			}
		}
	}
	return argMinV;
}


int v::argMinRand(vectorn const& a, double thr, int start, int end)
{
	if (end>a.size()) 
		end=a.size();
	int argMinV=a.argMin();

	intvectorn indexes;

	double minV=a(argMinV)*thr;
	for(int i=start; i<end; i++)
	{
		if(a[i]<minV)
			indexes.pushBack(i);
	}

	return indexes[rand()%indexes.size()];
}


index2 m::argMinRand(matrixn const& a, double thr)
{
	index2 argMinV=argMin(a);

	std::vector<index2> indexes;

	double minV=a(argMinV(0), argMinV(1))*thr;
	for(int i=0; i<a.rows(); i++)
	{
		for(int j=0; j<a.cols(); j++)
		{
			if(a(i,j)<minV)
			{
				indexes.push_back(index2(i,j));
			}
		}
	}

	return indexes[rand()%indexes.size()];
}

namespace m
{
	void multAB(matrixn& out, matrixn const& a, matrixn const& b, bool transposeA, bool transposeB)
	{
		if(transposeA)
		{
			if(transposeB)
				out.multAtBt(a,b);
			else
				out.multAtB(a,b);
		}
		else
		{
			if(transposeB)
				out.multABt(a,b);
			else
				out.mult(a,b);
		}
	}

	void multABC(matrixn& out, matrixn const& a, matrixn const& b, matrixn const& c, bool transposeA, bool transposeB, bool transposeC)
	{
		int rowA=a.rows();
		int colA=a.cols();
		int rowB=b.rows();
		int colB=b.cols();
		int rowC=c.rows();
		int colC=c.cols();

		if(transposeA)	SWAP<int>(rowA, colA);
		if(transposeB)	SWAP<int>(rowB, colB);
		if(transposeC)	SWAP<int>(rowC, colC);

		ASSERT(colA==rowB);
		ASSERT(colB==rowC);
		
		int aa=rowA;
		int bb=rowB;
		int cc=rowC;
		int dd=colC;

		if(aa*bb*cc+aa*cc*dd < bb*cc*dd+aa*bb*dd)
		{
			// case 1: (ab)c is efficient than a(bc)
			matrixn ab;
			m::multAB(ab, a, b, transposeA, transposeB);
			m::multAB(out, ab, c, false, transposeC);
		}
		else
		{
			// case 1: a(bc) is efficient than (ab)c
			matrixn bc;
			m::multAB(bc, b, c, transposeB, transposeC);
			m::multAB(out, a, bc, transposeA, false);
		}
	}

	void multAtB(vectorn& out, matrixn const& A, vectorn const& b)
	{
		ASSERT(A.cols()==b.size());
		// out.setSize(A.rows());
		// out.column().multAtB(A, b.column()); -> slow
		out.multmat(b,A);
	}


	
	matrixn op1(m1::_op const& op, matrixn const& A)	
	{
		matrixn c; c.op1(op, A); return c;
	}

	matrixn diag(vectorn const& a)
	{
		matrixn c;
		c.setSize(a.size(),a.size());
		c.setAllValue(0.0);
		c.setDiagonal(a);
		return c;
	}

	
	
	
}

void v0::colon::calc(vectorn& c) const
{
	int count=0;
	for(int i=mStart; i<mEnd; i+=mStepSize)
		count++;

	c.setSize(count);
	count=0;
	for(int i=mStart; i<mEnd; i+=mStepSize)
		c[count++]=i;
}
		
void v0::pow::calc(vectorn &c) const	
{
	for(int i=0; i<c.size(); i++) c[i]=::pow(c[i], mK);
}
void v0::useUnary::calc(vectorn& c) const
{
	vectorn temp(c);
	m_cOperator.calc(c,temp);
}

void v0::transition::calc(vectorn& c) const
{
	c.setSize(mnSize);

	double totalTime=mnSize-1;
	double currTime;
	for(int i=0; i<mnSize; i++)
	{
		currTime=(float)i/totalTime;
		float t=-2.f*CUBIC(currTime)+3.f*SQR(currTime);
		c[i]=sop::interpolate(t, mStart, mEnd);
	}
}

void v0::decay::operator()(vectorn& c) const
{
	c.setSize(mnSize);

	double totalTime=mnSize-1;
	double currTime;
	for(int i=0; i<mnSize; i++)
	{
		currTime=(double)i/totalTime;
		double t;
		switch(mType)
		{
		case TRANSITION:
			t=1.0-(-2.f*CUBIC(currTime)+3.f*SQR(currTime));
			break;
		case COS:
			t=cos(currTime*M_PI*0.5);
			break;
		case LINEAR:
			t=1.0-currTime;
			break;
		}
		c[i]=t*mStart;
	}
}
void v0::decay::calc(vectorn& c) const 
{
	this->operator()(c);
}

void v0::zeroToOne::operator()(vectorn& c) const
{
	if(mType==LINEAR)
	{
		v0::decay(1.0, mnSize, v0::decay::LINEAR)(c);
		c.sub(1.0, c);
	}
	else if(mType==SLOW_START)
	{
		v0::decay(1.0, mnSize, v0::decay::COS)(c);
		c.sub(1.0, c);
	}
	else if(mType==SLOW_END)
	{
		vectorn temp;
		v0::decay(1.0, mnSize, v0::decay::COS)(temp);
		v1::reverse()(c, temp);
	}
	else
	{
		v0::decay(1.0, mnSize, v0::decay::TRANSITION)(c);
		c.sub(1.0, c);
	}
}

void v0::oneToZero::operator()(vectorn& c) const
{
	if(mType==LINEAR)
	{
		v0::decay(1.0, mnSize, v0::decay::LINEAR)(c);
	}
	else if(mType==SLOW_START)
	{		
		v0::decay(1.0, mnSize, v0::decay::COS)(c);
	}
	else if(mType==SLOW_END)
	{
		vectorn temp;
		v0::decay(1.0, mnSize, v0::decay::COS)(temp);
		temp.sub(1.0, temp);
		v1::reverse()(c, temp);		
	}
	else
	{
		v0::decay(1.0, mnSize, v0::decay::TRANSITION)(c);
	}
}
/*
void v0::each::calc(vectorn& c) const
{
	for(int i=0; i<c.size(); i++)
		m_cOperator.Calc(c[i], c[i]);
}


void v0::useUnary::calcInt(intvectorn& c) const
{
	intvectorn temp(c);
	m_cOperator.calcInt(c,temp);
}

v0::domain::domain(const bitvectorn& abIndex, const v0::Operator& op)
:m_op(op)
{
	m_aIndex.findIndex(abIndex,true);
}

v0::domain::domain(int start, int end, int stepSize, const v0::Operator& op)
:m_op(op)
{
	m_aIndex.colon(start, end, stepSize);
}

void v0::domain::calc(vectorn& c) const
{
	vectorn temp;
	temp.op1(v1::extract(m_aIndex), c);
	temp.op0(m_op);
	
	if(temp.size()!=m_aIndex.size())
		printf("Error in v0::domain::calcInt\n");
	else
		c.op1(v1::assign(m_aIndex), temp);
}

void v0::domain::calcInt(intvectorn& c) const
{
	intvectorn temp;
	temp.op1(v1::extract(m_aIndex), c);
	temp.op0(m_op);
	
	if(temp.size()!=m_aIndex.size())
		printf("Error in v0::domain::calcInt\n");
	else
		c.op1(v1::assign(m_aIndex), temp);
}

v1::domainRange::domainRange(const bitvectorn& abIndex, const v1::Operator& op)
:m_op(op)
{
	m_aInIndex.findIndex(abIndex,true);
	m_aOutIndex=m_aInIndex;

}

v1::domainRange::domainRange(int start, int end, int stepSize, const v1::Operator& op)
:m_op(op)
{
	m_aInIndex.colon(start, end, stepSize);
	m_aOutIndex=m_aInIndex;
}

void v1::domainRange::calc(vectorn& c, const vectorn& a) const
{
	vectorn tempc;
	vectorn tempa;
	
	tempc.op1(v1::extract(m_aOutIndex),c);
	tempa.op1(v1::extract(m_aInIndex),a);
	tempc.op1(m_op, tempa);
	
	if(m_aInIndex.size()!=m_aOutIndex.size())
		printf("Error in v1::domain::calcInt\n");
	else
		c.op1(v1::assign(m_aOutIndex), tempc);
}

v1::domain::domain(int start, int end, const v1::Operator& op)
:m_op(op)
{
	m_aInIndex.colon(start, end);
}
void v1::domain::calc(vectorn& c, const vectorn& a) const
{
	vectorn tempa;

	tempa.op1(v1::extract(m_aInIndex),a);
	c.op1(m_op, tempa);
}

void v1::domainRange::calcInt(intvectorn& c, const intvectorn& a) const
{
	intvectorn tempc;
	intvectorn tempa;
	
	tempc.op1(v1::extract(m_aOutIndex),c);
	tempa.op1(v1::extract(m_aInIndex),a);
	tempc.op1(m_op, tempa);
	
	if(m_aInIndex.size()!=m_aOutIndex.size())
		printf("Error in v1::domain::calcInt\n");
	else
		c.op1(v1::assign(m_aOutIndex), tempc);
}

void v1::domainRange::calc(vectorn& c, double a) const
{
	vectorn temp;
	temp.op1(v1::extract(m_aOutIndex), c);
	temp.op1(m_op, a);

	if(temp.size()!=m_aOutIndex.size())
		printf("Error in v1::domain::calcInt\n");
	else
		c.op1(v1::assign(m_aOutIndex), temp);
}

void v1::domainRange::calcInt(intvectorn& c, int a) const
{
	intvectorn temp;
	temp.op1(v1::extract(m_aOutIndex), c);
	temp.op1(m_op, a);

	if(temp.size()!=m_aOutIndex.size())
		printf("Error in v1::domain::calcInt\n");
	else
		c.op1(v1::assign(m_aOutIndex), temp);
}*/


	// c.size()=a.size()-1. c[i]=a[i+1]-a[i].
void v1::delta::calc(vectorn& c, const vectorn& a) const
{
	operator()(c,a);
}

void v1::delta::operator()(vectorn& c, const vectorn& a) const
{
	c.setSize(a.size()-1);
	for(int i=0; i<c.size(); i++)
		c[i]=a[i+1]-a[i];
}


void v1::cumulate::calc(vectorn& c, const vectorn& a) const
{
	c.setSize(a.size()+1);
	c[0]=0.0;

	for(int i=0; i<a.size(); i++)
		c[i+1]=c[i]+a[i];
}


/*
v1::extract::extract(const intvectorn& index)
{
	m_vIndex.assign(index);
}

v1::extract::extract(int start, int end, int stepSize)
{
	m_vIndex.colon(start, end, stepSize);
}

void v1::extract::calc(vectorn& c, const vectorn& a) const
{
	c.setSize(m_vIndex.size());

	for(int j=0; j<m_vIndex.size(); j++)
		c[j]=a[m_vIndex[j]];
}

void v1::extract::calcInt(intvectorn& c, const intvectorn& a) const
{
	c.setSize(m_vIndex.size());

	for(int j=0; j<m_vIndex.size(); j++)
		c[j]=a[m_vIndex[j]];
}*/

v1::assign::assign(const intvectorn& index)
{
	m_vIndex.assign(index);
}

v1::assign::assign(int start, int end, int stepSize)
{
	m_vIndex.colon(start, end, stepSize);
}

void v1::assign::calc(vectorn& c, const vectorn& a) const
{
	if(m_vIndex.size())
	{
		for(int j=0; j<m_vIndex.size(); j++)
			c[m_vIndex[j]]=a[j];
	}
	else
	{
		c.assign(a);
	}
}
/*
void v1::assign::calcInt(intvectorn& c, const intvectorn& a) const
{
	for(int j=0; j<m_vIndex.size(); j++)
		c[m_vIndex[j]]=a[j];
}*/


/*v2::domain::domain(int start, int end, int stepSize, const v2::Operator& op)
:m_op(op)
{
	m_cIndex.colon(start, end, stepSize);
	m_aIndex=m_cIndex;
	m_bIndex=m_cIndex;
}

void v2::domain::calc(vectorn& c, const vectorn& a, const vectorn& b) const
{
	vectorn tempc;
	vectorn tempa;
	vectorn tempb;
	
	tempc.op1(v1::extract(m_cIndex),c);
	tempa.op1(v1::extract(m_aIndex),a);
	tempb.op1(v1::extract(m_bIndex),b);
	tempc.op2(m_op, tempa, tempb);
	
	if(m_aIndex.size()!=m_cIndex.size())
		printf("Error in v2::domain::calc\n");
	else
		c.op1(v1::assign(m_cIndex), tempc);
}

void v2::domain::calcInt(intvectorn& c, const intvectorn& a, const intvectorn& b) const
{
	intvectorn tempc;
	intvectorn tempa;
	intvectorn tempb;
	
	tempc.op1(v1::extract(m_cIndex),c);
	tempa.op1(v1::extract(m_aIndex),a);
	tempb.op1(v1::extract(m_bIndex),b);
	tempc.op2(m_op, tempa, tempb);
	
	if(m_aIndex.size()!=m_cIndex.size())
		printf("Error in v2::domain::calc\n");
	else
		c.op1(v1::assign(m_cIndex), tempc);
}
*/

/*
void v2::each::calc(vectorn& c, const vectorn& a, const vectorn& b) const
{
	ASSERT(a.size()==b.size());
	c.setSize(a.size());
	for(int i=0; i<a.size(); i++)
		c[i]=m_cOperator.Calc(a[i],b[i]);
}	

void v2::each::calc(vectorn& c, const vectorn& a, double b) const
{
	c.setSize(a.size());
	for(int i=0; i<a.size(); i++)
		c[i]=m_cOperator.Calc(a[i],b);
}	

void v2::each::calcInt(intvectorn& c, const intvectorn& a, const intvectorn& b) const
{
	ASSERT(a.size()==b.size());
	c.setSize(a.size());
	for(int i=0; i<a.size(); i++)
		c[i]=m_cOperator.CalcInt(a[i],b[i]);
}

void v2::each::calcInt(intvectorn& c, const intvectorn& a, int b) const
{
	c.setSize(a.size());
	for(int i=0; i<a.size(); i++)
		c[i]=m_cOperator.CalcInt(a[i],b);
}

*/

void v2::multAdd::calc(vectorn& c, const vectorn& a, const vectorn& b) const
{
	ASSERT(c.size()==a.size() && c.size()==b.size());

	for(int i=0; i<c.size(); i++)
	{
		c[i]+=a[i]*b[i];
	}
}

void v2::cross::calc(vectorn& c, const vectorn& a, const vectorn& b) const
{
	vector3 vc;
	vc.cross(a.toVector3(), b.toVector3());
	c.assign(vc);
}

void v2::minimum::calc(vectorn& c, const vectorn& a, const vectorn& b) const 
{
	c.each2(s2::MINIMUM, a,b);	
}

void v2::maximum::calc(vectorn& c, const vectorn& a, const vectorn& b) const 
{
	c.each2(s2::MAXIMUM, a,b);	
}

void v2::add::calc( vectorn& c, const vectorn & a, const vectorn & b ) const
{
    assert( a.size()==b.size() );
    c.setSize( a.size() );

    for( int i=0; i<a.size(); i++ )
        c[i] = a[i] + b[i];
}


void v2::sub::calc( vectorn& c, const vectorn & a, const vectorn & b ) const
{
    assert( a.size()==b.size() );
    c.setSize( a.size() );

    for( int i=0; i<a.size(); i++ )
        c[i] = a[i] - b[i];
}


void v2::mid::calc(vectorn& c, const vectorn& a, const vectorn& b) const
{
	ASSERT(a.size()==b.size());
	c.add(a,b);
	c/=2.0;
}


void v2::interpolate::calc(vectorn& c, const vectorn& a, const vectorn& b) const
{
	ASSERT(a.size()==b.size());
	c.setSize(a.size());
	for(int i=0; i<c.size(); i++)
	{
		c[i]=(1.0-m_fT)*a[i]+m_fT*b[i];
	}
}

/*
/// c=ADD(c,d) 처럼 binaryop를 사용한다. eg) matrix1.op1(m1::useBinary(m2::...()))..

m0::domain::domain(int start, int end, int stepSize, const m0::Operator& op)
:m_op(op)
{
	m_aIndex.colon(start, end, stepSize);
}

void m0::domain::calc(matrixn& c) const
{
	matrixn temp;
	temp.op1(m1::extractRows(m_aIndex), c);
	temp.op0(m_op);
	
	if(temp.rows()!=m_aIndex.size())
		Msg::error("Error in m0::domain::calc\n");
	else
		c.op1(m1::assignRows(m_aIndex), temp);
}

m1::assignRows::assignRows(const intvectorn& index)
{
	m_vIndex.assign(index);
}

m1::assignRows::assignRows(int start, int end, int stepSize)
{
	m_vIndex.colon(start, end, stepSize);
}

void m1::assignRows::calc(matrixn& c, const matrixn& a) const
{
	for(int j=0; j<m_vIndex.size(); j++)
		c[m_vIndex[j]]=a[j];
}

m1::domainRange::domainRange(const bitvectorn& abIndex, const m1::Operator& op)
:m_op(op)
{
	m_aInIndex.findIndex(abIndex,true);
	m_aOutIndex=m_aInIndex;

}

m1::domainRange::domainRange(int start, int end, int stepSize, const m1::Operator& op)
:m_op(op)
{
	m_aInIndex.colon(start, end, stepSize);
	m_aOutIndex=m_aInIndex;
}

void m1::domainRange::calc(matrixn& c, const matrixn& a) const
{
	matrixn tempc;
	matrixn tempa;
	
	// unary operator 에서 c는 사용하지 않는 경우가 있다. 이경우 사용자가 c의 크기를 일일이 세팅하지 않는경우가 있다.
	// 이경우 양쪽 크기가 다르면 에러가 날수 있으므로 아래를 추가하였다. c와 a의 크기가 같은 일반적인 경우 영향없다.
	c.setSameSize(a);

	tempc.op1(m1::extractRows(m_aOutIndex),c);
	tempa.op1(m1::extractRows(m_aInIndex),a);
	tempc.op1(m_op, tempa);
	
	if(m_aInIndex.size()!=m_aOutIndex.size())
		Msg::error("Error in m1::domain::calcInt (in, out size are different!)");
	else
		c.op1(m1::assignRows(m_aOutIndex), tempc);
}
*/

void m0::diagonalize::calc(matrixn& c) const
{
	ASSERT(c.rows()==c.cols());
	int nbands=c.rows();
    int b1,b2;

    for(b1=0; b1<nbands; b1++)
    for(b2=0; b2<nbands; b2++)
      if(b1!=b2) c[b1][b2] = 0;

}

void v1::useBinary::calc(vectorn& c, const vectorn& a) const
{
	vectorn temp(c);
	m_op.calc(c, temp, a);
}

void m0::useUnary::calc(matrixn& c) const
{
	matrixn temp(c);
	m_cOperator.calc(c, temp);
}
/*

void v1::useBinary::calcInt(intvectorn& c, const intvectorn& a) const
{
	intvectorn temp(c);
	m_op.calcInt(c, temp, a);
}

void v1::each::calc(vectorn& c, const vectorn& a) const
{
	
}

void v1::each::calc(vectorn& c, double a) const
{
	for(int i=0; i<c.size(); i++)
		m_cOperator.Calc(c[i], a);
}*/



/*
void v1::each::calcInt(intvectorn& c, const intvectorn& a) const
{
	for(int i=0; i<c.size(); i++)
		m_cOperator.CalcInt(c[i], a[i]);
}

void v1::each::calcInt(intvectorn& c, int a) const
{
	for(int i=0; i<c.size(); i++)
		m_cOperator.CalcInt(c[i], a);
}
void v1::concat::calc(vectorn& c, const vectorn& a) const
{
	int prev_size=c.size();
	c.resize(c.size()+a.size());
	c.setValue(prev_size, prev_size+a.size(), a);
}

void v1::concat::calcInt(intvectorn& c, const intvectorn& a) const
{
	int prev_size=c.size();
	c.resize(c.size()+a.size());
	c.setValue(prev_size, prev_size+a.size(), a);
}

void v1::derivative::calc(vectorn& c, const vectorn& a) const
{
	c.setSize(a.size());
	ASSERT(c.size()>2);
	for(int i=1; i<a.size()-1; i++)
	{
		c[i]= (a[i+1]- a[i-1])/2.f;
	}
	c[0]=c[1];
	c[a.size()-1]=c[a.size()-2];
}



void m0::each::calc(matrixn& c) const
{
	int prev_column=c.cols();

    // output size determination
	vectorn out(prev_column);

	for(int i=0; i<c.rows(); i++)
	{
		out.setSize(prev_column);
		out.setValue(0, prev_column, c[i]);
		m_cOperator.calc(out);
		c[i].assign(out);
	}
}*/
/*
void m0::align::calc(matrixn& c) const
{
	for(int i=1; i<c.rows(); i++)
	{
		if(c[i-1]%c[i]<0)
			c[i]*=-1.f;
	}
}*/
#include "../image/image.h"
#include "../image/imagepixel.h"
#include "../image/imageprocessor.h"

m0::drawSignals::drawSignals(const char* filename, bool useTheSameMinMax, const intvectorn& vXplot)
: m_strFilename(filename), m_fMin(0.f), m_fMax(0.f), m_bMultiDimensional(true), m_vXplot(vXplot),m_bUseTheSameMinMax(useTheSameMinMax)
{
}

void m0::drawSignals::calc(matrixn& c) const
{
	CImage* pImage;
	if(m_bMultiDimensional)
	{
		matrixn t;
		t.transpose(c);

		if(m_bUseTheSameMinMax)
			pImage=Imp::DrawChart(t, Imp::LINE_CHART, m_fMin, m_fMax);
		else
		{
			vectorn aMin, aMax;
			aMin.minimum(c);
			aMax.maximum(c);
			aMax.each1(s1::RADD, 0.000001); 
			pImage=Imp::DrawChart(t, Imp::LINE_CHART, aMin, aMax);
		}
	}
	else
		pImage=Imp::DrawChart(c, Imp::LINE_CHART, m_fMin, m_fMax);

	CImagePixel cip(pImage);
	
	for(int i=0; i<m_vXplot.size(); i++)
	{
		cip.DrawVertLine(m_vXplot[i], 0, cip.Height(), CPixelRGB8(0,0,0),true);
	}

	Imp::SafeDelete(pImage, m_strFilename);

}

void m0::draw::calc(matrixn& c) const
{
	double min, max;
	min=m_fMin;
	max=m_fMax;

	if(min==max)
	{
		min=c.toVector().minimum();
		max=c.toVector().maximum();
	}

	CImage* pImage;
	pImage=Imp::DrawMatrix2(c, min, max);

	CImagePixel cip(pImage);
	cip.DrawText(0, 0, sz1::format("[%f,%f]", min, max));

	Imp::SafeDelete(pImage, m_strFilename);
}

void m0::plotScatteredData::calc(matrixn& c) const
{
	CImage* pImage;
	pImage=Imp::Plot(c, m_vMin, m_vMax);
	Imp::SafeDelete(pImage, TString("myplot/")+m_strFilename);
}
/*
void m0::linspace::calc(matrixn& c) const
{
	vectorn space;
	space.linspace(0.f, 1.f, m_n);
	c.setSize(m_n, m_vStart.size());
	for(int i=0; i<m_n; i++)
		c[i].interpolate(m_vStart, m_vEnd, space[i]);
}
*/
/*void m1::extractRows::calc(matrixn& c, const matrixn& a) const
{
	c.extractRows(a, m_aIndex);
}*/

	
/*void m1::each::calc(matrixn& c, const matrixn& a) const
{
	ASSERT(c.rows()==a.rows());

	int prev_column=c.cols();

    // output size determination
	vectorn out(prev_column);

	for(int i=0; i<a.rows(); i++)
	{
		out.setSize(prev_column);
		out.setValue(0, prev_column, c[i]);
		m_cOperator.calc(out, a[i]);
		c[i].assign(out);
	}
}*/

void m1::derivative::calc(matrixn& c, const matrixn& a) const
{
	c.setSameSize(a);
	ASSERT(c.rows()>2);
	for(int i=1; i<a.rows()-1; i++)
	{
		c.row(i).sub(a.row(i+1), a.row(i-1));
		c.row(i)*=1/2.f;
	}
	c.setRow(0, c.row(1));
	c.setRow(a.rows()-1, c.row(a.rows()-2));
}


void m1::downSampling::calc(matrixn& c, const matrixn& a) const/// c.op(a)
{
	int start=m_nStart;
	int end=m_nEnd;

	if(start<0) start=0;
	if(end>a.rows()) end=a.rows();
	ASSERT((end-start)%m_nXn==0);

	c.setSize((end-start)/m_nXn, a.cols());

	c.setAllValue(0);
	for(int i=0; i<c.rows(); i++)
	{
		for(int j=0; j<m_nXn; j++)
		{
			c.row(i)+=a.row(i*m_nXn+j);
		}
		c.row(i)/=(double)m_nXn;
	}
}
/*
void m1::NR_OLD::SVDInverse::calc(matrixn& c, const matrixn& a) const
{
	c.inverse(a);
}

void m1::NR_OLD::PseudoInverse::calc(matrixn& c, const matrixn& a) const
{
	c.pseudoInverse(a);
}

void m2::each::calc(matrixn& c, const matrixn& a, const matrixn& b) const
{
	// component wise
	ASSERT(a.rows()==b.rows());
	
	// output size detection
	vectorn outfirst;
	m_cOperator.calc(outfirst, a[0], b[0]);
	c.setSize(a.rows(), outfirst.size());
	c[0].assign(outfirst);
	
	for(int i=1; i<a.rows(); i++)
		m_cOperator.calc(c[i], a[i], b[i]);
}
*/

void v0::uniformSampling::calc(vectorn& c) const
{
	if(nSize!=-1)
		c.setSize(nSize);

	// simple sampling
	double len=x2-x1;
	double factor=1.f/((double)c.size());

	for(int i=0; i<c.size(); i++)
	{
		double position=((double)(i+(i+1)))*factor/2.f;
		c[i]=position*len+x1;
	}
}


void v0::drawSignals::calc(vectorn& c) const
{
	CImage* pImage;
	pImage=Imp::DrawChart(c, Imp::LINE_CHART, m_fMin, m_fMax);

	CImagePixel cip(pImage);

	for(int i=0; i<m_vXplot.size(); i++)
	{
		cip.DrawVertLine(m_vXplot[i], 0, cip.Height(), CPixelRGB8(0,0,0),true);
	}
	if(m_strCommentname!=NULL)cip.DrawText(0,15,m_strCommentname);
	Imp::SafeDelete(pImage, m_strFilename);
}

/*
void h1::each::calc(hypermatrixn& c, const hypermatrixn& a) const
{
	// matrix의 각 row vector들을 aggregate한다. (결과 vector dim은 rows())
	c.setSize(a.page(), a.rows(), a.cols());
	for(int i=0; i<a.page(); i++)
		m_cOperator.calc(c[i], a[i]);
}*/

/*void v0::colon::calcInt(intvectorn& c) const
{
	c.colon(m_nStart, m_nEnd, m_nStepSize);
}*/



void m::multA_diagB(matrixn& c, matrixn const& a, vectorn const& b)
{
	c.setSameSize(a);
	for(int i=0; i<c.cols(); i++)
		c.column(i).mult(a.column(i), b[i]);
}


double m::vMv(vectorn const& v, matrixn const& M)
{
	/* 깔끔한 버젼
	static vectorn vM;
	vM.multmat(v, M);
	return vM%v;*/

	// 빠른 구현.
	int b1,b2;
    double diff1,diff2;
    double sum;

    sum = 0;
	int nbands=v.size();
    for(b1=0; b1<nbands; b1++) 
    for(b2=0; b2<nbands; b2++)
    {
      diff1 = v[b1];
      diff2 = v[b2];
      sum += diff1*diff2*M[b1][b2];
    }
	return sum;
}

double m::sMs(vectorn const& a, vectorn const& b, matrixn const& M)
{
	/* 깔끔한 버젼
	static vectorn s;
	s.sub(a,b);
	return m::vMv(s,M);*/

	// 빠른 구현. (속도 차이 많이 남)
	int b1,b2;
    double diff1,diff2;
    double sum;

    sum = 0;
	int nbands=a.size();
    for(b1=0; b1<nbands; b1++) 
    for(b2=0; b2<nbands; b2++)
    {
      diff1 = a[b1]-b[b1];
      diff2 = a[b2]-b[b2];
      sum += diff1*diff2*M[b1][b2];
    }
	return sum;

}

double m::ss(vectorn const& a, vectorn const& b)
{
	double ss=0;
	for(int i=0; i<a.size(); i++)
	{
		ss+=SQR(a[i]-b[i]);
	}
	return ss;
}
double m::vDv(vectorn const& v, vectorn const& diagM)
{
    int b1;
    double diff1,diff2;
    double sum;

    sum = 0;
	int nbands=v.size();
    for(b1=0; b1<nbands; b1++) 
    {
      diff1 = v[b1];      
      sum += diff1*diff1*diagM[b1];
    }
	return sum;
}

double m::sDs(vectorn const& a, vectorn const& b, vectorn const& diagM)
{
    int b1;
    double diff1,diff2;
    double sum;

    sum = 0;
	int nbands=a.size();
    for(b1=0; b1<nbands; b1++) 
    {
      diff1 = a[b1]-b[b1];
      
      sum += diff1*diff1*diagM[b1];
    }

	return sum;
}

void m1::inverseDiagonal ::calc(matrixn& c, matrixn const& a) const
{
	c.setSameSize(a);
	c.setAllValue(0.0);
	for(int i=0; i<a.rows(); i++)
		c[i][i]=1.0/a[i][i];
}

void m1::add::calc(matrixn& c, matrixn const& a) const
{
	for(int i=0; i<c.rows(); i++)
		for(int j=0; j<c.cols(); j++)
			c[i][j]+=a[i][j];
}
void m1::multAdd::calc(matrixn& c, matrixn const& a) const
{
	for(int i=0; i<c.rows(); i++)
		for(int j=0; j<c.cols(); j++)
			c[i][j]+=a[i][j]*mMult;
}

void m2::multAtBA::calc(matrixn& c, matrixn const& a, matrixn const& b) const
{
	c.multAtB(a, b*a);
}

void m2::multABAt::calc(matrixn& c, matrixn const& a, matrixn const& b) const
{
	c.multABt(a*b, a);
}

void m2::multABt::calc(matrixn& c, matrixn const& a, matrixn const& b) const
{
	if(&a==&c)
	{
		matrixn aa(a);
		calc(c, aa,b);
	}

	if(&b==&c)
	{
		matrixn bb(b);
		calc(c, a,bb);
	}

    assert( a.cols()==b.cols() );
    c.setSize( a.rows(), b.rows() );

    for( int i=0; i<a.rows(); i++ )
    for( int j=0; j<b.rows(); j++ )
    {
        c[i][j] = 0;
        for( int k=0; k<a.cols(); k++ )
            c[i][j] += a[i][k] * b[j][k];
    }
    
}


void s1::COS(double&b,double a)  {b= (double)cos(a);}
void s1::SIN(double&b,double a)  {b= (double)sin(a);}
void s1::EXP(double&b,double a)  {b= (double)exp(a);}
void s1::NEG(double&b,double a)  {b= -1*a;}
void s1::SQRT(double&b,double a)  { b= sqrt(a);}
void s1::SQUARE(double&b,double a)  { b= a*a;}
void s1::ASSIGN(double&b,double a)  { b= a;}
void s1::LOG(double&b,double a)  { b= log(a);}
void s1::abs(double&b,double a)  { b= (double)ABS(a);}
void s1::SMOOTH_TRANSITION(double&b,double a)  { b= ((double)-2.0)*a*a*a+((double)3.0)*a*a;} // y=-2x^3+3x^2
void s1::RADD(double&b,double a)  { b+=a;}
void s1::RDIV(double&b,double a)  { b/=a;}
void s1::RSUB(double&b,double a)  { b-=a;}
void s1::RMULT(double&b,double a)  { b*=a;}
void s1::BOUND(double&b, double a)  { b=CLAMP(b, -1*a, a);}
void s1::INVERSE(double&b, double a) { b=1.0/a;	}


double s2::ADD(double a, double b)  {return a+b;}
double s2::SUB(double a, double b)  {return a-b;}
double s2::MULT(double a, double b)  {return a*b;}
double s2::DIV(double a, double b)  {return a/b;}
double s2::POW(double a, double b)  {return pow(a,b);}
double s2::MINIMUM(double a, double b)  {return MIN(a,b);}
double s2::MAXIMUM(double a, double b)  {return MAX(a,b);}
double s2::GREATER(double a, double b)  { return (double) a>b;}
double s2::GREATER_EQUAL(double a, double b)  { return (double) a>=b;}
double s2::SMALLER(double a, double b)  { return (double) a<b;}
double s2::SMALLER_EQUAL(double a, double b)  { return (double) a<=b;}
double s2::EQUAL(double a, double b)  { return (double) a==b;}
double s2::AVG(double a, double b)  { return (a+b)/(double)2.0;}
double s2::BOUND(double a, double b)  { return CLAMP(a, -1*b, b);}
int s2::INT_NOT_EQUAL(int a, int b)		{ return a!=b;}
int s2::INT_EQUAL(int a, int b)		{ return a==b;}

