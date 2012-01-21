#-------------------------------------------------
#
# Project created by QtCreator 2011-11-30T22:30:58
#
#-------------------------------------------------

QT       += core gui network

TARGET = tmChatEric
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    chatrooms.cpp \
    chatroom.cpp \
    users.cpp \
    user.cpp \
    chatroomusers.cpp \
    chatroomuser.cpp \
    client.cpp \
    dataelement.cpp \
    chatsocket.cpp \
    clientchatrooms.cpp \
    clientchatroom.cpp \
    chatroomwindow.cpp \
    createchatroomdialog.cpp \
    dataelementviewer.cpp

HEADERS  += mainwindow.h \
    server.h \
    chatrooms.h \
    chatroom.h \
    users.h \
    user.h \
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
    dataelementviewer.h

FORMS    += mainwindow.ui \
    chatroomwindow.ui \
    createchatroomdialog.ui \
    dataelementviewer.ui
