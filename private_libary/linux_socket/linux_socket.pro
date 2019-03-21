TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += main.c \
    ../linux_thread/linux_thread.c \
    socket_protocol.c \
    linux_net_socket.c

HEADERS += \
    ../linux_thread/linux_thread.h \
    socket_protocol.h \
    linux_net_socket.h
