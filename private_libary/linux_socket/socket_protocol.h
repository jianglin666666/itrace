#ifndef SOCKET_PROTOCOL_H
#define SOCKET_PROTOCOL_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <strings.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <stddef.h>
#include <stdarg.h>


#define MAX_TRY_CONN_TIMES  15
#define MAX_TRY_BIND_TIMES  15
#define MAX_BACK_LOG  20

struct net_socket_info{
    char ip[64];
    unsigned int port;
    char desc[128];
    int rsv;
};
typedef struct net_socket_info SocketInfo;

void init_socket_info(SocketInfo *socket_info, const char *ip, const unsigned int port, const char *socket_desc);


struct head_frame_st{
    unsigned int id;
    unsigned int len;
    unsigned int flag;
    unsigned int res;
};
typedef struct head_frame_st HeadFrame;

void init_head_frame(HeadFrame *head_frame, int id, int flag, int res, int len);

int read_head_frame(const int fd, HeadFrame *hf);

int read_data(const int fd, void *buf, const unsigned int max_buf_len);

int write_data(const int fd, const void *buf, const int buf_len);

int close_fd(const int fd_num, ...);

#endif // SOCKET_PROTOCOL_H
