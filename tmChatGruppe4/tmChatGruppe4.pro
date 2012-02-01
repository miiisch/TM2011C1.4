#-------------------------------------------------
#
# Project created by QtCreator 2011-11-30T22:30:58
#
#-------------------------------------------------

QT       += core gui network

TARGET = tmChatEric
TEMPLATE = app

CXXFLAGS += -Wall -Werror

#CONFIG += link_pkgconfig
#PKGCONFIG += libnotify
#PKGCONFIG += libglib

#INCLUDEPATH += "/usr/include/glib-2.0"
#INCLUDEPATH += "/usr/include/gtk-2.0"
#INCLUDEPATH += "/usr/lib/gtk-2.0/include"
#INCLUDEPATH += "/usr/include/cairo"
#INCLUDEPATH += "/usr/include/pango-1.0"
#INCLUDEPATH += "/usr/include/gdk-pixbuf-2.0"
#INCLUDEPATH += "/usr/include/atk-1.0"
#LIBS += /usr/lib/libnotify.so


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    chatrooms.cpp \
    chatroom.cpp \
    users.cpp \
    user.cpp \
    commandline.cpp \
    chatroomusers.cpp \
    chatroomuser.cpp \
    client.cpp \
    dataelement.cpp \
    chatsocket.cpp \
    clientchatrooms.cpp \
    clientchatroom.cpp \
    chatroomwindow.cpp \
    createchatroomdialog.cpp \
    dataelementviewer.cpp \
    commandprocessor.cpp \
    stdinreader.cpp

HEADERS  += mainwindow.h \
    server.h \
    chatrooms.h \
    chatroom.h \
    users.h \
    user.h \
    commandline.h \
    chatroomusers.h \
    chatroomuser.h \
    client.h \
    dataelement.h \
    chatsocket.h \
    clientchatrooms.h \
    clientchatroom.h \
    chatroomwindow.h \
    types.h \
    createchatroomdialog.h \
    dataelementviewer.h \
    commandprocessor.h \
    stdinreader.h

FORMS    += mainwindow.ui \
    chatroomwindow.ui \
    createchatroomdialog.ui \
    dataelementviewer.ui
