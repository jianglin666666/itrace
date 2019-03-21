TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
# 增加线程库
LIBS += -lpthread

SOURCES += main.c \
    linux_thread.c

HEADERS += \
    linux_thread.h
