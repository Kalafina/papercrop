#include "stdafx.h"
#include "BaselibLUA.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	//#include "luadebug.h"
}


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

void addBaselibModule(lua_State* L);
LUAwrapper::LUAwrapper()
{
	L = lua_open();
	luabind::open(L);
	luaopen_base(L );
	luaopen_io(L );
	luaopen_string(L );
	luaopen_math(L );
	luaopen_debug(L );
	luaopen_table(L );
	addBaselibModule(L);
}

LUAwrapper::~LUAwrapper()
{
	lua_setgcthreshold(L, 0);  // collected garbage
	lua_close(L);
}

void LUAwrapper::dofile(const char* pFilename )
{
	/*	- simple way without error_checking
	lua_dofile(L, pFilename );*/

	if (0 != luaL_loadfile(L, pFilename))
	{
		TString errorMsg;
		errorMsg.format("Lua Error - Script Load\nScript Name:%s\nError Message:%s\n", pFilename, luaL_checkstring(L, -1));
		throw std::runtime_error(errorMsg.ptr());
	}
		
	if (0 != lua_pcall(L, 0, LUA_MULTRET, 0))
	{
		TString errorMsg;
		errorMsg.format("Lua Error - Script run\nScript Name:%s\nError Message:%s\n", pFilename, luaL_checkstring(L, -1));
		throw std::runtime_error(errorMsg.ptr());
	}
}

void LUAwrapper::dostring(const char* script)
{
	/* -simple way without error_checking
	lua_dostring(L, script);*/

	if (0 != luaL_loadbuffer(L, script, strlen(script), NULL))
	{
		TString errorMsg;
		errorMsg.format("Lua Error - String Load\nString:%s\nError Message:%s\n", script, luaL_checkstring(L, -1));
		throw std::runtime_error(errorMsg.ptr());
	}

	if (0 != lua_pcall(L, 0, LUA_MULTRET, 0))
	{
		TString errorMsg;
		errorMsg.format("Lua Error - String run\nString:%s\nError Message:%s\n", script, luaL_checkstring(L, -1));
		throw std::runtime_error(errorMsg.ptr());
	}

}

int LUAwrapper::arraySize(luabind::object const& ll)
{
	if (luabind::type(ll) != LUA_TTABLE)
		throw std::range_error("arraySize called for non-table object");

	int count=0;
	for (luabind::iterator i(ll), end; i != end; ++i)
		count++;
	return count;
}

int LUAwrapper::treeSize(luabind::object const& ll)
{
	// a={{"a","b"}, "c"} -> size=5 ( Root, firstInternalNode, "a", "b", "c")
	if (luabind::type(ll) != LUA_TTABLE)
	{
//		printf("%s\n", luabind::object_cast<const char*>(ll));
		return 1;		
	}

	int count=0;
	for(luabind::iterator i(ll), end; i!=end; ++i)
	{
		count+=treeSize(*i);
	}

	return count+1;	// 한개의 internal node가 필요하다.
}