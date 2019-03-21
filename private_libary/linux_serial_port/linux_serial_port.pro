TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += main.c \
    linux_serial_port.c \
    ../linux_thread/linux_thread.c

HEADERS += \
    linux_serial_port.h \
    ../linux_thread/linux_thread.h
