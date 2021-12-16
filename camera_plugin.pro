QT -= gui

TEMPLATE = lib
DEFINES += PLUGINGAZEBO_LIBRARY

CONFIG += c++11

SOURCES += \
        cameraplugin.cpp \
        mjpg-streamer/mjpg-streamer-experimental/mjpg_streamer.c \
        mjpg-streamer/mjpg-streamer-experimental/plugins/output_http/httpd.c \
        mjpg-streamer/mjpg-streamer-experimental/plugins/output_http/output_http.c \
        mjpg-streamer/mjpg-streamer-experimental/utils.c \
        networkstreamer.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gazebo
unix: PKGCONFIG += gstreamer-1.0
unix: PKGCONFIG += opencv

INCLUDEPATH += /usr/include/OGRE
INCLUDEPATH += /usr/include/OGRE/Paging

HEADERS += \
    cameraplugin.h \
    mjpg-streamer/mjpg-streamer-experimental/mjpg_streamer.h \
    mjpg-streamer/mjpg-streamer-experimental/plugins/output_http/httpd.h \
    mjpg-streamer/mjpg-streamer-experimental/utils.h \
    networkstreamer.h
