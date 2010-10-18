#include "stdafx.h"
#include "WrapperLua/BaselibLUA.h"
#include "GlobalUI.h"
#include "utility/FlLayout.h"

#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/error.hpp>


#define CATCH_LUABIND_ERROR(x) catch(luabind::error& e)\
{\
	printf("lua error %s, %s \n", x, e.what());\
	int n=lua_gettop(e.state());\
	Msg::msgBox("lua error %s", lua_tostring(e.state(), n));\
	ASSERT(0);\
	delete _L; _L=NULL;\
}

#ifndef WIN32
#include <unistd.h>
#endif

void GlobalUI::work(const char* _wn, luabind::adl::object const& table)
{
	TString wn=_wn;
#ifndef WIN32
	if(wn=="chdir"){
		TString temp=luabind::object_cast<const char*>(table);
		chdir(temp.ptr());
	}
	else if(wn=="printcwd"){
		char buf[1000];
		getcwd(buf,1000);
		printf("cwd:%s\n", buf);
	}
#endif

}

void addMainlibModule(lua_State* L);
GlobalUI::GlobalUI(FlLayout* win,int argc, char* argv[])
{

	_param.resize(argc-1);

	for(int i=1; i<argc; i++)
		_param[i-1]=argv[i];


	_L=NULL;
	if(argc>1)
	{
		_L=new LUAwrapper();

		addMainlibModule(_L->L);

		luabind::module(_L->L)
			[
			luabind::class_<GlobalUI>("GlobalUI")
			.def("__call", &GlobalUI::work)
			];
		_L->setRef<GlobalUI>("GUI", *this);
		_L->setRef<FlLayout>("wins", *win);
		_L->setRef<TStrings>("param", _param);
		try{
			_L->dofile(_param[0]);
		}
		CATCH_LUABIND_ERROR("ctor")
	}

}

GlobalUI::~GlobalUI()
{
	if(_L) delete _L;
	_L=NULL;
}

