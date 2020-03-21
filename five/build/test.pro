TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../include

SOURCES += \
        ../test/main.cpp

DESTDIR = ../build/bin

LIBS += -L$${DESTDIR} -lfive
