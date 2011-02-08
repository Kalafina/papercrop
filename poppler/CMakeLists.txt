project(poppler)

cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

#set (CMAKE_VERBOSE_MAKEFILE  ON)

project (poppler)



include_directories(
	${poppler_SOURCE_DIR}/goo
	${poppler_SOURCE_DIR}/poppler
	${poppler_SOURCE_DIR}/splash	
	${poppler_SOURCE_DIR}/
	/usr/include/freetype2
	/usr/include/libpng12
	)

  add_definitions(

-DENABLE_LIBJPEG=1
-DENABLE_LIBPNG=1
-DHAVE_DIRENT_H=1
-DHAVE_DLFCN_H=1
-DHAVE_FREETYPE_H=1
-DHAVE_FSEEKO=1
-DHAVE_GETTIMEOFDAY=1
-DHAVE_GMTIME_R=1
-DHAVE_INTTYPES_H=1
-DHAVE_LOCALTIME_R=1
-DHAVE_MEMORY_H=1
-DHAVE_MKSTEMP=1
-DHAVE_MKSTEMPS=1
-DHAVE_POPEN=1
-DHAVE_PTHREAD=1
-DHAVE_SPLASH=1
-DHAVE_STDINT_H=1
-DHAVE_STDLIB_H=1
-DHAVE_STRINGS_H=1
-DHAVE_STRING_H=1
-DHAVE_SYS_STAT_H=1
-DHAVE_SYS_TYPES_H=1
-DHAVE_UNISTD_H=1
-DLT_OBJDIR=".libs/"
-DMULTITHREADED=1
-DOPI_SUPPORT=1
-DPACKAGE="poppler"
-DPACKAGE_BUGREPORT="https://bugs.freedesktop.org/enter_bug.cgi?product=poppler"
-DPACKAGE_NAME="poppler"
-DPACKAGE_STRING="poppler 0.14.4"
-DPACKAGE_TARNAME="poppler"
-DPACKAGE_URL=""
-DPACKAGE_VERSION="0.14.4"
-DPOPPLER_DATADIR="/usr/local/share/poppler"
-DSTDC_HEADERS=1
-DTEXTOUT_WORD_LIST=1
-DUSE_CMS=1
-DVERSION="0.14.4"
-DWITH_FONTCONFIGURATION_FONTCONFIG=1
-D_FILE_OFFSET_BITS=64
)
if(UNIX)
 add_definitions(-Wno-write-strings -Woverloaded-virtual -Wnon-virtual-dtor)
endif()
set(poppler_SRCS


goo/FixedPoint.cc
goo/gfile.cc
goo/gmem.cc
goo/gmempp.cc
goo/GooHash.cc
goo/GooList.cc
goo/GooString.cc
goo/GooTimer.cc
goo/gstrtod.cc
goo/ImgWriter.cc
goo/JpegWriter.cc
goo/PNGWriter.cc


fofi/FoFiBase.cc
fofi/FoFiEncodings.cc
fofi/FoFiTrueType.cc
fofi/FoFiType1.cc
fofi/FoFiType1C.cc


     splash/SplashBitmap.cc
     splash/Splash.cc
     splash/SplashClip.cc
     splash/SplashFont.cc
     splash/SplashFontEngine.cc
     splash/SplashFontFile.cc
     splash/SplashFontFileID.cc
     splash/SplashFTFont.cc
     splash/SplashFTFontEngine.cc
     splash/SplashFTFontFile.cc
     splash/SplashPath.cc
     splash/SplashPattern.cc
     splash/SplashScreen.cc
     splash/SplashState.cc
     splash/SplashT1Font.cc
     splash/SplashT1FontEngine.cc
     splash/SplashT1FontFile.cc
     splash/SplashXPath.cc
     splash/SplashXPathScanner.cc



    poppler/SplashOutputDev.cc
    poppler/DCTStream.cc
    poppler/JPXStream.cc
    poppler/Annot.cc
    poppler/Array.cc
    poppler/BuiltinFont.cc
    poppler/BuiltinFontTables.cc
    poppler/CachedFile.cc
    poppler/Catalog.cc
    poppler/CharCodeToUnicode.cc
    poppler/CMap.cc
    poppler/DateInfo.cc
    poppler/Decrypt.cc
    poppler/Dict.cc
    poppler/Error.cc
    poppler/FileSpec.cc
    poppler/FontEncodingTables.cc
    poppler/Form.cc
    poppler/FontInfo.cc
    poppler/Function.cc
    poppler/Gfx.cc
    poppler/GfxFont.cc
    poppler/GfxState.cc
    poppler/GlobalParams.cc
    poppler/JArithmeticDecoder.cc
    poppler/JBIG2Stream.cc
    poppler/Lexer.cc
    poppler/Link.cc
    poppler/LocalPDFDocBuilder.cc
    poppler/Movie.cc
    poppler/NameToCharCode.cc
    poppler/Object.cc
    poppler/OptionalContent.cc
    poppler/Outline.cc
    poppler/OutputDev.cc
    poppler/Page.cc
    poppler/PageTransition.cc
    poppler/Parser.cc
    poppler/PDFDoc.cc
    poppler/PDFDocEncoding.cc
    poppler/PDFDocFactory.cc
    poppler/PopplerCache.cc
    poppler/ProfileData.cc
    poppler/PreScanOutputDev.cc
    poppler/PSTokenizer.cc
    poppler/Rendition.cc
    poppler/StdinCachedFile.cc
    poppler/StdinPDFDocBuilder.cc
    poppler/Stream.cc
    poppler/strtok_r.cpp
    poppler/UnicodeMap.cc
    poppler/UnicodeTypeTable.cc
    poppler/XRef.cc
    poppler/PSOutputDev.cc
    poppler/TextOutputDev.cc
    poppler/PageLabelInfo.cc
    poppler/SecurityHandler.cc
    poppler/Sound.cc
    poppler/XpdfPluginAPI.cc


)


add_library (poppler STATIC ${poppler_SRCS})
