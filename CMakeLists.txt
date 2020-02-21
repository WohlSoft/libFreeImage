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
    "-DVER_MINOR=18.0"
    -DFREEIMAGE_LITE
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(EMSCRIPTEN 1)
endif()

list(APPEND FreeImage_Defs
    "-DFREEIMAGE_BUILD"
)

option(FREEIMAGE_SHARED "Build shared FreeImageLite" ON)
option(FREEIMAGE_STATIC "Build static FreeImageLite" ON)

option(FREEIMAGE_USE_SYSTEM_LIBPNG "Use external libPNG and ZLib" OFF)

set(FREEIMAGE_PNG_LIB "" CACHE STRING "Path to libPNG library")
set(FREEIMAGE_ZLIB_LIB "" CACHE STRING "Path to Zlib library")

set(FREEIMAGE_PNG_INCLUDE "" CACHE STRING "Path to libPNG include")
set(FREEIMAGE_ZLIB_INCLUDE "" CACHE STRING "Path to Zlib include")

if(NOT FREEIMAGE_USE_SYSTEM_LIBPNG AND ANDROID)
    # Disable NEON in libPNG which is broken in current version, TODO: Update libPNG
    list(APPEND FreeImage_Defs "-DPNG_ARM_NEON_OPT=0")
endif()

if(NOT FREEIMAGE_USE_SYSTEM_LIBPNG)
    if(NOT FREEIMAGE_ZLIB_LIB OR NOT FREEIMAGE_ZLIB_INCLUDE)
        find_package(ZLIB REQUIRED)
        set(FREEIMAGE_ZLIB_LIB ${ZLIB_LIBRARIES})
        set(FREEIMAGE_ZLIB_INCLUDE ${ZLIB_INCLUDE_DIRS})
    endif()

    if(NOT FREEIMAGE_PNG_LIB OR NOT FREEIMAGE_PNG_INCLUDE)
        find_package(PNG REQUIRED)
        set(FREEIMAGE_PNG_LIB ${PNG_LIBRARIES})
        set(FREEIMAGE_PNG_INCLUDE ${PNG_INCLUDE_DIRS})
    endif()
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
include_directories(
    ${FREEIMAGE_DIR}/Source
    ${CMAKE_CURRENT_BINARY_DIR}/config
)

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
        Source/3rdParty/LibPNG/png.c
        Source/3rdParty/LibPNG/pngerror.c
        Source/3rdParty/LibPNG/pngget.c
        Source/3rdParty/LibPNG/pngmem.c
        Source/3rdParty/LibPNG/pngpread.c
        Source/3rdParty/LibPNG/pngread.c
        Source/3rdParty/LibPNG/pngrio.c
        Source/3rdParty/LibPNG/pngrtran.c
        Source/3rdParty/LibPNG/pngrutil.c
        Source/3rdParty/LibPNG/pngset.c
        Source/3rdParty/LibPNG/pngtrans.c
        Source/3rdParty/LibPNG/pngwio.c
        Source/3rdParty/LibPNG/pngwrite.c
        Source/3rdParty/LibPNG/pngwtran.c
        Source/3rdParty/LibPNG/pngwutil.c
        Source/3rdParty/ZLib/adler32.c
        Source/3rdParty/ZLib/compress.c
        Source/3rdParty/ZLib/crc32.c
        Source/3rdParty/ZLib/deflate.c
        Source/3rdParty/ZLib/gzclose.c
        Source/3rdParty/ZLib/gzlib.c
        Source/3rdParty/ZLib/gzread.c
        Source/3rdParty/ZLib/gzwrite.c
        Source/3rdParty/ZLib/infback.c
        Source/3rdParty/ZLib/inffast.c
        Source/3rdParty/ZLib/inflate.c
        Source/3rdParty/ZLib/inftrees.c
        Source/3rdParty/ZLib/trees.c
        Source/3rdParty/ZLib/uncompr.c
        Source/3rdParty/ZLib/zutil.c
    )
endif()

if(WIN32)
    set(FREEIMAGE_SRC ${FREEIMAGE_SRC} Source/FreeImage/freeimage_misc.cpp)
endif()

set(FREEIMAGE_TARGETS)
if(FREEIMAGE_STATIC)
    add_library(FreeImageLiteStatic STATIC ${FREEIMAGE_SRC})
    if(WIN32)
        set_target_properties(FreeImageLiteStatic PROPERTIES OUTPUT_NAME FreeImageLite-static)
    else()
        set_target_properties(FreeImageLiteStatic PROPERTIES OUTPUT_NAME FreeImageLite)
    endif()
    target_include_directories(FreeImageLiteStatic PUBLIC "${FREEIMAGE_ZLIB_INCLUDE}" "${FREEIMAGE_PNG_INCLUDE}")
    target_compile_definitions(FreeImageLiteStatic PRIVATE ${FreeImage_Defs} "-DFREEIMAGE_LIB")
    list(APPEND FREEIMAGE_TARGETS FreeImageLiteStatic)
endif()

if(FREEIMAGE_SHARED)
    add_library(FreeImageLiteShared SHARED ${FREEIMAGE_SRC})
    set_target_properties(FreeImageLiteShared PROPERTIES OUTPUT_NAME FreeImageLite)
    target_include_directories(FreeImageLiteShared PUBLIC "${FREEIMAGE_ZLIB_INCLUDE}" "${FREEIMAGE_PNG_INCLUDE}")
    target_link_libraries(FreeImageLiteShared PUBLIC "${FREEIMAGE_ZLIB_LIB}" "${FREEIMAGE_PNG_LIB}")
    target_compile_definitions(FreeImageLiteShared PRIVATE ${FreeImage_Defs})
    list(APPEND FREEIMAGE_TARGETS FreeImageLiteShared)
endif()

####### PUBLIC HEADER
install(FILES
    "${FREEIMAGE_DIR}/Source/FreeImageLite.h"
    DESTINATION include
)

install(TARGETS ${FREEIMAGE_TARGETS}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

