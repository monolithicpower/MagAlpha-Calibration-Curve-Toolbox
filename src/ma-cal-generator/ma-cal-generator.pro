QT += core
QT -= gui

CONFIG += c++11

TARGET = ma-cal-generator
DESTDIR = "../../bin"
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    ../calibration-curve-generator

SOURCES += main.cpp \
    ../calibration-curve-generator/calibrationcurvegenerator.c

HEADERS += \
    ../calibration-curve-generator/calibrationcurvegenerator.h

