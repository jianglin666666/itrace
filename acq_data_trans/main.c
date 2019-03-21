#include "server.h"
#include "test.h"
#include <stdio.h>

enum SystemStatus system_status = Running;
sem_t *g_sem;
//catch signal
void sig_handler(int sig_no)
{
    switch(sig_no)
    {

        case SIGINT:
        {
            printc(MSG_WARN, "recv SIGINT signal, exit process\b");
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
    if(pthread_create(&thread_id, NULL, (void *)read_dma_test, NULL) != 0)
    {
        printc(MSG_ERROR, "create cmd_and_status_channel_rw_loop_test failed!. exit\n");
        return;
    }
    while(1)
    {
        pause();
    }

    print_log();
    printc(MSG_INFO, "Version: %s\n", iCTS_Arm_Version);
    printc(MSG_INFO, "open acq data trans server...111\n");
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);
    g_sem = (sem_t *)malloc(sizeof(sem_t));
    if(sem_init(g_sem, 0, 0) < 0)
    {
        perror("init sem failed. ");
        return -1;
    }

    SocketInfo server_info = {"local", 9200, "acq_server", 0};
    create_acq_data_trans_server(&server_info);
    while(1)
    {
        pause();
    }
    return 0;
}
