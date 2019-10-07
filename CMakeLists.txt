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
project(FreeImage)

set(FreeImage_Defs)

####### DEFINES
list(APPEND FreeImage_Defs
    "-DVER_MAJOR=3"
    "-DVER_MINOR=17.0"
    -DFREEIMAGE_LITE
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(EMSCRIPTEN 1)
endif()

if(WIN32)
    list(APPEND FreeImage_Defs
        "-DOPJ_STATIC"
        "-DLIBRAW_NODLL"
        "-DFREEIMAGE_LIB=1"
    )
endif()

if(UNIX)
    if(APPLE)
        list(APPEND FreeImage_Defs
            "-DNO_LCMS"
            "-D__ANSI__"
            "-DDISABLE_PERF_MEASUREMENT"
        )
    else()
        list(APPEND FreeImage_Defs "-DFREEIMAGE_LIB=1")
    endif()
endif()

option(FREEIMAGE_USE_SYSTEM_LIBPNG "Use external libPNG and ZLib" OFF)

if(NOT FREEIMAGE_USE_SYSTEM_LIBPNG AND ANDROID)
    # Disable NEON in libPNG which is broken in current version, TODO: Update libPNG
    list(APPEND FreeImage_Defs "-DPNG_ARM_NEON_OPT=0")
endif()

####### COMPILER FLAGS
set(CMAKE_CXX_STANDARD 11)

if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-sign-compare -Wno-unused-function -Wno-implicit-function-declaration -Wno-pointer-sign \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-sign-compare -Wno-unused-value \
                -Wno-type-limits")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                -Wno-sign-compare -Wno-unused-function \
                -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter  \
                -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-unused-value \
                -Wno-type-limits -Wno-reorder")
endif()

# Disable bogus MSVC warnings
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
endif()

if(APPLE)
    string(REGEX REPLACE "-O3" ""
        CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REGEX REPLACE "-O3" ""
        CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-const-variable -Wno-uninitialized -fexceptions -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-const-variable -Wno-uninitialized -Wno-header-guard -fexceptions \
                                            -fvisibility=hidden -Wno-ctor-dtor-privacy -stdlib=libc++ -Wc++11-narrowing")
else()
    if(NOT MSVC AND NOT EMSCRIPTEN AND NOT ANDROID)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-old-style-declaration")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-clobbered")
    endif()
endif()

####### INCLUDE PATHS
include_directories(${FREEIMAGE_DIR}/Source/)

####### SOURCE CODE
set(FREEIMAGE_SRC ${CMAKE_CURRENT_SOURCE_DIR}
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
)

if(NOT FREEIMAGE_USE_SYSTEM_LIBPNG)
    list(APPEND FreeImage_Defs
        FREEIMAGE_USE_INTERNAL_LIBPNG
    )
    list(APPEND FREEIMAGE_SRC
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
endif()

if(WIN32)
    set(FREEIMAGE_SRC ${FREEIMAGE_SRC} Source/FreeImage/freeimage_misc.cpp)
endif()

add_library(FreeImageLite STATIC ${FREEIMAGE_SRC})
target_compile_definitions(FreeImageLite PRIVATE ${FreeImage_Defs})

####### PUBLIC HEADER
file(GLOB freeimage_headers ${FREEIMAGE_DIR}/Source/FreeImageLite.h)
install(FILES ${freeimage_headers} DESTINATION include)

install(TARGETS FreeImageLite
    ARCHIVE DESTINATION lib CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
    LIBRARY DESTINATION lib CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
    RUNTIME DESTINATION bin CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
)
