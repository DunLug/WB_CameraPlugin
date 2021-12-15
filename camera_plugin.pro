QT -= gui

TEMPLATE = lib
DEFINES += PLUGINGAZEBO_LIBRARY

CONFIG += c++11

SOURCES += \
        cameraplugin.cpp \
        networkstreamer.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gazebo
unix: PKGCONFIG += libvlc
unix: PKGCONFIG += gstreamer-1.0

INCLUDEPATH += /usr/include/OGRE
INCLUDEPATH += /usr/include/OGRE/Paging

HEADERS += \
    cameraplugin.h \
    networkstreamer.h
