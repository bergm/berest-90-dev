#-------------------------------------------------
#
# Project created by QtCreator 2011-01-26T13:40:41
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -std=c++0x
DEFINES += _UNICODE

QT -= core
QT -= gui

TARGET = berest
CONFIG += console
CONFIG += static
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
src/berest.h \
src/helper.h

SOURCES += \
src/main.cpp \
src/berest.cpp \
src/helper.cpp \
../../../../mongo/mongodb-mongo-ee36f50/client/mongo_client_lib.cpp

INCLUDEPATH += \
C:\boost \
C:\Users\michael\development\mongo\mongodb-mongo-ee36f50 \
C:\Users\michael\development\mongo\mongodb-mongo-ee36f50\third_party\pcre-7.4

LIBS += \
-LC:\boost\lib\vs2010_64 \
-lws2_32

