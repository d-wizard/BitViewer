#-------------------------------------------------
#
# Project created by QtCreator 2012-07-06T18:19:39
#
#-------------------------------------------------

QT       += core gui

TARGET = QTBitViewer
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    IOUtilities.cpp \
    bitOperations.cpp \
    dString.cpp \
    FileSystemOperations.cpp \
    BitViewerIni.cpp \
    AutoDelimiter.cpp \
    bitViewerGuiTab.cpp \
    bitStreamCompare.cpp \
    bitViewerData.cpp \
    tabcomparedialog.cpp

HEADERS  += mainwindow.h \
    DataTypes.h \
    IOUtilities.h \
    bitOperations.h \
    dString.h \
    FileSystemOperations.h \
    hist.h \
    BitViewerIni.h \
    AutoDelimiter.h \
    bitViewerGuiTab.h \
    bitStreamCompare.h \
    bitViewerData.h \
    tabcomparedialog.h

FORMS    += mainwindow.ui \
    tabcomparedialog.ui

win32:RC_FILE = QTBitViewer.rc
QT += widgets

RESOURCES += \
    bitViewerResource.qrc
