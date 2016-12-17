#-------------------------------------------------
#
# Project created by QtCreator 2016-12-03T09:29:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fish_select
TEMPLATE = app


SOURCES += \
    main.cpp\
    mainwindow.cpp \
    selectionmodel.cpp \
    fullimageview.cpp \
    selectiondetailsview.cpp \
    selectioncropview.cpp

HEADERS  += \
    mainwindow.h \
    selectionmodel.h \
    fullimageview.h \
    selectiondetailsview.h \
    selectioncropview.h

FORMS    += mainwindow.ui \
    selectiondetailsview.ui
