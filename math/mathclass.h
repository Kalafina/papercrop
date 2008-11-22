//
// mathclass.H 
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

#include <stdio.h>
#include <tchar.h>

#include <math.h>
#include <assert.h>

#include <limits>

#ifndef	M_PI
#define	M_PI	3.14159265358979323846
#endif


#define EPS (1.0e-10)

#include <LIMITS.H>
#include <FLOAT.H>
#include "math_macro.h"




// scalar binary operators
namespace s2
{
	double ADD(double a, double b) ;
	double SUB(double a, double b) ;
	double MULT(double a, double b);
	double DIV(double a, double b);
	double POW(double a, double b);
	double MINIMUM(double a, double b);
	double MAXIMUM(double a, double b);
	double GREATER(double a, double b);
	double GREATER_EQUAL(double a, double b);
	double SMALLER(double a, double b);
	double SMALLER_EQUAL(double a, double b);
	double EQUAL(double a, double b);
	double AVG(double a, double b);
	double BOUND(double a, double b);
	int INT_NOT_EQUAL(int a, int b);
	int INT_EQUAL(int a, int b);
}

namespace s1
{
	// (scalar->scalar¿¬»ê)	
	void COS(double&b,double a);
	void SIN(double&b,double a);
	void EXP(double&b,double a);
	void NEG(double&b,double a); 
	void SQRT(double&b,double a);
	void SQUARE(double&b,double a);
	void ASSIGN(double&b,double a);
	void LOG(double&b,double a);
	void abs(double&b,double a);
	void SMOOTH_TRANSITION(double&b,double a);
	void RADD(double&b,double a);
	void RDIV(double&b,double a);
	void RSUB(double&b,double a);
	void RMULT(double&b,double a);
	void BOUND(double&b, double a);
	void INVERSE(double&b, double a);
}

//#include "complex.h"
//#include "cmplxvectorn.h"

#include "interval.h"
//#include "intervalN.h"

#include "vector_n.h"
//#include "bitvectorn.h"
#include "matrix_n.h"
#include "quater.h"
#include "vector.h"
#include "matrix.h"
//#include "smatrixn.h"// include only when needed.
//#include "optimize.h"// include only when needed.
//#include "operator.h"	// include only when needed.
//#include "quaterN.h" // include only when needed.
//#include "vector3N.h"// include only when needed.
//#include "BSpline.h" // include only when needed.
