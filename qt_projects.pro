TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = iolib \
          iolibTest \
    qt_console

iolibTest.depends = iolib
