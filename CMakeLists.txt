# -*- mode: cmake; -*-
cmake_minimum_required(VERSION 2.6)

project (paperCrop)

include(${paperCrop_SOURCE_DIR}/modules.cmake)


if(UNIX)
	if(CMAKE_BUILD_TYPE MATCHES Release)
		add_definitions(-DNDEBUG)
	endif()

	set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS_DEBUG_INIT} -ftemplate-depth-100")
	set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT} -ftemplate-depth-100")

	message("Unix OS")
else()
	add_definitions(-D_SCL_SECURE_NO_DEPRECATE -D__WIN32__ -DHPDF_DLL)

	message("Windows OS")
endif()


if(UNIX)
	set(LUA_INCLUDE "${paperCrop_SOURCE_DIR}/../Lua5.1.4/include")
	set(LUA_LIB "${paperCrop_SOURCE_DIR}/../Lua5.1.4/include/liblua.a")
	set(boost_INCLUDE "${paperCrop_SOURCE_DIR}/../Boost_1_44_0")
	set(image_INCLUDE
		) # all in /usr/include 
else()
	set(LUA_INCLUDE "${paperCrop_SOURCE_DIR}/../Lua5.1.4/include")
	set(LUA_LIB "${paperCrop_SOURCE_DIR}/../lua5.1.4/lua5.1.lib")
	set(boost_INCLUDE "${paperCrop_SOURCE_DIR}/../boost_1_34_1")
	set(image_INCLUDE 
		"${paperCrop_SOURCE_DIR}/devil/include"
		"${paperCrop_SOURCE_DIR}/FreeImage/Dist"
	)

endif()

add_subdirectory(luabind-0.9)
#add_subdirectory(poppler)

include_directories (
	#${LUA_INCLUDE}
	${boost_INCLUDE}
	${paperCrop_SOURCE_DIR}/../lua-5.1.4/src
	#${paperCrop_SOURCE_DIR}/../Boost_1_44_0
	${paperCrop_SOURCE_DIR}
	${paperCrop_SOURCE_DIR}/../FLTK
	${paperCrop_SOURCE_DIR}/../FLTK/Fl_Native_File_Chooser-0.84
	${paperCrop_SOURCE_DIR}/libharu-2.0.8/include
	${paperCrop_SOURCE_DIR}/luabind-0.9
	${paperCrop_SOURCE_DIR}/poppler
	${paperCrop_SOURCE_DIR}/poppler/goo
	${paperCrop_SOURCE_DIR}/ext/jpeg
	${paperCrop_SOURCE_DIR}/ext/zlib
	${paperCrop_SOURCE_DIR}/ext/freetype231/include
	${paperCrop_SOURCE_DIR}/poppler/poppler
	${paperCrop_SOURCE_DIR}/poppler/splash
	
	)
if(UNIX)
	include_directories(
		./include 
		.
		./unix
		)
else()
	include_directories(${paperCrop_SOURCE_DIR}/../fltk)
endif()

if(UNIX)
link_directories(

	${paperCrop_SOURCE_DIR}/../FLTK/lib
	${paperCrop_SOURCE_DIR}/libharu-2.0.8
	${paperCrop_SOURCE_DIR}/devil/lib
	${paperCrop_SOURCE_DIR}/../FreeImage/Dist
	${paperCrop_SOURCE_DIR}/poppler/poppler/ 
	${paperCrop_SOURCE_DIR}/../Freetype/objs/.libs 
	${paperCrop_SOURCE_DIR}/luabind-0.9/Debug
	${paperCrop_SOURCE_DIR}/../lua-5.1.4/src
)
endif()

#${paperCrop_SOURCE_DIR}
#	if(UNIX)
#	"./fltk/Fl_Native_File_Chooser-0.84/Fl_Native_File_Chooser.cxx"
#	else()	
#	"./fltk/Fl_Native_File_Chooser-0.84/Fl_Native_File_Chooser_WIN32.cxx"
#	"./fltk/Fl_Native_File_Chooser-0.84/FL/Fl_Native_File_Chooser_WIN32.H"
#	end()

set(src
	"./GlobalUI.cpp"
	"./GlobalUI.h"
	"./ImageSegmentation.cpp"
	"./ImageSegmentation.h"
	"./PaperCrop.cpp"
	"./PaperCrop.h"
	"./PDFwin.cpp"
	"./PDFwin.h"
	"./PDFWriter.cpp"
	"./PDFWriter.h"
	#"./poppler.cpp"
	"./RightPanel.cpp"
	"./RightPanel.h"
	"./stdafx.cpp"
	"./stdafx.h"
	"./WrapperLua/BaselibLUA.cpp"
	"./WrapperLua/BaselibLUA.h"
	"./WrapperLua/BaselibLUA2.cpp"
	"./WrapperLua/BaselibLUA2.h"
	"./WrapperLua/BaselibLUA3.cpp"
	"./WrapperLua/BaselibLUA3.h"
	"./WrapperLua/MainlibLUA.cpp"
	"./WrapperLua/MainlibLUA.h"
	"./math/bitVectorN.cpp"
	"./math/bitVectorN.h"
	"./math/interval.cpp"
	"./math/interval.h"
	"./math/intervals.cpp"
	"./math/intervals.h"
	"./math/math_macro.h"
	"./math/mathclass.h"
	"./math/matrix.cpp"
	"./math/matrix.h"
	"./math/matrix3.cpp"
	"./math/matrix3.h"
	"./math/matrix_n.cpp"
	"./math/matrix_n.h"
	"./math/Operator.cpp"
	"./math/Operator.h"
	"./math/operatorTemplate.hpp"
	"./math/quater.cpp"
	"./math/quater.h"
	"./math/template_math.h"
	"./math/template_matrix.h"
	"./math/tvector.cpp"
	"./math/tvector.h"
	"./math/transf.cpp"
	"./math/transf.h"
	"./math/vector.cpp"
	"./math/vector.h"
	"./math/vector3N.cpp"
	"./math/vector3N.h"
	"./math/quaterN.cpp"
	"./math/quaterN.h"
	"./math/vector_n.cpp"
	"./math/vector_n.h"
	"./image/Image.cpp"
	"./image/Image.h"
	"./image/ImagePixel.cpp"
	"./image/ImagePixel.h"
	"./image/ImageProcessor.cpp"
	"./image/ImageProcessor.h"
	"./fltk/Fl_Native_File_Chooser-0.84/FL/Fl_Native_File_Chooser.H"
	"./fltk/Fl_Native_File_Chooser-0.84/Fl_Native_File_Chooser.cxx"
##	"./fltk/Fl_Native_File_Chooser-0.84/Fl_Native_File_Chooser_WIN32.cxx"
##	"./fltk/Fl_Native_File_Chooser-0.84/FL/Fl_Native_File_Chooser_WIN32.H"
	"./utility/FlLayout.cpp"
	"./utility/FlLayout.h"
	"./utility/FltkAddon.cpp"
	"./utility/FltkAddon.h"
	"./utility/operatorString.cpp"
	"./utility/operatorString.h"
	"./utility/TypeString.cpp"
	"./utility/TypeString.h"
	"./utility/TextFile.cpp"
	"./utility/TextFile.h"
	)




add_executable(paperCrop
	${src}
	)

if(UNIX)

	target_link_libraries(paperCrop
	
	fltk_images
	fltk
	hpdf
	X11
	ILU
	poppler
	freeimage
	z
	jpeg
	freetype
	luabind-0.9.1
	lua
	fontconfig
	)

endif()


IF(MSVC)
	target_link_libraries(paperCrop
		${LUA_LIB}
		LuaBind
		PopplerLib
		debug ${paperCrop_SOURCE_DIR}/ext/freetype231/freetype231mt_d.lib 
		debug ${paperCrop_SOURCE_DIR}/ext/zlib/zlib_ds.lib 
		debug ${paperCrop_SOURCE_DIR}/ext/jpeg/jpeg_ds.lib 
		debug ${paperCrop_SOURCE_DIR}/../fltk/test/fltkdlld.lib
		debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkimagesd.lib
		debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkpngd.lib
		debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkzd.lib
		debug ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkgld.lib
		optimized ${paperCrop_SOURCE_DIR}/ext/freetype231/freetype231mt.lib 
		optimized ${paperCrop_SOURCE_DIR}/ext/zlib/zlib_s.lib 
		optimized ${paperCrop_SOURCE_DIR}/ext/jpeg/jpeg_s.lib 
		optimized ${paperCrop_SOURCE_DIR}/../fltk/test/fltkdll.lib
		optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkimages.lib
		optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkpng.lib
		optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkz.lib
		optimized ${paperCrop_SOURCE_DIR}/../fltk/lib/fltkgl.lib
		${paperCrop_SOURCE_DIR}/devil/lib/DevIl.lib
		${paperCrop_SOURCE_DIR}/devil/lib/Ilu.lib
		${paperCrop_SOURCE_DIR}/FreeImage/Dist/FreeImage.lib
		${paperCrop_SOURCE_DIR}/libharu-2.0.8/libhpdf.lib
		comctl32.lib 
		winspool.lib 
		Msimg32.lib 
		wininet.lib
		)
endif()
