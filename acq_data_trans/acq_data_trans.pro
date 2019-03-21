TEMPLATE = app
CONFIG += console
#QMAKE_CFLAGS += -std=c99
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lpthread

SOURCES += main.c \
    $$PWD/../private_libary/cstring_extern_lib/cstring_extern_lib.c \
#    $$PWD/../private_libary/linux_serial_port/linux_serial_port.c \
    $$PWD/../private_libary/linux_socket/linux_net_socket.c \
    $$PWD/../private_libary/linux_socket/socket_protocol.c \
    $$PWD/../private_libary/linux_thread/linux_thread.c \
    $$PWD/../private_libary/time_extern_lib/time_extern_lib.c \
    server.c \
    ../general/general.c \
    test.c


HEADERS += \
    $$PWD/../private_libary/cstring_extern_lib/cstring_extern_lib.h \
#    $$PWD/../private_libary/linux_serial_port/linux_serial_port.h \
    $$PWD/../private_libary/linux_socket/linux_net_socket.h \
    $$PWD/../private_libary/linux_socket/socket_protocol.h \
    $$PWD/../private_libary/linux_thread/linux_thread.h \
    $$PWD/../private_libary/log_type/console_color.h \
    $$PWD/../private_libary/log_type/log_type.h \
    $$PWD/../private_libary/time_extern_lib/time_extern_lib.h \
    server.h \
    ../general/general.h \
    test.h
