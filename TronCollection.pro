# The Tron Collection: Classic Tron and Super Tron
# Build with: qmake && make
QT += core gui widgets
CONFIG += c++17
TARGET = TronCollection
TEMPLATE = app

SOURCES += \
    main.cpp \
    qttronaboutdialog.cpp \
    qttronchartdialog.cpp \
    qttroncollectionmenudialog.cpp \
    qttrongamedialog.cpp \
    qttrongraphics.cpp \
    qttronmenudialog.cpp \
    qttronpresskeydialog.cpp \
    qttronselectcolordialog.cpp \
    trongame.cpp

HEADERS += \
    qttronaboutdialog.h \
    qttronchartdialog.h \
    qttroncollectionmenudialog.h \
    qttrongamedialog.h \
    qttrongraphics.h \
    qttronmenudialog.h \
    qttronpresskeydialog.h \
    qttronselectcolordialog.h \
    trongame.h

RESOURCES += TronCollection.qrc

OTHER_FILES += Licence.txt README.md
