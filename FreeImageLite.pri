TEMPLATE = lib
CONFIG  -= app_bundle
CONFIG  -= qt
CONFIG  -= dll
CONFIG  += staticlib
CONFIG  += c++11

DEFINES += VER_MAJOR=3 VER_MINOR=17.0 FREEIMAGE_LITE

!win*-msvc*: {
    QMAKE_CFLAGS += -std=c11

    QMAKE_CFLAGS_WARN_ON = -Wall \
                    -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                    -Wno-sign-compare -Wno-unused-function -Wno-implicit-function-declaration -Wno-pointer-sign \
                    -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                    -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-sign-compare -Wno-unused-value \
                    -Wno-type-limits

    QMAKE_CXXFLAGS_WARN_ON = -Wall \
                    -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter \
                    -Wno-sign-compare -Wno-unused-function \
                    -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter  \
                    -Wno-parentheses -Wno-switch -Wno-unused-result -Wno-format -Wno-unused-value \
                    -Wno-type-limits -Wno-reorder
} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON    += /wd4244 /wd4100 /wd4996
    QMAKE_CXXFLAGS_WARN_ON  += /wd4244 /wd4100
}

macx:{
    QMAKE_CFLAGS_WARN_ON    += -Wno-unused-const-variable -Wno-uninitialized
    QMAKE_CXXFLAGS_WARN_ON  += -Wno-unused-const-variable -Wno-uninitialized -Wno-header-guard
} else {
    !win*-msvc*: {
        QMAKE_CFLAGS_WARN_ON    += -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-old-style-declaration
        QMAKE_CXXFLAGS_WARN_ON  += -Wno-unused-but-set-variable -Wno-maybe-uninitialized -Wno-clobbered
        QMAKE_LFLAGS    += -Wl,-rpath=\'\$\$ORIGIN\'
    }
}

win32:{
    DEFINES     += OPJ_STATIC LIBRAW_NODLL FREEIMAGE_LIB=1 #__ANSI__ DISABLE_PERF_MEASUREMENT
}
linux-g++||unix:!macx:!android:{
    CONFIG      += unversioned_libname
    DEFINES     += FREEIMAGE_LIB=1
}
android:{
    warning("NOT PORTED YET!")
}
macx:{
    QMAKE_CFLAGS    += -fexceptions -fvisibility=hidden
    QMAKE_CXXFLAGS  += -fexceptions -fvisibility=hidden -Wno-ctor-dtor-privacy -stdlib=libc++ -Wc++11-narrowing
    QMAKE_CFLAGS_RELEASE    += -Os
    QMAKE_CXXFLAGS_RELEASE  += -Os
    DEFINES += NO_LCMS __ANSI__ DISABLE_PERF_MEASUREMENT
}

MAKEFILE     = Makefile.FreeImageLITE
RC_FILE      = $$PWD/FreeImage.rc
INCLUDEPATH += $$PWD/Source

HEADERS += \
    $$PWD/Source/DeprecationManager/DeprecationMgr.h \
    $$PWD/Source/FreeImageToolkit/Filters.h \
    $$PWD/Source/FreeImageToolkit/Resize.h \
    $$PWD/Source/LibPNG/png.h \
    $$PWD/Source/LibPNG/pngconf.h \
    $$PWD/Source/LibPNG/pngdebug.h \
    $$PWD/Source/LibPNG/pnginfo.h \
    $$PWD/Source/LibPNG/pnglibconf.h \
    $$PWD/Source/LibPNG/pngpriv.h \
    $$PWD/Source/LibPNG/pngstruct.h \
    $$PWD/Source/Metadata/FIRational.h \
    $$PWD/Source/Metadata/FreeImageTag.h \
    $$PWD/Source/ZLib/crc32.h \
    $$PWD/Source/ZLib/deflate.h \
    $$PWD/Source/ZLib/gzguts.h \
    $$PWD/Source/ZLib/inffast.h \
    $$PWD/Source/ZLib/inffixed.h \
    $$PWD/Source/ZLib/inflate.h \
    $$PWD/Source/ZLib/inftrees.h \
    $$PWD/Source/ZLib/trees.h \
    $$PWD/Source/ZLib/zconf.h \
    $$PWD/Source/ZLib/zlib.h \
    $$PWD/Source/ZLib/zutil.h \
    $$PWD/Source/CacheFile.h \
    $$PWD/Source/FreeImageIO.h \
    $$PWD/Source/MapIntrospector.h \
    $$PWD/Source/Plugin.h \
    $$PWD/Source/Quantizers.h \
    $$PWD/Source/ToneMapping.h \
    $$PWD/Source/Utilities.h \
    $$PWD/Source/FreeImage.h \
    $$PWD/Source/FreeImageLite.h \
    $$PWD/Source/FreeImage/FreeImage_misc.h

SOURCES += \
    $$PWD/Source/FreeImage/BitmapAccess.cpp \
    $$PWD/Source/FreeImage/ColorLookup.cpp \
    $$PWD/Source/FreeImage/FreeImage.cpp \
    $$PWD/Source/FreeImage/FreeImageIO.cpp \
    $$PWD/Source/FreeImage/GetType.cpp \
    $$PWD/Source/FreeImage/MemoryIO.cpp \
    $$PWD/Source/FreeImage/PixelAccess.cpp \
    $$PWD/Source/FreeImage/Plugin.cpp \
    $$PWD/Source/FreeImage/PluginBMP.cpp \
    $$PWD/Source/FreeImage/PluginGIF.cpp \
    $$PWD/Source/FreeImage/PluginICO.cpp \
    $$PWD/Source/FreeImage/PluginPNG.cpp \
    $$PWD/Source/FreeImage/Conversion.cpp \
    $$PWD/Source/FreeImage/Conversion16_555.cpp \
    $$PWD/Source/FreeImage/Conversion16_565.cpp \
    $$PWD/Source/FreeImage/Conversion24.cpp \
    $$PWD/Source/FreeImage/Conversion32.cpp \
    $$PWD/Source/FreeImage/Conversion4.cpp \
    $$PWD/Source/FreeImage/Conversion8.cpp \
    $$PWD/Source/FreeImage/ConversionFloat.cpp \
    $$PWD/Source/FreeImage/ConversionRGB16.cpp \
    $$PWD/Source/FreeImage/ConversionRGBA16.cpp \
    $$PWD/Source/FreeImage/ConversionRGBAF.cpp \
    $$PWD/Source/FreeImage/ConversionRGBF.cpp \
    $$PWD/Source/FreeImage/ConversionType.cpp \
    $$PWD/Source/FreeImage/ConversionUINT16.cpp \
    $$PWD/Source/FreeImage/Halftoning.cpp \
    $$PWD/Source/FreeImage/tmoColorConvert.cpp \
    $$PWD/Source/FreeImage/tmoDrago03.cpp \
    $$PWD/Source/FreeImage/tmoFattal02.cpp \
    $$PWD/Source/FreeImage/tmoReinhard05.cpp \
    $$PWD/Source/FreeImage/ToneMapping.cpp \
    $$PWD/Source/FreeImage/LFPQuantizer.cpp \
    $$PWD/Source/FreeImage/NNQuantizer.cpp \
    $$PWD/Source/FreeImage/WuQuantizer.cpp \
    $$PWD/Source/DeprecationManager/Deprecated.cpp \
    $$PWD/Source/DeprecationManager/DeprecationMgr.cpp \
    $$PWD/Source/FreeImage/CacheFile.cpp \
    $$PWD/Source/FreeImage/MultiPage.cpp \
    $$PWD/Source/FreeImage/ZLibInterface.cpp \
    $$PWD/Source/Metadata/FIRational.cpp \
    $$PWD/Source/Metadata/FreeImageTag.cpp \
    $$PWD/Source/Metadata/IPTC.cpp \
    $$PWD/Source/Metadata/TagConversion.cpp \
    $$PWD/Source/Metadata/TagLib.cpp \
    $$PWD/Source/FreeImageToolkit/Background.cpp \
    $$PWD/Source/FreeImageToolkit/BSplineRotate.cpp \
    $$PWD/Source/FreeImageToolkit/Channels.cpp \
    $$PWD/Source/FreeImageToolkit/ClassicRotate.cpp \
    $$PWD/Source/FreeImageToolkit/Colors.cpp \
    $$PWD/Source/FreeImageToolkit/CopyPaste.cpp \
    $$PWD/Source/FreeImageToolkit/Display.cpp \
    $$PWD/Source/FreeImageToolkit/Flip.cpp \
    $$PWD/Source/FreeImageToolkit/MultigridPoissonSolver.cpp \
    $$PWD/Source/FreeImageToolkit/Rescale.cpp \
    $$PWD/Source/FreeImageToolkit/Resize.cpp \
    $$PWD/Source/LibPNG/png.c \
    $$PWD/Source/LibPNG/pngerror.c \
    $$PWD/Source/LibPNG/pngget.c \
    $$PWD/Source/LibPNG/pngmem.c \
    $$PWD/Source/LibPNG/pngpread.c \
    $$PWD/Source/LibPNG/pngread.c \
    $$PWD/Source/LibPNG/pngrio.c \
    $$PWD/Source/LibPNG/pngrtran.c \
    $$PWD/Source/LibPNG/pngrutil.c \
    $$PWD/Source/LibPNG/pngset.c \
    $$PWD/Source/LibPNG/pngtrans.c \
    $$PWD/Source/LibPNG/pngwio.c \
    $$PWD/Source/LibPNG/pngwrite.c \
    $$PWD/Source/LibPNG/pngwtran.c \
    $$PWD/Source/LibPNG/pngwutil.c \
    $$PWD/Source/ZLib/adler32.c \
    $$PWD/Source/ZLib/compress.c \
    $$PWD/Source/ZLib/crc32.c \
    $$PWD/Source/ZLib/deflate.c \
    $$PWD/Source/ZLib/gzclose.c \
    $$PWD/Source/ZLib/gzlib.c \
    $$PWD/Source/ZLib/gzread.c \
    $$PWD/Source/ZLib/gzwrite.c \
    $$PWD/Source/ZLib/infback.c \
    $$PWD/Source/ZLib/inffast.c \
    $$PWD/Source/ZLib/inflate.c \
    $$PWD/Source/ZLib/inftrees.c \
    $$PWD/Source/ZLib/trees.c \
    $$PWD/Source/ZLib/uncompr.c \
    $$PWD/Source/ZLib/zutil.c

win32: include($$PWD/Source/FreeImage/freeimage_misc.pro)

