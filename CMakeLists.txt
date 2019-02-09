
cmake_minimum_required(VERSION 3.2)

####### ADD PREDEFINED DEFINITIONS:
SET(FREEIMAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(LIBPNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/LibPNG/)
SET(ZLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/zlib/)

####### SET POSTFIX:
set(CMAKE_DEBUG_POSTFIX             "d")
set(CMAKE_RELEASE_POSTFIX           "")
set(CMAKE_RELWITHDEBINFO_POSTFIX    "")
set(CMAKE_MINSIZEREL_POSTFIX        "")

####### FREEIMAGE
PROJECT(FreeImage)

####### DEFINES
ADD_DEFINITIONS("-DVER_MAJOR=3")
ADD_DEFINITIONS("-DVER_MINOR=17.0")
ADD_DEFINITIONS("-DFREEIMAGE_LITE")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(EMSCRIPTEN 1)
endif()

IF(WIN32)
    ADD_DEFINITIONS("-DOPJ_STATIC")
    ADD_DEFINITIONS("-DLIBRAW_NODLL")
    ADD_DEFINITIONS("-DFREEIMAGE_LIB=1")
ENDIF()

IF(UNIX)
    IF(APPLE)
        ADD_DEFINITIONS("-DNO_LCMS")
        ADD_DEFINITIONS("-D__ANSI__")
        ADD_DEFINITIONS("-DDISABLE_PERF_MEASUREMENT")
    ELSE()
        ADD_DEFINITIONS("-DFREEIMAGE_LIB=1")
    ENDIF()
ENDIF()

IF(ANDROID)
    ADD_DEFINITIONS("-DPNG_ARM_NEON_OPT=0") # Disable NEON in libPNG which is broken in current version, TODO: Update libPNG
ENDIF()

####### COMPILER FLAGS
set(CMAKE_CXX_STANDARD 11)

IF(NOT MSVC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-sign-compare -Wno-unused-function -Wno-implicit-function-declaration -Wno-pointer-sign \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-sign-compare -Wno-unused-value \
                -Wno-type-limits")

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-sign-compare -Wno-unused-function \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter  \
                -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-unused-value \
                -Wno-type-limits -Wno-reorder")
ENDIF()

IF(APPLE)
    string(REGEX REPLACE "-O3" ""
        CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "-O3" ""
        CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-const-variable -Wno-uninitialized -fexceptions -fvisibility=hidden")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-const-variable -Wno-uninitialized -Wno-header-guard -fexceptions \
                                            -fvisibility=hidden -Wno-ctor-dtor-privacy -stdlib=libc++ -Wc++11-narrowing")
ELSE()
    IF(NOT MSVC AND NOT EMSCRIPTEN)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-old-style-declaration")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-clobbered")
    ENDIF()
ENDIF()

####### INCLUDE PATHS
INCLUDE_DIRECTORIES(${FREEIMAGE_DIR}/Source/)

####### SOURCE CODE
SET(FREEIMAGE_SRC ${CMAKE_CURRENT_SOURCE_DIR}
                            Source/FreeImage/BitmapAccess.cpp
                            Source/FreeImage/ColorLookup.cpp
                            Source/FreeImage/FreeImage.cpp
                            Source/FreeImage/FreeImageIO.cpp
                            Source/FreeImage/GetType.cpp
                            Source/FreeImage/MemoryIO.cpp
                            Source/FreeImage/PixelAccess.cpp
                            Source/FreeImage/Plugin.cpp
                            Source/FreeImage/PluginBMP.cpp
                            Source/FreeImage/PluginGIF.cpp
                            Source/FreeImage/PluginICO.cpp
                            Source/FreeImage/PluginPNG.cpp
                            Source/FreeImage/Conversion.cpp
                            Source/FreeImage/Conversion16_555.cpp
                            Source/FreeImage/Conversion16_565.cpp
                            Source/FreeImage/Conversion24.cpp
                            Source/FreeImage/Conversion32.cpp
                            Source/FreeImage/Conversion4.cpp
                            Source/FreeImage/Conversion8.cpp
                            Source/FreeImage/ConversionFloat.cpp
                            Source/FreeImage/ConversionRGB16.cpp
                            Source/FreeImage/ConversionRGBA16.cpp
                            Source/FreeImage/ConversionRGBAF.cpp
                            Source/FreeImage/ConversionRGBF.cpp
                            Source/FreeImage/ConversionType.cpp
                            Source/FreeImage/ConversionUINT16.cpp
                            Source/FreeImage/Halftoning.cpp
                            Source/FreeImage/tmoColorConvert.cpp
                            Source/FreeImage/tmoDrago03.cpp
                            Source/FreeImage/tmoFattal02.cpp
                            Source/FreeImage/tmoReinhard05.cpp
                            Source/FreeImage/ToneMapping.cpp
                            Source/FreeImage/LFPQuantizer.cpp
                            Source/FreeImage/NNQuantizer.cpp
                            Source/FreeImage/WuQuantizer.cpp
                            Source/DeprecationManager/Deprecated.cpp
                            Source/DeprecationManager/DeprecationMgr.cpp
                            Source/FreeImage/CacheFile.cpp
                            Source/FreeImage/MultiPage.cpp
                            Source/FreeImage/ZLibInterface.cpp
                            Source/Metadata/FIRational.cpp
                            Source/Metadata/FreeImageTag.cpp
                            Source/Metadata/IPTC.cpp
                            Source/Metadata/TagConversion.cpp
                            Source/Metadata/TagLib.cpp
                            Source/FreeImageToolkit/Background.cpp
                            Source/FreeImageToolkit/BSplineRotate.cpp
                            Source/FreeImageToolkit/Channels.cpp
                            Source/FreeImageToolkit/ClassicRotate.cpp
                            Source/FreeImageToolkit/Colors.cpp
                            Source/FreeImageToolkit/CopyPaste.cpp
                            Source/FreeImageToolkit/Display.cpp
                            Source/FreeImageToolkit/Flip.cpp
                            Source/FreeImageToolkit/MultigridPoissonSolver.cpp
                            Source/FreeImageToolkit/Rescale.cpp
                            Source/FreeImageToolkit/Resize.cpp
                            Source/LibPNG/png.c
                            Source/LibPNG/pngerror.c
                            Source/LibPNG/pngget.c
                            Source/LibPNG/pngmem.c
                            Source/LibPNG/pngpread.c
                            Source/LibPNG/pngread.c
                            Source/LibPNG/pngrio.c
                            Source/LibPNG/pngrtran.c
                            Source/LibPNG/pngrutil.c
                            Source/LibPNG/pngset.c
                            Source/LibPNG/pngtrans.c
                            Source/LibPNG/pngwio.c
                            Source/LibPNG/pngwrite.c
                            Source/LibPNG/pngwtran.c
                            Source/LibPNG/pngwutil.c
                            Source/ZLib/adler32.c
                            Source/ZLib/compress.c
                            Source/ZLib/crc32.c
                            Source/ZLib/deflate.c
                            Source/ZLib/gzclose.c
                            Source/ZLib/gzlib.c
                            Source/ZLib/gzread.c
                            Source/ZLib/gzwrite.c
                            Source/ZLib/infback.c
                            Source/ZLib/inffast.c
                            Source/ZLib/inflate.c
                            Source/ZLib/inftrees.c
                            Source/ZLib/trees.c
                            Source/ZLib/uncompr.c
                            Source/ZLib/zutil.c
                        )

IF(WIN32)
    SET(FREEIMAGE_SRC ${FREEIMAGE_SRC} Source/FreeImage/freeimage_misc.cpp )
ENDIF()

add_library(FreeImageLite STATIC ${FREEIMAGE_SRC})

####### PUBLIC HEADER
FILE(GLOB freeimage_headers ${FREEIMAGE_DIR}/Source/FreeImageLite.h)
INSTALL(FILES ${freeimage_headers} DESTINATION include)

INSTALL(TARGETS FreeImageLite
        ARCHIVE DESTINATION lib CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
        LIBRARY DESTINATION lib CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
        RUNTIME DESTINATION bin CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
)

