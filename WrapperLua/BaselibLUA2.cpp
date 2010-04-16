
/* 
luabind로 바꾸는 중.
lua
function f(a) return a.x+a.y end

// C++
luabind::object table = luabind::newtable(L);
table["x"] = 1;
table["y"] = 2;
functor<int> f(L,"f");
f(table);

You can put a table into another table.

// C++
luabind::object table = luabind::newtable(L);
luabind::object subtable = luabind::newtable(L);
table["sub"] = subtable;
functor<int> f(L,"f");
f(table);

*/

#include "stdafx.h"
#include "BaselibLUA.h"


#include <iostream>

#include "image/image.h"
#include "image/imageprocessor.h"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/copy_policy.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/discard_result_policy.hpp>
#include <luabind/dependency_policy.hpp>
#include <luabind/luabind.hpp>

#include "baseliblua2.h"
#include "baseliblua3.h"

using namespace luabind;
void addBaselibModule(lua_State* L)
{

	// assumes that L is already opened and that luabind::open(L) is already called.

	// image
	{
		module(L)[
			class_<CImage>("CImage")
				.def(constructor<>())
				.def("GetWidth", &CImage::GetWidth)
				.def("GetHeight", &CImage::GetHeight)
				.def("Load", &CImage::Load)
				.def("Save", &CImage::Save)
				.def("create", &CImage::Create)
				.def("CopyFrom", &CImage::CopyFrom)
				.def("concatVertical", &Imp::concatVertical)
				.def("rotateRight", &Imp::rotateRight)
				.def("rotateLeft", &Imp::rotateLeft)
				.def("sharpen", &Imp::sharpen)
				.def("contrast", &Imp::contrast)
				.def("dither", &Imp::dither)
				.def("gammaCorrect", &Imp::gammaCorrect)
				.def("gamma", &Imp::gammaCorrect)
				.def("crop", &Imp::crop)
				.def("resize", &Imp::resize)
				.def("blit", &Imp::blit)
				.def("drawBox", &Imp::drawBox),
				class_<TRect>("TRect")
				.def(constructor<>())
				.def(constructor<int,int,int,int>())
				.def_readwrite("left", &TRect::left)
				.def_readwrite("right", &TRect::right)
				.def_readwrite("top", &TRect::top)
				.def_readwrite("bottom", &TRect::bottom)
				
		];
	}

	// vector3
	{
		void (vector3::*add1)(const vector3&, const vector3&) =&vector3::add;
		void (vector3::*sub1)(const vector3&, const vector3&) =&vector3::sub;
		void (vector3::*cross1)(const vector3&, const vector3&) =&vector3::cross;
		void (vector3::*add2)(const vector3&) =&vector3::add;
		void (vector3::*sub2)(const vector3&) =&vector3::sub;

		struct vector3_wrap
		{
			static void assign(vector3& l, object const& ll) 
			{
				if (type(ll) != LUA_TTABLE)
					throw std::range_error("vector3_assign");
				l.x=object_cast<double>(ll[1]);	// lua indexing starts from 1.
				l.y=object_cast<double>(ll[2]);
				l.z=object_cast<double>(ll[3]);
			}
			static std::string out(vector3& v)
			{
				return std::string(v.output());
			}

		};

		module(L)[
			class_<vector3>("vector3")
				.def(constructor<>())
				.def(constructor<double, double, double>())
				.def_readwrite("x", &vector3::x)
				.def_readwrite("y", &vector3::y)
				.def_readwrite("z", &vector3::z)
				.def("add", add1)
				.def("sub", sub1)
				.def("radd", add2)
				.def("rsub", sub2)
				.def("cross", cross1)
				.def("multadd", &vector3::multadd)
				.def("length", &vector3::length)
				.def(-self) // neg (unary minus)
				.def(self + self) // add (homogeneous)
				.def(self * self) // mul
				.def("assign", &vector3_wrap::assign)
				.def("set", &vector3_wrap::assign)
				.def("interpolate", &vector3::interpolate)
				.def("difference", &vector3::difference)
				.def("rotationVector", &vector3::rotationVector)
				.def("__tostring", &vector3_wrap::out)
		];
	}


	// TStrings
	{

		struct TStrings_wrap
		{
			static void assign(TStrings& l, object const& ll) 
			{
				if (type(ll) != LUA_TTABLE)
					throw std::range_error("TStrings_assign");

				l.resize(0);

				for(luabind::iterator i(ll), end; i!=end; ++i)
				{
					l.pushBack(luabind::object_cast<const char*>(*i));
				}
			}
			static std::string out(TStrings& v, int i)
			{
				return std::string(v[i]);
			}

			static void in(TStrings& v, int i, std::string& a)
			{
				v[i]=a.c_str();
			}

		};

		module(L)[
			class_<TStrings>("TStrings")
				.def(constructor<>())
				.def("assign", &TStrings_wrap::assign)
				.def("data", &TStrings_wrap::out)
				.def("setData", &TStrings_wrap::in)
		];
	}

	// quater
	{	
		struct quater_wrap
		{
			static void assign(quater& l, object const& ll) 
			{
				if (type(ll) != LUA_TTABLE)
					throw std::range_error("quater_assign");
				l.w=object_cast<double>(ll[1]);	// lua indexing starts from 1.
				l.x=object_cast<double>(ll[2]);	// lua indexing starts from 1.
				l.y=object_cast<double>(ll[3]);
				l.z=object_cast<double>(ll[4]);
			}
			static std::string out(quater& v)
			{
				return std::string(v.output());
			}

			static double toRadian(double deg)
			{
				return TO_RADIAN(deg);
			}

			static double toDegree(double rad)
			{
				return TO_DEGREE(rad);
			}

		};


		double (quater::*rotationAngle1)(void) const=&quater::rotationAngle;
		void (quater::*normalize1)()=&quater::normalize;

		module(L)[
			def("toRadian",&quater_wrap::toRadian),
				def("toDegree",&quater_wrap::toDegree),
				class_<quater>("quater")
				.def(constructor<>())
				.def(constructor<double, double, double, double>())
				.def_readwrite("x", &quater::x)
				.def_readwrite("y", &quater::y)
				.def_readwrite("z", &quater::z)
				.def_readwrite("w", &quater::w)
				.def("slerp", &quater::slerp)
				.def("safeSlerp", &quater::safeSlerp)
				.def("decomposeTwistTimesNoTwist", &quater::decomposeTwistTimesNoTwist)
				.def("decomposeNoTwistTimesTwist", &quater::decomposeNoTwistTimesTwist)
				.def("scale", &quater::scale)
				.def(-self) // neg (unary minus)
				.def(self + self) // add (homogeneous)
				.def(self * self) // mul
				.def("length", &quater::length)
				.def("rotationAngle", rotationAngle1)
				.def("rotationAngleAboutAxis", &quater::rotationAngleAboutAxis)
				.def("assign", &quater_wrap::assign)
				.def("set", &quater_wrap::assign)
				.def("leftMult", &quater::leftMult)
				.def("setRotation", (void (quater::*)(const vector3&, double))&quater::setRotation)
				.def("setRotation", (void (quater::*)(const vector3& ))&quater::setRotation)
				.def("setRotation", (void (quater::*)(const matrix4& a))&quater::setRotation)
				.def("normalize", normalize1)
				.def("align", &quater::align)
				.def("difference", &quater::difference)
				.def("__tostring", &quater_wrap::out)
		];
	}

	// matrix4
	{
		struct wrap_matrix4
		{
			static void assign(matrix4& l, object const& ll) 
			{
				if(LUAwrapper::arraySize(ll)!=16) throw std::range_error("matrix4_assign");
				l._11=object_cast<double>(ll[0]);
				l._12=object_cast<double>(ll[1]);
				l._13=object_cast<double>(ll[2]);
				l._14=object_cast<double>(ll[3]);
				l._21=object_cast<double>(ll[4]);
				l._22=object_cast<double>(ll[5]);
				l._23=object_cast<double>(ll[6]);
				l._24=object_cast<double>(ll[7]);
				l._31=object_cast<double>(ll[8]);
				l._32=object_cast<double>(ll[9]);
				l._33=object_cast<double>(ll[10]);
				l._34=object_cast<double>(ll[11]);
				l._41=object_cast<double>(ll[12]);
				l._42=object_cast<double>(ll[13]);
				l._43=object_cast<double>(ll[14]);
				l._44=object_cast<double>(ll[15]);
			}
			static std::string out(matrix4& l)
			{
				TString v;
				v.format(" %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f\n", l._11,l._12,l._13,l._14,l._21,l._22,l._23,l._24,l._31,l._32,l._33,l._34,l._41,l._42,l._43,l._44);
				return std::string(v.ptr());
			}
		};

		module(L)[		
			class_<matrix4>("matrix4")
				.def_readwrite("_11", &matrix4::_11)
				.def_readwrite("_12", &matrix4::_12)
				.def_readwrite("_13", &matrix4::_13)
				.def_readwrite("_14", &matrix4::_14)
				.def_readwrite("_21", &matrix4::_21)
				.def_readwrite("_22", &matrix4::_22)
				.def_readwrite("_23", &matrix4::_23)
				.def_readwrite("_24", &matrix4::_24)
				.def_readwrite("_31", &matrix4::_31)
				.def_readwrite("_32", &matrix4::_32)
				.def_readwrite("_33", &matrix4::_33)
				.def_readwrite("_34", &matrix4::_34)
				.def_readwrite("_41", &matrix4::_41)
				.def_readwrite("_42", &matrix4::_42)
				.def_readwrite("_43", &matrix4::_43)
				.def_readwrite("_44", &matrix4::_44)
				.def(constructor<>())
				.def("assign", &wrap_matrix4::assign)
				.def("identity", &matrix4::identity)
				.def("__tostring", &wrap_matrix4::out)
				.def("setRotation", (void (matrix4::*)(const quater&))&matrix4::setRotation)
		];

	}


	addBaselibModule2(L);



}
void testLuaBind()
{
	try
	{
		LUAwrapper l;
		lua_dostring(l.L, "function add(first, second)\nprint(first,second)\nreturn 1\nend\n");

		int a=3;
		l.setVal<int>("a", a);
		vectorn b(4, 1.0, 2.0,3.0,4.0);
		l.setVal<vectorn>("b", b);
		b[2]=5;

		l.setRef<vectorn>("b_ref", b);


		struct test_raw
		{
			static void greet(lua_State* L)
			{
				lua_pushstring(L, "hello");
			}
		};

		module(l.L)
			[
				def("greet", &test_raw::greet, raw(_1))
			];

		lua_dostring(l.L, "d=3");

		lua_dostring(l.L, "e=vectorn()\ne:assign({1,1,1,1})");

		lua_dostring(l.L, "e[0]=3\ne[1]=e[1]+1\n");


		int d;
		l.getVal<int>("d", d);
		std::cout << d;

		vectorn e;
		l.getVal<vectorn>("e", e);
		
		
		vectorn& er=l.getRef<vectorn>("e");
		
		er[1]=3;

		std::cout << "Result: "
			<< luabind::call_function<int>(l.L, "add", "asdf", "3333");


		lua_dostring(l.L, "debug.debug()");
    }
    catch(luabind::error& e)
    {
		
		printf("lua error %s\n", e.what());
    }
}


