#-------------------------------------------------
#
# Project created by QtCreator 2017-05-22T15:39:03
#
#-------------------------------------------------

QT -= core gui

TARGET = iolib
TEMPLATE = lib
CONFIG += staticlib c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += iolib.cpp \
    factories.cpp \
    entropy.cpp \
    iolib.cpp \
    iofs.cpp

HEADERS += iolib.h \
    physicaldrive.h \
    constants.h \
    utility.h \
    iodevice.h \
    abstractraw.h \
    quicktimeraw.h \
    rawmts.h \
    factories.h \
    standartraw.h \
    finder.h \
    iofs.h \
    rawavi.h \
    rawmfx.h \
    rawmpeg.h \
    rawriff.h \
    dbf.h \
    entropy.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

BOOST_ROOT = $$(BOOST_ROOT)
INCLUDEPATH += $$BOOST_ROOT
LIBS += -L$${BOOST_ROOT}/stage/lib
TARGET = ../build/iolib
