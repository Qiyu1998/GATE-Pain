#-------------------------------------------------
#
# Project created by QtCreator 2017-12-19T14:58:19
#
#-------------------------------------------------

QT += core gui
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iDBS-TRP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    amplmodulation.cpp \
    cleandbs.cpp \
    closedloop.cpp \
    dualthresholds.cpp \
    lib/cascadeiir/biquad.cpp \
    lib/cascadeiir/cascadeiir.cpp \
    lib/cascadeiir/state.cpp \
    lib/kissfft/kiss_fft.cpp \
    lib/kissfft/kiss_fftr.cpp \
    main.cpp \
    mainwindow.cpp \
    neuralstate.cpp \
    neuroomega.cpp \
    ns2dbs.cpp \
    nsdbs.cpp \
    resampling.cpp \
    alphalabsnr.cpp \
    lfpchart.cpp \
    aotest.cpp \
    lib/wavelib/conv.c \
    lib/wavelib/cwt.c \
    lib/wavelib/cwtmath.c \
    lib/wavelib/hsfft.c \
    lib/wavelib/real.c \
    lib/wavelib/wavefilt.c \
    lib/wavelib/wavefunc.c \
    lib/wavelib/wavelib.c \
    lib/wavelib/wtmath.c \
    thradbs.cpp

HEADERS += \
    amplmodulation.h \
    cleandbs.h \
    closedloop.h \
    devicefactory.hpp \
    dualthresholds.h \
    lib/cascadeiir/biquad.h \
    lib/cascadeiir/cascadeiir.h \
    lib/cascadeiir/mathlib.h \
    lib/cascadeiir/state.h \
    lib/kissfft/_kiss_fft_guts.h \
    lib/kissfft/kiss_fft.h \
    lib/kissfft/kiss_fft_log.h \
    lib/kissfft/kiss_fftr.h \
    mainwindow.h \
    neuralstate.h \
    neuroomega.h \
    ns2dbs.h \
    nsdbs.h \
    resampling.h \
    alphalabsnr.h \
    lfpchart.h \
    aotest.h \
    stimchanconfig.hpp \
    lib/wavelib/conv.h \
    lib/wavelib/cwt.h \
    lib/wavelib/cwtmath.h \
    lib/wavelib/hsfft.h \
    lib/wavelib/real.h \
    lib/wavelib/wavefilt.h \
    lib/wavelib/wavefunc.h \
    lib/wavelib/wavelib.h \
    lib/wavelib/wtmath.h \
    stimdevice.hpp \
    stimstrategy.h \
    strategyfactory.hpp \
    thradbs.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    src.qrc

LIBS += \
    Iphlpapi.lib

INCLUDEPATH += \
    ./lib \
    ./lib/wavelib \
    ./lib/iir1-1.7.5
