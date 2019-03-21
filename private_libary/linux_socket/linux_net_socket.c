#include "linux_net_socket.h"
#include <netinet/tcp.h>

int create_clt_netsock(const SocketInfo *socket_info)
{
    char clt_name_[128];
    if(socket_info->desc)
    {
        sprintf(clt_name_, "%s%c", socket_info->desc, 0);
    }else{
        sprintf(clt_name_, "%s:%d%c", socket_info->ip, socket_info->port, 0);
    }

    int clt_sockfd = 0;

    struct sockaddr_in srvaddr;

    // 创建套接字
    if( (clt_sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        printc(MSG_FATAL, "%s: craete client socket failed! exit...\n", clt_name_);
        close(clt_sockfd);
        return(-1);
    }

    // 设置套接字地址结构
    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family=AF_INET;
    srvaddr.sin_port = htons(socket_info->port);
    // 新函数inet_pton，与inet_addr()功能一致
    if(inet_pton(AF_INET, socket_info->ip, &srvaddr.sin_addr) < 0)
    {
        printc(MSG_FATAL, "%s: inet pton fatal. exit...\n", clt_name_);
        return -1;
    }

    // 最多重试连接15次，每次1s
    int try_count = 0;
CONNECT_AGAIN:

    printc(MSG_INFO, "%s: try to connect ip: %s, port: %d.\n", clt_name_, socket_info->ip, socket_info->port);
    if( connect(clt_sockfd, (struct sockaddr*)&srvaddr, sizeof(struct sockaddr)) <0 )
    {
        printc(MSG_ERROR, "%s: connect server failed! Left trying connect times is %d.\n", clt_name_ ,MAX_TRY_CONN_TIMES - try_count);
        sleep(1);
        try_count++;
        if(MAX_TRY_CONN_TIMES == try_count)
        {
            printc(MSG_FATAL, "%s: Stop to trying to connect server. ip: %s, port: %d\n", clt_name_, socket_info->ip, socket_info->port);
            close(clt_sockfd);
            return -1;
        }
        goto CONNECT_AGAIN;
    }

    printc(MSG_INFO, "%s: connect server %s:%d success!\n", clt_name_, socket_info->ip, socket_info->port);
    return(clt_sockfd);
}

int create_srv_netsock(const int port, const char *log_name)
{
    char srv_name[128] = {0};

    if(log_name)
    {
        sprintf(srv_name, "%s%c", log_name, 0);
    }else{
        sprintf(srv_name, "%s:%d%c", "localhost", port, 0);
    }

    int srv_sockfd = 0;
    int SRV_BACKLOG=20;

    struct sockaddr_in srvaddr;

    if( (srv_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        printc(MSG_FATAL, "%s: create server socket failed.\n", srv_name);
        perror("       ");
        close(srv_sockfd);
        return(-1);
    }

    bzero(&srvaddr, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    srvaddr.sin_port = htons(port);
    const int ON = 1;
    setsockopt(srv_sockfd, SOL_SOCKET, SO_REUSEADDR, &ON, sizeof(ON));
    // 无延迟发送
    setsockopt(srv_sockfd, IPPROTO_TCP, TCP_NODELAY, &ON, sizeof(ON));

    int try_bind_count = 0;
BIND_AGAIN:
    if( bind(srv_sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1 )
    {
        try_bind_count++;
        if(try_bind_count == MAX_TRY_BIND_TIMES)
        {
            printc(MSG_FATAL, "%s: stop try bind port %d, exit...\n", srv_name, port);
            return -1;
        }
        printc(MSG_ERROR, "%s: try to bind port %d failed, current try times %d(max: %d)\n", srv_name, port, try_bind_count, MAX_TRY_BIND_TIMES);
        goto BIND_AGAIN;
    }
    return(srv_sockfd);
}

int create_netsock_server(const SocketInfo *socket_info, int (*protocol)( const int *sockfd, HeadFrame *hf, void *buf) )
{

    char socket_desc[128];
    if(!socket_info)
    {
        printc(MSG_FATAL, "get wrong socket info when trying create net socket server.\n");
        return -1;
    }

    if(strlen(socket_info->desc))
    {
        sprintf(socket_desc, "%s%c", socket_info->desc, 0);
    }else{
        sprintf(socket_desc, "%s:%d:%c", socket_info->ip, socket_info->port, 0);
    }

    int listen_fd = -1;
    if((listen_fd = create_srv_netsock(socket_info->port, socket_info->desc)) < 0)
    {
        printc(MSG_FATAL, "%s: creat server net socket failed. exit...\n", socket_desc);
        return -1;
    }

    const int max_back_log = MAX_TRY_BIND_TIMES;
    int clt_addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in clt_addr;
    int clt_socket_fd = -1;
    HeadFrame hf;
    int read_len = 0;
    int conn_fd = -1;

    int max_data_entity_len = 1024;
    char *data_entity = (char *)malloc(sizeof(char) * max_data_entity_len);

LISTEN_AGAIN:
    // 如果已经打开的情况下仍有连接进入，应该关闭之前的
    if(conn_fd > 0)
    {
        shutdown(conn_fd, SHUT_RDWR);
        conn_fd = -1;
    }
    if( listen(listen_fd, max_back_log) == -1 )
    {
        printc(MSG_FATAL, "%s: listen failed. exit...\n", socket_desc);
        return -1;
    }

ACCEPT_AGAIN:
    if( (conn_fd=accept(listen_fd, (struct sockaddr *)&clt_addr, &clt_addr_len)) == -1 )
    {
        printc(MSG_FATAL, "%s: accept new connect failed. exit...\n", socket_desc);
        perror("       ");
        return -1;
    }
    printc(MSG_INFO, "%s: get connecting from %s:%d \n", socket_info->desc, inet_ntoa(clt_addr.sin_addr), socket_info->port);

    while(1)
    {
        if((read_len = read_head_frame(conn_fd, &hf)) != sizeof(HeadFrame))
        {
            printc(MSG_ERROR, "%s: receive incomplete head frame, conn lost, relisten... \n", socket_desc);

            if(conn_fd)
            {
                close(conn_fd);
            }
            goto LISTEN_AGAIN;
        }

        if(hf.len> 0)
        {
            if(hf.len > max_data_entity_len)
            {
                data_entity = realloc(data_entity, hf.len);

                if(data_entity == NULL)
                {
                    close(conn_fd);
                    close(listen_fd);
                    printc(MSG_FATAL, "%s: application memory for entity data failed. curr: %d bytes, try to get %d bytes. exit...\n",
                           socket_desc, max_data_entity_len, hf.len);
                    return -1;
                }
            }

            if(read_data(conn_fd, (void *)data_entity, hf.len) != hf.len)
            {
                printc(MSG_ERROR, "%s: receive incomplete head frame, conn lost, relisten... \n", socket_desc);

                if(conn_fd)
                {
                    close(conn_fd);
                }
                goto LISTEN_AGAIN;
            }

            protocol(&conn_fd, &hf, data_entity);
        }
    }

    int fd_count = 2;
    close_fd(fd_count, &conn_fd, &listen_fd);
    return 0;
}


