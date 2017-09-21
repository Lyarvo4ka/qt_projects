TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = iolib \
          iolibTest \
          qt_console \
          #ioTest

iolibTest.depends = iolib
