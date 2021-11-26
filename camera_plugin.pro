QT -= gui

TEMPLATE = lib
DEFINES += PLUGINGAZEBO_LIBRARY

CONFIG += c++11

SOURCES += \
        cameraplugin.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gazebo

INCLUDEPATH += /usr/include/OGRE
INCLUDEPATH += /usr/include/OGRE/Paging

HEADERS += \
    cameraplugin.h
