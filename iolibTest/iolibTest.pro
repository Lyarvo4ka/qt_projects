TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += "../iolib/"
INCLUDEPATH += $$(GTEST_DIR)/include

INCLUDEPATH += $$(BOOST_ROOT)
#win32 {
LIBS += -L$$(BOOST_ROOT)/stage/lib \
            -lboost_serialization-mgw63-mt-d-1_64 \
            -lboost_filesystem-mgw63-mt-d-1_64 \
            -lboost_system-mgw63-mt-d-1_64
#}
LIBS +=-lgtest -L$$(GTEST_DIR)/lib
LIBS +=-lgmock -L$$(GTEST_DIR)/lib

#LIBS += -L../build/iolib/debug
#LIBS += -liolib -L"../build/iolib/build"
LIBS += "../../build/iolib/build/libiolib.a"

SOURCES += main.cpp \
   # ../../iolib.cpp \
    utilitytest.cpp \
    iodevicetest.cpp \
    physicaldrivetest.cpp \
    findertest.cpp \
    datetest.cpp

HEADERS += \
    fakefile.h \
    mockphysicaldrive.h \
    fakephysicaldrive.h
