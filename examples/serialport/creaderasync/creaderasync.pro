QT = core

include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)

CONFIG += console
CONFIG -= app_bundle

TARGET = creaderasync
TEMPLATE = app

HEADERS += \
    serialportreader.h

SOURCES += \
    main.cpp \
    serialportreader.cpp
