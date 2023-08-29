cmake_minimum_required(VERSION 3.2...3.5)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(CheckTypeSize)
include(CheckFunctionExists)

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

set(FREEIMAGE_SHARED_DEFAULT ON)
if(VITA OR NINTENDO_DS OR NINTENDO_3DS OR NINTENDO_WII OR NINTENDO_WIIU OR NINTENDO_SWITCH)
    set(FREEIMAGE_SHARED_DEFAULT OFF)
endif()

option(FREEIMAGE_SHARED "Build shared FreeImageLite" ${FREEIMAGE_SHARED_DEFAULT})
option(FREEIMAGE_STATIC "Build static FreeImageLite" ON)

option(FREEIMAGE_USE_SYSTEM_LIBPNG "Use external libPNG" OFF)
option(FREEIMAGE_USE_SYSTEM_ZLIB "Use external ZLib" OFF)
option(FREEIMAGE_USE_SYSTEM_LIBJPEG "Use external libJPEG" OFF)

option(FREEIMAGE_ENABLE_JPEG "Enables JPEG support" ON)

set(FREEIMAGE_PNG_LIB "" CACHE STRING "Path to libPNG library")
set(FREEIMAGE_ZLIB_LIB "" CACHE STRING "Path to Zlib library")

set(FREEIMAGE_JPEG_LIB "" CACHE STRING "Path to JPEG library")

set(FREEIMAGE_PNG_INCLUDE "" CACHE STRING "Path to libPNG include")
set(FREEIMAGE_ZLIB_INCLUDE "" CACHE STRING "Path to Zlib include")

set(FREEIMAGE_JPEG_INCLUDE "" CACHE STRING "Path to libJPEG include")

if(FREEIMAGE_USE_SYSTEM_ZLIB)
    if(NOT FREEIMAGE_ZLIB_LIB OR NOT FREEIMAGE_ZLIB_INCLUDE)
        find_package(ZLIB REQUIRED)
        set(FREEIMAGE_ZLIB_LIB ${ZLIB_LIBRARIES})
        set(FREEIMAGE_ZLIB_INCLUDE ${ZLIB_INCLUDE_DIRS})
    endif()
endif()

if(FREEIMAGE_USE_SYSTEM_LIBPNG)
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

macro(fi_add_c_warning_flag WARNINGFLAG WARNING_VAR)
    check_c_compiler_flag("${WARNINGFLAG}" HAVE_CW_${WARNING_VAR})
    if(HAVE_CW_${WARNING_VAR})
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNINGFLAG}")
    endif()
endmacro()

macro(fi_add_cxx_warning_flag WARNINGFLAG WARNING_VAR)
    check_cxx_compiler_flag("${WARNINGFLAG}" HAVE_CXXW_${WARNING_VAR})
    if(HAVE_CXXW_${WARNING_VAR})
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNINGFLAG}")
    endif()
endmacro()

macro(fi_disable_c_warning_flag WARNINGFLAG WARNING_VAR)
    check_c_compiler_flag("-W${WARNINGFLAG}" HAVE_CW_${WARNING_VAR})
    if(HAVE_CW_${WARNING_VAR})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-${WARNINGFLAG}")
    endif()
endmacro()

macro(fi_disable_cxx_warning_flag WARNINGFLAG WARNING_VAR)
    check_cxx_compiler_flag("-W${WARNINGFLAG}" HAVE_CXXW_${WARNING_VAR})
    if(HAVE_CXXW_${WARNING_VAR})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-${WARNINGFLAG}")
    endif()
endmacro()

if(NOT MSVC)
    fi_disable_c_warning_flag("all" ALL)
    fi_disable_cxx_warning_flag("all" ALL)

    fi_disable_c_warning_flag("missing-field-initializers" MISSING_FIELD_INITIALIZERS)
    fi_disable_c_warning_flag("unused-variable" UNUSED_VARIABLE)
    fi_disable_c_warning_flag("unused-parameter" UNUSED_PARAMENTER)
    fi_disable_c_warning_flag("sign-compare" SIGN_COMPARE)
    fi_disable_c_warning_flag("unused-function" UNUSED_FUNCTION)
    fi_disable_c_warning_flag("pointer-sign" POINTER_SIGN)
    fi_disable_c_warning_flag("missing-field-initializers" MISSING_FIELD_INITIALIZERS)
    fi_disable_c_warning_flag("unused-variable" UNUSED_VARIABLE)
    fi_disable_c_warning_flag("unused-parameter" UNUSED_PARAMETER)
    fi_disable_c_warning_flag("parentheses" PARENTHESES)
    fi_disable_c_warning_flag("switch" SWITCH)
    fi_disable_c_warning_flag("unused-result" UNUSED_RESULT)
    fi_disable_c_warning_flag("format" FORMAT)
    fi_disable_c_warning_flag("format-security" FORMAT_SECURITY)
    fi_disable_c_warning_flag("unused-value" UNUSED)
    fi_disable_c_warning_flag("type-limits" TYPE_LIMITS)

    fi_disable_cxx_warning_flag("missing-field-initializers" MISSING_FIELD_INITIALIZERS)
    fi_disable_cxx_warning_flag("unused-variable" UNUSED_VARIABLE)
    fi_disable_cxx_warning_flag("unused-parameter" UNUSED_PARAMENTER)
    fi_disable_cxx_warning_flag("sign-compare" SIGN_COMPARE)
    fi_disable_cxx_warning_flag("unused-function" UNUSED_FUNCTION)
    fi_disable_cxx_warning_flag("missing-field-initializers" MISSING_FIELD_INITIALIZERS)
    fi_disable_cxx_warning_flag("unused-variable" UNUSED_VARIABLE)
    fi_disable_cxx_warning_flag("unused-parameter" UNUSED_PARAMETER)
    fi_disable_cxx_warning_flag("parentheses" PARENTHESES)
    fi_disable_cxx_warning_flag("switch" SWITCH)
    fi_disable_cxx_warning_flag("unused-result" UNUSED_RESULT)
    fi_disable_cxx_warning_flag("format" FORMAT)
    fi_disable_cxx_warning_flag("format-security" FORMAT_SECURITY)
    fi_disable_cxx_warning_flag("unused-value" UNUSED)
    fi_disable_cxx_warning_flag("type-limits" TYPE_LIMITS)
    fi_disable_cxx_warning_flag("reorder" REORDER)

    fi_disable_c_warning_flag("unused-const-variable" UNUSED_CONST_VARIABLE)
    fi_disable_cxx_warning_flag("unused-const-variable" UNUSED_CONST_VARIABLE)
    fi_disable_c_warning_flag("uninitialized" UNINITIALIZED)
    fi_disable_cxx_warning_flag("uninitialized" UNINITIALIZED)
    fi_disable_cxx_warning_flag("header-guard" HEADER_GUARD)
    fi_disable_cxx_warning_flag("ctor-dtor-privacy" CTOR_DTOR_PRIVACY)

    fi_disable_c_warning_flag("unused-but-set-variable" UNUSED_BUT_SET_VARIABLE)
    fi_disable_cxx_warning_flag("unused-but-set-variable" UNUSED_BUT_SET_VARIABLE)
    fi_disable_c_warning_flag("maybe-uninitialized" MAYBE_UNINITIALIZED)
    fi_disable_cxx_warning_flag("maybe-uninitialized" MAYBE_UNINITIALIZED)

    fi_disable_c_warning_flag("old-style-declaration" OLD_STYLE_DECLARATIONS)
    fi_disable_cxx_warning_flag("no-clobbered" CLOBBERED)
endif()

if(MSVC)
    # Disable bogus MSVC warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -fvisibility=hidden")

    check_cxx_compiler_flag("-stdlib=libc++" HAS_STDLIB_LIBCPP_FLAG)
    if(HAS_STDLIB_LIBCPP_FLAG)
        set(CXX_FLAG_STDLIB_LIBCPP "-stdlib=libc++")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fvisibility=hidden ${CXX_FLAG_STDLIB_LIBCPP}")
    fi_add_cxx_warning_flag("c++11-narrowing" CPP11_NARROWING)
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

# Directory for special kind of includes
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/")

if(NOT FREEIMAGE_USE_SYSTEM_ZLIB)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H)
    check_include_file(stdint.h    HAVE_STDINT_H)
    check_include_file(stddef.h    HAVE_STDDEF_H)

    set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
    if(HAVE_SYS_TYPES_H)
        list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
    endif()
    if(HAVE_STDINT_H)
        list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
    endif()
    if(HAVE_STDDEF_H)
        list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
    endif()
    check_type_size(off64_t OFF64_T)
    if(HAVE_OFF64_T)
       add_definitions(-D_LARGEFILE64_SOURCE=1)
    endif()
    set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

    check_function_exists(fseeko HAVE_FSEEKO)
    if(NOT HAVE_FSEEKO)
        add_definitions(-DNO_FSEEKO)
    endif()

    check_include_file(unistd.h Z_HAVE_UNISTD_H)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Source/3rdParty/ZLib/zconf.h.cmakein
                   ${CMAKE_CURRENT_BINARY_DIR}/include/zconf.h @ONLY)

    list(APPEND FREEIMAGE_SRC
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
endif()

if(NOT FREEIMAGE_USE_SYSTEM_LIBPNG)
    list(APPEND FreeImage_Defs
        FREEIMAGE_USE_INTERNAL_LIBPNG
    )

    option(PNG_HARDWARE_OPTIMIZATIONS "Enable Hardware Optimizations" ON)

    if(PNG_HARDWARE_OPTIMIZATIONS)
        include(${CMAKE_CURRENT_SOURCE_DIR}/TargetArch.cmake)

        target_architecture(TARGET_PROCESSOR)
        message("--- FreeImage ${TARGET_PROCESSOR} ---")

        # set definitions and sources for arm
        if(${TARGET_PROCESSOR} MATCHES "arm.*")
            message("--- ARM detected ---")
            if(VITA)
                set(PNG_ARM_NEON_POSSIBLE_VALUES on off)
                set(PNG_ARM_NEON "on") # Force this on for PS Vita to build with optimizations.
                set(PNG_ARM_NEON_NO_CACHE TRUE)
            elseif(APPLE) # TODO: Fix the Apple ARM support
                set(PNG_ARM_NEON_POSSIBLE_VALUES check on off)
                set(PNG_ARM_NEON "off") # Force disable NEON on Apple, because of failures
                set(PNG_ARM_NEON_NO_CACHE TRUE)
            elseif(${TARGET_PROCESSOR} MATCHES "arm64" OR ${TARGET_PROCESSOR} MATCHES "aarch64")
                set(PNG_ARM_NEON_POSSIBLE_VALUES on off)
                set(PNG_ARM_NEON "on" CACHE STRING "PNG: Enable ARM NEON optimizations: on|off; on is default")
            else()
                set(PNG_ARM_NEON_POSSIBLE_VALUES check on off)
                set(PNG_ARM_NEON "off" CACHE STRING "PNG: Enable ARM NEON optimizations: check|on|off; off is default")
            endif()

            if(NOT PNG_ARM_NEON_NO_CACHE)
                set_property(CACHE PNG_ARM_NEON PROPERTY STRINGS ${PNG_ARM_NEON_POSSIBLE_VALUES})
            endif()

            list(FIND PNG_ARM_NEON_POSSIBLE_VALUES ${PNG_ARM_NEON} index)

            if(index EQUAL -1)
                message(FATAL_ERROR " PNG_ARM_NEON must be one of [${PNG_ARM_NEON_POSSIBLE_VALUES}]")
            elseif(NOT ${PNG_ARM_NEON} STREQUAL "off")
                set(libpng_arm_sources
                    Source/3rdParty/LibPNG/arm/arm_init.c
                    Source/3rdParty/LibPNG/arm/filter_neon.S
                    Source/3rdParty/LibPNG/arm/filter_neon_intrinsics.c
                    Source/3rdParty/LibPNG/arm/palette_neon_intrinsics.c)
                if(${PNG_ARM_NEON} STREQUAL "on")
                    list(APPEND FreeImage_Defs PNG_ARM_NEON_OPT=2)
                    message("--- PNG NEON is ON ---")
                elseif(${PNG_ARM_NEON} STREQUAL "check")
                    list(APPEND FreeImage_Defs PNG_ARM_NEON_CHECK_SUPPORTED)
                    message("--- PNG NEON is CHECK ---")
                else()
                    message(FATAL_ERROR "--- PNG NEON is UNDEFINED ---")
                endif()
            else()
                list(APPEND FreeImage_Defs PNG_ARM_NEON_OPT=0)
                message("--- PNG NEON is DISABLED ---")
            endif()
        endif()

        # set definitions and sources for powerpc
        if(${TARGET_PROCESSOR} MATCHES "ppc.*")
            message("--- PowerPC detected ---")
            set(PNG_POWERPC_VSX_POSSIBLE_VALUES on off)
            set(PNG_POWERPC_VSX "on" CACHE STRING "Enable POWERPC VSX optimizations: on|off; on is default")
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
                    list(APPEND FreeImage_Defs PNG_POWERPC_VSX_OPT=2)
                endif()
            else()
                list(APPEND FreeImage_Defs PNG_POWERPC_VSX_OPT=0)
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
                    list(APPEND FreeImage_Defs PNG_INTEL_SSE_OPT=1)
                endif()
            else()
                list(APPEND FreeImage_Defs PNG_INTEL_SSE_OPT=0)
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
                    list(APPEND FreeImage_Defs PNG_MIPS_MSA_OPT=2)
                endif()
            else()
                list(APPEND FreeImage_Defs PNG_MIPS_MSA_OPT=0)
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
    )
    set(FREEIMAGE_PNG_INCLUDE "Source/3rdParty/LibPNG")
endif()

if(FREEIMAGE_ENABLE_JPEG AND NOT FREEIMAGE_USE_SYSTEM_LIBJPEG)
    list(APPEND FreeImage_Defs
        FREEIMAGE_USE_INTERNAL_LIBJPEG
    )

    check_include_file(stddef.h HAVE_STDDEF_H)
    check_include_file(stdlib.h HAVE_STDLIB_H)
    check_include_file(string.h HAVE_STRING_H)
    check_symbol_exists(size_t stddef.h stdlib.h stdio.h HAVE_ANSI_SIZE_T)
    check_symbol_exists(setmode io.h USE_SETMODE)
    check_c_source_compiles(
    "
    void f()
    {
        char x[ (char)0xff ]; /* x[-1] if signed */
    }
    "
    CHAR_IS_UNSIGNED)

    check_c_source_compiles(
    "
    void f()
    {
        char x[ ((signed char)0xff) >> 1 ]; /* x[-1] if signed */
    }
    "
    RIGHT_SHIFT_IS_UNSIGNED)

    if(NOT HAVE_STRING_H)
        set(NEED_BSD_STRINGS TRUE)
    else()
        set(NEED_BSD_STRINGS FALSE)
    endif()

    if(NOT HAVE_ANSI_SIZE_T)
        set(NEED_SYS_TYPES_H TRUE)
    else()
        set(NEED_SYS_TYPES_H FALSE)
    endif()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Source/3rdParty/LibJPEG/jconfig.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/jconfig.h")

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
    if(CMAKE_STATIC_LIBRARY_PREFIX STREQUAL "" AND CMAKE_STATIC_LIBRARY_SUFFIX STREQUAL ".lib")
        set_target_properties(FreeImageLiteStatic PROPERTIES OUTPUT_NAME FreeImageLite-static)
    else()
        set_target_properties(FreeImageLiteStatic PROPERTIES OUTPUT_NAME FreeImageLite)
    endif()
    target_include_directories(FreeImageLiteStatic PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include" "${FREEIMAGE_ZLIB_INCLUDE}" "${FREEIMAGE_PNG_INCLUDE}" "${FREEIMAGE_JPEG_INCLUDE}")
    target_compile_definitions(FreeImageLiteStatic PRIVATE ${FreeImage_Defs} "-DFREEIMAGE_LIB")
    list(APPEND FREEIMAGE_TARGETS FreeImageLiteStatic)
endif()

if(FREEIMAGE_SHARED)
    add_library(FreeImageLiteShared SHARED ${FREEIMAGE_SRC})
    set_target_properties(FreeImageLiteShared PROPERTIES OUTPUT_NAME FreeImageLite)
    target_include_directories(FreeImageLiteShared PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include" "${FREEIMAGE_ZLIB_INCLUDE}" "${FREEIMAGE_PNG_INCLUDE}" "${FREEIMAGE_JPEG_INCLUDE}")
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

option(FREEIMAGE_BUILD_TESTS "Do build of test applications" OFF)
if(FREEIMAGE_BUILD_TESTS)
    add_subdirectory(test)
endif()
