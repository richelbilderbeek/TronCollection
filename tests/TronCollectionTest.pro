# Unit tests of the game rules
# Build and run with: qmake && make && ./TronCollectionTest
QT += core gui testlib
QT -= widgets
CONFIG += c++17 console testcase
CONFIG -= app_bundle
TARGET = TronCollectionTest
TEMPLATE = app

INCLUDEPATH += ..
SOURCES += test_trongame.cpp ../trongame.cpp
HEADERS += ../trongame.h
