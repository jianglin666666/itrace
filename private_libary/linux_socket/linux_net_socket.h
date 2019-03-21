#ifndef LINUX_SOCKET_H
#define LINUX_SOCKET_H

#include "../log_type/log_type.h"
#include "socket_protocol.h"

int create_clt_netsock(const SocketInfo *socket_info);
int create_srv_netsock(const int port, const char *log_name);
int create_netsock_server(const SocketInfo *socket_info, int (*protocol)(const int *conn_fd, HeadFrame *hf, void *data_entity) );


#endif // LINUX_SOCKET_H
