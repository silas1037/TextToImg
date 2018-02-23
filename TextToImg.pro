#-------------------------------------------------
#
# Project created by QtCreator 2017-08-09T17:07:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TextToImg
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        TextToImg.cpp

HEADERS  += TextToImg.h

FORMS    += TextToImg.ui

RC_ICONS += conversion.ico

RESOURCES += \
    res.qrc
