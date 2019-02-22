#-------------------------------------------------
#
# Project created by QtCreator 2019-02-13T14:26:45
#
#-------------------------------------------------

QT       += core gui printsupport serialport quick qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = oym8CHWave
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += qcustomplot


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    dialogconnect.cpp \
    GfThread/gfthread.cpp \
    gfsdkqml.cpp

HEADERS += \
        mainwindow.h \
    qcustomplot/qcustomplot.h \
    dialogconnect.h \
    GfThread/gfthread.h \
    gfsdkqml.h \
    third_party/gforce/inc/gforce.h \
    CircleBuff.h

FORMS += \
        mainwindow.ui \
    dialogconnect.ui


# -----------------------------------------
# add third party dependency, relative path
# 1. '.h' file
#------------------------------------------
# add gforce sdk dependency
INCLUDEPATH += $$PWD/third_party/gforce/inc

# -----------------------------------------
# 2. '.lib' file
#------------------------------------------
# add gforce lib
INCLUDEPATH += $$PWD/third_party/gforce
DEPENDPATH += $$PWD/third_party/gforce

#LIBS += $$PWD/third_party/gforce/gforce64.lib
win32: LIBS += -L$$PWD/third_party/gforce/

CONFIG(debug, debug|release) {
    LIBS += -lgforce64d
    message("debug")
} else {
    LIBS += -lgforce64
    message("release")
}

DISTFILES +=

RESOURCES += \
    image.qrc \
    qdarkstyle/style.qrc





