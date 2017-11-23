QT += core
QT -= gui

CONFIG += c++11

TARGET = ma-cal-generator
DESTDIR = "../../bin"
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    ../calibration-curve-generator \
    ../angle-interpolation

SOURCES += main.cpp \
    ../calibration-curve-generator/calibrationcurvegenerator.c \
    ../angle-interpolation/angleinterpolation.c

HEADERS += \
    ../calibration-curve-generator/calibrationcurvegenerator.h \
    ../angle-interpolation/angleinterpolation.h

