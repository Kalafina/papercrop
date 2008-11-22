#pragma once

#include <limits>

namespace sv
{
	// usage : m_real sum=sv::minimum(a);
	template <class VEC_TYPE, class T>
	T minimum(VEC_TYPE const& a);

	template <class VEC_TYPE, class T>
	T maximum(VEC_TYPE const& a);	
}

namespace s2
{
	struct mid
	{
		template <class T>
		T operator()(T a, T b)
		{
			return a*0.5+b*0.5;
		}
	};	

	struct sub
	{
		template <class T>
		T operator() (T a, T b)
		{
			return a-b;
		}
	};
}

namespace v
{
	// std::for_each의 사용법과 유사(std의 func를 사용할 수 있음.) 
	// ex: v::for_each(a.range(0,3), s0::SQR)

	// 없으면 -1 리턴. if(b(a(i))) return i;
	template <class VEC_TYPE, class S1>
	int findFirstIndex(VEC_TYPE const& a, S1 & b);
	
	template <class VEC_TYPE, class S1>
	int findLastIndex(VEC_TYPE const& a, S1& b);

	// _Func(c[i])
	template <class VEC_TYPE, class S0>
	S0 for_each(VEC_TYPE & a, S0 _Func);

	// c[i]=_Func(a[i])
	template <class VEC_TYPE, class S1>
	S1 for_each(VEC_TYPE & c, VEC_TYPE const& a, S1 _Func);

	// c[i]=_Func(a[i], b[i])
	template <class VEC_TYPE, class V2>
	V2 for_each(VEC_TYPE & c, VEC_TYPE const& a, VEC_TYPE const& b, V2 _Func);

	template <class VEC_TYPE, class MAT_TYPE, class SV>
	void for_each_column(VEC_TYPE & c, MAT_TYPE const & m, SV func);

	// _Func(c[i], a[i])
	template <class VEC_TYPE, class S1_inout>
	S1_inout for_each1(VEC_TYPE & c, VEC_TYPE const& a, S1_inout _Func);


	// m_real S1(T a) returns a cost.
	// e.g. vector3N a; int argNearest=v::argMin(a, s0::distanceTo(vector3(0,0,0)));
	template <class VEC_TYPE, class S1>
	int argMin(VEC_TYPE& a, S1 _Func, int start=0, int end=INT_MAX);

}


namespace m
{
	// template functions shared for various matrix classes (such as blitz matrix, matrixn, matrixnTransposedView..)
	// usage: matrixn c, matrixn A; m::mult(c, A, A.transpose()); -> C=A*A^T; without mem-copies.
	// However, due to memory random access. This function is typically slower than a simple copy and mult scheme. 
	template <class MAT1, class MAT2> 
	void mult( matrixn& c, MAT1 const& a, MAT2 const& b )
	{
		assert( a.cols()==b.rows() );
		c.setSize( a.rows(), b.cols() );

		for( int i=0; i<a.rows(); i++ )
		for( int j=0; j<b.cols(); j++ )
		{
			c(i,j) = 0;
			for( int k=0; k<a.cols(); k++ )
				c(i,j) += a(i,k)*b(k,j);
		}
	}
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// implementations ( You don't need to read the code below.
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace sv
{
	template <class VEC_TYPE, class T>
	T minimum(VEC_TYPE const& a)
	{
		T minv=std::numeric_limits<T>::max();

		for(int i=0; i<a.size(); i++)
		{
			if(a[i]<minv)
				minv=a[i];
		}

		return minv;
	}

	template <class VEC_TYPE, class T>
	T maximum(VEC_TYPE const& a)
	{
		T max=-1*std::numeric_limits<T>::max();

		for(int i=0; i<a.size(); i++)
		{
			if(a[i]>max)
				max=a[i];
		}

		return max;
	}
}

namespace v
{
	template <class VEC_TYPE, class MAT_TYPE, class SV>
	void for_each_column(VEC_TYPE & c, MAT_TYPE const & m, SV func)
	{
		c.setSize(m.cols());


		for(int i=0; i<m.cols(); i++)
		{
			c[i]=func(m.column(i));
		}
	}

	template <class VEC_TYPE, class Bool>
	int findFirstIndex(VEC_TYPE const& a, Bool b)
	{
		for(int i=0; i<a.size(); i++)
			if(b(a[i])) return i;
		return -1;
	}

	template <class VEC_TYPE, class Bool>
	int findLastIndex(VEC_TYPE const& a, Bool b)
	{
		for(int i=a.size()-1; i>=0; i--)
			if(b(a[i])) return i;
		return -1;
	}

	// std::for_each의 사용법과 유사(std의 func를 사용할 수 있음.) 
	// ex: v::for_each(a.range(0,3), s0::SQR)
	template <class VEC_TYPE, class FUNC_TYPE>
	FUNC_TYPE for_each(VEC_TYPE & a, FUNC_TYPE _Func)
	{
		for(int i=0; i<a.size(); i++)
			_Func(a[i]);
		return _Func;
	}

	template <class VEC_TYPE, class S1>
	int argMin(VEC_TYPE& a, S1 _Func, int start, int end)
	{
		if(end>a.size()) end=a.size();
		
		m_real minV=DBL_MAX;
		int argMinV=start;

		for(int i=start; i<end; i++)
		{
			m_real d=_Func(a[i]);
			if(d<minV)
			{
				minV=d;
				argMinV=i;
			}
		}

		return argMinV;
	};


	template <class VEC_TYPE, class FUNC_TYPE>
	FUNC_TYPE for_each(VEC_TYPE & c, VEC_TYPE const& a, FUNC_TYPE _Func)
	{
		c.setSize(a.size());
		for(int i=0; i<a.size(); i++)
			c[i]=_Func(a[i]);
		return _Func;
	}

	template <class VEC_TYPE, class FUNC_TYPE>
	FUNC_TYPE for_each(VEC_TYPE & c, VEC_TYPE const& a, VEC_TYPE const& b, FUNC_TYPE _Func)
	{
		ASSERT(a.size()==b.size());
		c.setSize(a.size());
		for(int i=0; i<a.size(); i++)
			c[i]=_Func(a[i], b[i]);
		return _Func;
	}

	template <class VEC_TYPE, class FUNC_TYPE>
	FUNC_TYPE for_each1(VEC_TYPE & c, VEC_TYPE const& a, FUNC_TYPE _Func)
	{
		c.setSize(a.size());
		for(int i=0; i<a.size(); i++)
			_Func(c[i], a[i]);
		return _Func;
	}
}
