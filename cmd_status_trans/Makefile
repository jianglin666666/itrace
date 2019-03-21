# static 版本只能在arm-linux-gnueabihf-gcc下可以运行
ARM_GCC     = arm-linux-gnueabihf-gcc
ARM_EXE    	= icts_arm_cmd_status_server.exe
GCC			= gcc
EXE			= icts_cmd_status_server.exe
LIB        	= -lpthread
COMP_STATIC = -static

SRC_FILE    = main.c \
        ../private_libary/cstring_extern_lib/cstring_extern_lib.c \
        ../private_libary/linux_thread/linux_thread.c \
        ../private_libary/time_extern_lib/time_extern_lib.c \
        ../private_libary/linux_socket/linux_net_socket.c \
        ../private_libary/linux_socket/socket_protocol.c \
        ../general/general.c \
		server.c \
		test.c

HEAD_FILE = \
        ../private_libary/cstring_extern_lib/cstring_extern_lib.h \
        ../private_libary/linux_thread/linux_thread.h \
        ../private_libary/log_type/console_color.h \
        ../private_libary/log_type/log_type.h \
        ../private_libary/time_extern_lib/time_extern_lib.h \
        ../private_libary/linux_socket/linux_net_socket.h \
        ../private_libary/linux_socket/socket_protocol.h \
        ../general/general.h \
        server.h  \
		test.h

all: $(ARM_EXE) $(EXE)

$(ARM_EXE): $(SRC_FILE) $(HEAD_FILE)
	$(ARM_GCC) -Wall -o $@ $^ $(LIB) $(COMP_STATIC) -std=c99
$(EXE): $(SRC_FILE) $(HEAD_FILE)
	$(GCC) -Wall -o $@ $^ $(LIB) -std=c99
clean:
	rm -f $(ARM_EXE)
	rm -f $(EXE)
