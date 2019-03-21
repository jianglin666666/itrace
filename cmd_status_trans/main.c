#include "server.h"
#include "test.h"
#include <stdio.h>

enum SystemStatus system_status = Running;
sem_t *g_sem;

volatile int cmd_status_fd;
void sig_handler(int sig_no)
{
    switch(sig_no)
    {
        case SIGINT:
        {
            char write_cmd = 0xff;
            printc( MSG_WARN, "recv SIGINT signal, exit process\b");
            writen_to_cmd_status_dev(cmd_status_fd, write_cmd, sizeof(write_cmd));
            system_status = Stop;
            sem_post(g_sem);
            signal(SIGINT, sig_handler);
            break;
        }
        case SIGPIPE:
        {
            printc(MSG_WARN, "bad pipe\n");
            signal(SIGPIPE, sig_handler);
            break;
        }
        default:
        {
            printc(MSG_WARN, "unknow signal... no is %d\n", sig_no);
        }
    }
}

int main(int argc, char *argv[])
{

    pthread_t thread_id = 0;
    if(pthread_create(&thread_id, NULL, (void *)cmd_status_trans_rw_test, NULL) != 0)
    {
        printc(MSG_ERROR, "create cmd_status_trans_rw_test failed!. exit\n");
        return;
    }
    while(1)
    {
        pause();
    }

    printc(MSG_INFO, "Version: %s\n", iCTS_Arm_Version);
    printc(MSG_INFO, "open cmd status trans server...\n");
    if(argc != 2)
    {
        printc(MSG_ERROR, "not find IP no, quit\n");
        return 0;
    }
    int ip_no = atoi(argv[1]);
    if((ip_no < 80) || (ip_no >= 255))
    {
        printc(MSG_ERROR, "get invaild ip address, quit\n");
        return 0;
    }
    printc(MSG_INFO, "open acq data trans server...\n");
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);
    g_sem = (sem_t *)malloc(sizeof(sem_t));
    if(sem_init(g_sem, 0, 0) < 0)
    {
        perror("init sem failed. ");
        return -1;
    }

    SocketInfo server_info = {"local", 9100, "cmd_status", ip_no};
    create_cmd_status_trans_server(&server_info);
    while(1)
    {
        pause();
    }
    return 0;
}
