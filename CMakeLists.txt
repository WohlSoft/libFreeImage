cmake_minimum_required(VERSION 3.2)

####### ADD PREDEFINED DEFINITIONS:
SET(FREEIMAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(LIBPNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/3rdParty/LibPNG/)
SET(LIBJPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/3rdParty/LibJPEG/)
SET(ZLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/3rdParty/zlib/)

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
option(FREEIMAGE_USE_SYSTEM_LIBJPEG "Use external libJPEG" OFF)

option(FREEIMAGE_ENABLE_JPEG "Enables JPEG support" ON)

set(FREEIMAGE_PNG_LIB "" CACHE STRING "Path to libPNG library")
set(FREEIMAGE_ZLIB_LIB "" CACHE STRING "Path to Zlib library")

set(FREEIMAGE_JPEG_LIB "" CACHE STRING "Path to JPEG library")

set(FREEIMAGE_PNG_INCLUDE "" CACHE STRING "Path to libPNG include")
set(FREEIMAGE_ZLIB_INCLUDE "" CACHE STRING "Path to Zlib include")

set(FREEIMAGE_JPEG_INCLUDE "" CACHE STRING "Path to libJPEG include")

if(FREEIMAGE_USE_SYSTEM_LIBPNG)
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

if(FREEIMAGE_ENABLE_JPEG AND FREEIMAGE_USE_SYSTEM_LIBJPEG)
    if(NOT FREEIMAGE_JPEG_INCLUDE)
        find_package(JPEG REQUIRED)
        set(FREEIMAGE_JPEG_LIB ${JPEG_LIBRARIES})
        set(FREEIMAGE_JPEG_INCLUDE ${JPEG_INCLUDE_DIRS})
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

if(MSVC)
    # Disable bogus MSVC warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
    # Enable unwind for ARM builds
    if(${MSVC_C_ARCHITECTURE_ID} MATCHES "ARM")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHsc")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    endif()
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
    if(NOT MSVC AND NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
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

if(FREEIMAGE_ENABLE_JPEG)
    list(APPEND FreeImage_Defs -DFREEIMAGE_ENABLE_JPEG)
    list(APPEND FREEIMAGE_SRC
        Source/FreeImage/PluginJPEG.cpp
        Source/Metadata/Exif.cpp
    )
endif()

if(NOT FREEIMAGE_USE_SYSTEM_LIBPNG)
    list(APPEND FreeImage_Defs
        FREEIMAGE_USE_INTERNAL_LIBPNG
    )

    option(PNG_HARDWARE_OPTIMIZATIONS "Enable Hardware Optimizations" ON)

    if(PNG_HARDWARE_OPTIMIZATIONS)

        include(${CMAKE_CURRENT_SOURCE_DIR}/TargetArch.cmake)

        target_architecture(TARGET_PROCESSOR)
        message("--- ${TARGET_PROCESSOR} ---")

        # set definitions and sources for arm
        if(${TARGET_PROCESSOR} MATCHES "arm.*")
            message("--- ARM detected ---")
            set(PNG_ARM_NEON_POSSIBLE_VALUES check on off)
            set(PNG_ARM_NEON "check" CACHE STRING "Enable ARM NEON optimizations:
                check: (default) use internal checking code;
                off: disable the optimizations;
                on: turn on unconditionally.")
            set_property(CACHE PNG_ARM_NEON PROPERTY STRINGS ${PNG_ARM_NEON_POSSIBLE_VALUES})
            list(FIND PNG_ARM_NEON_POSSIBLE_VALUES ${PNG_ARM_NEON} index)
            if(index EQUAL -1)
                message(FATAL_ERROR " PNG_ARM_NEON must be one of [${PNG_ARM_NEON_POSSIBLE_VALUES}]")
            elseif(NOT ${PNG_ARM_NEON} STREQUAL "no" AND NOT APPLE AND NOT ANDROID) # TODO: Fix the Apple ARM support
                set(libpng_arm_sources
                    Source/3rdParty/LibPNG/arm/arm_init.c
                    Source/3rdParty/LibPNG/arm/filter_neon.S
                    Source/3rdParty/LibPNG/arm/filter_neon_intrinsics.c)
                if(${PNG_ARM_NEON} STREQUAL "on")
                    add_definitions(-DPNG_ARM_NEON_OPT=2)
                    message("--- PNG NEON is ON ---")
                elseif(${PNG_ARM_NEON} STREQUAL "check")
                    add_definitions(-DPNG_ARM_NEON_OPT=2)
                    add_definitions(-DPNG_ARM_NEON_CHECK_SUPPORTED)
                    message("--- PNG NEON is CHECK ---")
                else()
                    message(FATAL_ERROR "--- PNG NEON is UNDEFINED ---")
                endif()
            else()
                add_definitions(-DPNG_ARM_NEON_OPT=0)
            endif()
        endif()

        # set definitions and sources for powerpc
        if(${TARGET_PROCESSOR} MATCHES "ppc.*")
            message("--- PowerPC detected ---")
            set(PNG_POWERPC_VSX_POSSIBLE_VALUES on off)
            set(PNG_POWERPC_VSX "on" CACHE STRING "Enable POWERPC VSX optimizations:
                off: disable the optimizations.")
            set_property(CACHE PNG_POWERPC_VSX PROPERTY STRINGS ${PNG_POWERPC_VSX_POSSIBLE_VALUES})
            list(FIND PNG_POWERPC_VSX_POSSIBLE_VALUES ${PNG_POWERPC_VSX} index)
            if(index EQUAL -1)
                message(FATAL_ERROR
                " PNG_POWERPC_VSX must be one of [${PNG_POWERPC_VSX_POSSIBLE_VALUES}]")
            elseif(NOT ${PNG_POWERPC_VSX} STREQUAL "no")
                set(libpng_powerpc_sources
                    Source/3rdParty/LibPNG/powerpc/powerpc_init.c
                    Source/3rdParty/LibPNG/powerpc/filter_vsx_intrinsics.c)
                if(${PNG_POWERPC_VSX} STREQUAL "on")
                    add_definitions(-DPNG_POWERPC_VSX_OPT=2)
                endif()
            else()
                add_definitions(-DPNG_POWERPC_VSX_OPT=0)
            endif()
        endif()

        # set definitions and sources for intel
        if(TARGET_PROCESSOR MATCHES "x86_64|AMD64" OR TARGET_PROCESSOR MATCHES "^i.86$")
            message("--- Intel/AMD detected ---")
            set(PNG_INTEL_SSE_POSSIBLE_VALUES on off)
            set(PNG_INTEL_SSE "on" CACHE STRING "Enable INTEL_SSE optimizations:
             off: disable the optimizations")
            set_property(CACHE PNG_INTEL_SSE PROPERTY STRINGS
             ${PNG_INTEL_SSE_POSSIBLE_VALUES})
            list(FIND PNG_INTEL_SSE_POSSIBLE_VALUES ${PNG_INTEL_SSE} index)
            if(index EQUAL -1)
                message(FATAL_ERROR " PNG_INTEL_SSE must be one of [${PNG_INTEL_SSE_POSSIBLE_VALUES}]")
            elseif(NOT ${PNG_INTEL_SSE} STREQUAL "no")
                set(libpng_intel_sources
                    Source/3rdParty/LibPNG/intel/intel_init.c
                    Source/3rdParty/LibPNG/intel/filter_sse2_intrinsics.c)
                if(${PNG_INTEL_SSE} STREQUAL "on")
                    add_definitions(-DPNG_INTEL_SSE_OPT=1)
                endif()
            else()
                add_definitions(-DPNG_INTEL_SSE_OPT=0)
            endif()
        endif()

        # set definitions and sources for MIPS
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "mipsel*" OR
            CMAKE_SYSTEM_PROCESSOR MATCHES "mips64el*" )
            message("--- MIPS detected ---")
            set(PNG_MIPS_MSA_POSSIBLE_VALUES on off)
            set(PNG_MIPS_MSA "on" CACHE STRING "Enable MIPS_MSA optimizations:
                off: disable the optimizations")
            set_property(CACHE PNG_MIPS_MSA PROPERTY STRINGS ${PNG_MIPS_MSA_POSSIBLE_VALUES})
            list(FIND PNG_MIPS_MSA_POSSIBLE_VALUES ${PNG_MIPS_MSA} index)
            if(index EQUAL -1)
                message(FATAL_ERROR " PNG_MIPS_MSA must be one of [${PNG_MIPS_MSA_POSSIBLE_VALUES}]")
            elseif(NOT ${PNG_MIPS_MSA} STREQUAL "no")
                set(libpng_mips_sources
                    Source/3rdParty/LibPNG/mips/mips_init.c
                    Source/3rdParty/LibPNG/mips/filter_msa_intrinsics.c)
                if(${PNG_MIPS_MSA} STREQUAL "on")
                    add_definitions(-DPNG_MIPS_MSA_OPT=2)
                endif()
            else()
                add_definitions(-DPNG_MIPS_MSA_OPT=0)
            endif()
        endif()

    endif(PNG_HARDWARE_OPTIMIZATIONS)

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
        ${libpng_arm_sources}
        ${libpng_intel_sources}
        ${libpng_mips_sources}
        ${libpng_powerpc_sources}
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
    set(FREEIMAGE_ZLIB_INCLUDE "Source/3rdParty/ZLib")
    set(FREEIMAGE_PNG_INCLUDE "Source/3rdParty/LibPNG")
endif()

if(FREEIMAGE_ENABLE_JPEG AND NOT FREEIMAGE_USE_SYSTEM_LIBJPEG)
    list(APPEND FreeImage_Defs
        FREEIMAGE_USE_INTERNAL_LIBJPEG
    )

    list(APPEND FREEIMAGE_SRC
        Source/3rdParty/LibJPEG/jaricom.c
        Source/3rdParty/LibJPEG/jcapimin.c
        Source/3rdParty/LibJPEG/jcapistd.c
        Source/3rdParty/LibJPEG/jcarith.c
        Source/3rdParty/LibJPEG/jccoefct.c
        Source/3rdParty/LibJPEG/jccolor.c
        Source/3rdParty/LibJPEG/jcdctmgr.c
        Source/3rdParty/LibJPEG/jchuff.c
        Source/3rdParty/LibJPEG/jcinit.c
        Source/3rdParty/LibJPEG/jcmainct.c
        Source/3rdParty/LibJPEG/jcmarker.c
        Source/3rdParty/LibJPEG/jcmaster.c
        Source/3rdParty/LibJPEG/jcomapi.c
        Source/3rdParty/LibJPEG/jcparam.c
        Source/3rdParty/LibJPEG/jcprepct.c
        Source/3rdParty/LibJPEG/jcsample.c
        Source/3rdParty/LibJPEG/jctrans.c
        Source/3rdParty/LibJPEG/jdapimin.c
        Source/3rdParty/LibJPEG/jdapistd.c
        Source/3rdParty/LibJPEG/jdarith.c
        Source/3rdParty/LibJPEG/jdatadst.c
        Source/3rdParty/LibJPEG/jdatasrc.c
        Source/3rdParty/LibJPEG/jdcoefct.c
        Source/3rdParty/LibJPEG/jdcolor.c
        Source/3rdParty/LibJPEG/jddctmgr.c
        Source/3rdParty/LibJPEG/jdhuff.c
        Source/3rdParty/LibJPEG/jdinput.c
        Source/3rdParty/LibJPEG/jdmainct.c
        Source/3rdParty/LibJPEG/jdmarker.c
        Source/3rdParty/LibJPEG/jdmaster.c
        Source/3rdParty/LibJPEG/jdmerge.c
        Source/3rdParty/LibJPEG/jdpostct.c
        Source/3rdParty/LibJPEG/jdsample.c
        Source/3rdParty/LibJPEG/jdtrans.c
        Source/3rdParty/LibJPEG/jerror.c
        Source/3rdParty/LibJPEG/jfdctflt.c
        Source/3rdParty/LibJPEG/jfdctfst.c
        Source/3rdParty/LibJPEG/jfdctint.c
        Source/3rdParty/LibJPEG/jidctflt.c
        Source/3rdParty/LibJPEG/jidctfst.c
        Source/3rdParty/LibJPEG/jidctint.c
        Source/3rdParty/LibJPEG/jmemmgr.c
        Source/3rdParty/LibJPEG/jmemnobs.c
        Source/3rdParty/LibJPEG/jquant1.c
        Source/3rdParty/LibJPEG/jquant2.c
        Source/3rdParty/LibJPEG/jutils.c
        Source/3rdParty/LibJPEG/transupp.c
    )
    set(FREEIMAGE_JPEG_INCLUDE "Source/3rdParty/LibJPEG")
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
    target_include_directories(FreeImageLiteStatic PUBLIC "${FREEIMAGE_ZLIB_INCLUDE}" "${FREEIMAGE_PNG_INCLUDE}" "${FREEIMAGE_JPEG_INCLUDE}")
    target_compile_definitions(FreeImageLiteStatic PRIVATE ${FreeImage_Defs} "-DFREEIMAGE_LIB")
    list(APPEND FREEIMAGE_TARGETS FreeImageLiteStatic)
endif()

if(FREEIMAGE_SHARED)
    add_library(FreeImageLiteShared SHARED ${FREEIMAGE_SRC})
    set_target_properties(FreeImageLiteShared PROPERTIES OUTPUT_NAME FreeImageLite)
    target_include_directories(FreeImageLiteShared PUBLIC "${FREEIMAGE_ZLIB_INCLUDE}" "${FREEIMAGE_PNG_INCLUDE}" "${FREEIMAGE_JPEG_INCLUDE}")
    target_link_libraries(FreeImageLiteShared PUBLIC "${FREEIMAGE_ZLIB_LIB}" "${FREEIMAGE_PNG_LIB}" "${FREEIMAGE_JPEG_LIB}")
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
