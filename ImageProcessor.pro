#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T10:18:03
#
#-------------------------------------------------

QT       += core gui
QT       +=printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageProcessor
TEMPLATE = app


SOURCES += main.cpp\
        imgprocessor.cpp \
    showwidget.cpp

HEADERS  += imgprocessor.h \
    showwidget.h

RESOURCES += \
    res.qrc
