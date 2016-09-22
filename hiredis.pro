TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lhiredis
QMAKE_CXXFLAGS += -std=c++11
SOURCES += main.cpp \
    redis_context.cpp \
    redis_status.cpp

HEADERS += \
    redis_context.h \
    redis_status.h \
    utils.h

