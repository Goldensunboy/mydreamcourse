#-------------------------------------------------
#
# Project created by QtCreator 2012-11-01T12:45:38
#
#-------------------------------------------------

QT       += core gui

TARGET = MyDreamCourse
TEMPLATE = app


SOURCES += main.cpp\
        mdcwindow.cpp \
    fileio.cpp \
    codec.cpp \
    levelhandler.cpp

HEADERS  += mdcwindow.h \
    defs.h \
    fileio.h \
    codec.h \
    levelhandler.h

FORMS    += mdcwindow.ui

OTHER_FILES += \
    MDCWindow.qml

RESOURCES += \
    GFXResource.qrc
