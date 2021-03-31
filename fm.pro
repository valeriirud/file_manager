
QT_PATH = /usr/local/qt/Qt5.14.1/5.14.1/gcc_64
QT_LIBRARY_PATH = $$QT_PATH/lib

INCLUDEPATH = $$QT_PATH/include
INCLUDEPATH += .

LIBS = $$QT_LIBRARY_PATH/libQt5Gui.so 
LIBS += $$QT_LIBRARY_PATH/libQt5Core.so 
LIBS += $$QT_LIBRARY_PATH/libQt5Widgets.so

TEMPLATE = app

CONFIG += c++1z

QT += core
QT += widgets

TARGET = fm

HEADERS += MainWindow.h ExecThread.h FileNameDialog.h
SOURCES += main.cpp MainWindow.cpp ExecThread.cpp FileNameDialog.cpp

