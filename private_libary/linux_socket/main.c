#include "linux_net_socket.h"
#include "../linux_thread/linux_thread.h"
#include <stdio.h>

SocketInfo sock_info;

int protocol( const int *sockfd, const HeadFrame *hf, void *buf)
{
    printc(MSG_INFO, "\n==================++++++++++++++++++++====\n");
    printc(MSG_INFO, "id: %x\n", hf->id);
    printc(MSG_INFO, "flag: %x\n", hf->flag);
    printc(MSG_INFO, "len: %x\n", hf->len);
    printc(MSG_INFO, "res: %x\n", hf->res);
    printc(MSG_INFO, "Data: %s\n", (char *)buf);
    return 0;
}

void open_server()
{
    create_netsock_server(&sock_info, protocol);
}

void open_clt()
{
    int fd = create_clt_netsock(&sock_info);
    if(fd < 0)
        return;

    HeadFrame hf;
    char msg[128];
    for(int i = 0; i < 20; i++)
    {
        sprintf(msg, "HHHHHHello: %02d\n%c", i, 0);
        init_head_frame(&hf, i, i+1, i + 2, strlen(msg));
        printc(MSG_INFO, "Clt: send: %s, len: %d\n", msg, strlen(msg));

        printc(MSG_INFO, "Head: %d\n", write_data(fd, &hf, sizeof(HeadFrame)));
        printc(MSG_INFO, "MSG: %d\n", write_data(fd, msg, strlen(msg)));
        sleep(1);
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    init_socket_info(&sock_info, "127.0.0.1", 9100, "net_test");

    if(create_detached_pthread(&open_server, NULL) == 0)
    {
        printc(MSG_INFO, "Open Srv\n");
    }else{
        return -1;
    }

    if(create_detached_pthread(open_clt, NULL) == 0)
    {
        printc(MSG_INFO, "Open Clt\n");
    }else{
        return -1;
    }

    while(1)
    {
        sleep(2000);
    }
    return 0;
}
