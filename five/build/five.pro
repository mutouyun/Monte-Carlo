#-------------------------------------------------
#
# Project created by QtCreator 2020-03-16T17:42:27
#
#-------------------------------------------------

CONFIG -= qt

TARGET = five
TEMPLATE = lib

DEFINES += __FIVE_LIBRARY__

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -std=c++1z

INCLUDEPATH += \
        ../include \
        ../src

HEADERS += \
        ../include/five.h \
        ../include/five_global.h \
        ../src/def.h \
        ../src/coord.h \
        ../src/steps.h \
        ../src/layer.h \
        ../src/board.h \
        ../src/pool.h \
        ../src/async.h

SOURCES += \
        ../src/five.cpp

DESTDIR = ../build/bin

LIBS += -lpthread

unix {
    target.path = /usr/lib
    INSTALLS += target
}
