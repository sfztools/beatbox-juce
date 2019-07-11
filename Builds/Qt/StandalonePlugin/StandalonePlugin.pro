#-------------------------------------------------
#
# Project created by QtCreator 2019-06-14T05:33:33
#
#-------------------------------------------------
#QT      += core gui
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT        -= core gui
TEMPLATE   = app
TARGET     = BeatBox
CONFIG    += link_pkgconfig
PKGCONFIG += alsa freetype2 x11 xext xinerama webkit2gtk-4.0 gtk+-x11-3.0 libcurl
QMAKE_CXXFLAGS += -std=c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
           LINUX=1 DEBUG=1 _DEBUG=1 JUCE_APP_VERSION=1.0.0 \
           JUCE_APP_VERSION_HEX=0x10000 \
           JucePlugin_Build_VST=0 JucePlugin_Build_VST3=0 JucePlugin_Build_AU=0 \
           JucePlugin_Build_AUv3=0 JucePlugin_Build_RTAS=0 JucePlugin_Build_AAX=0 \
           JucePlugin_Build_Standalone=1 JucePlugin_Build_Unity=0

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/freetype2 $$_PRO_FILE_PWD_/../../../Source \
               $$_PRO_FILE_PWD_/../../../JuceLibraryCode $(HOME)/JUCE/modules
LIB_BUILDDIR = $$OUT_PWD/../SharedCode
LIBS        += -m64 -ldl -lpthread -lrt -L"$$LIB_BUILDDIR" -lBeatBox
QMAKE_CFLAGS = -Wall -Wno-strict-aliasing -Wno-strict-overflow \
               -m64 -O0 -mstackrealign -fPIC -g

SOURCES += ./../../../Source/PluginEditor.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
