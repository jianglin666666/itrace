#ifndef SERVER_H
#define SERVER_H
#include "../general/general.h"
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

int open_cmd_status_device(const char *dev_path);

int write_ip_to_device(const char *dev_path);

void create_cmd_status_trans_server(SocketInfo *server_info);

int reset_hardware(int cmd_status_fd);

int set_gtx_rx_ch_enable(int fd, int open);

struct CmdStatusRWThreadParams
{
    int socket_fd;
    int dev_fd;
    enum SystemStatus *sys_status;
};

void read_net_cmd_thread(struct CmdStatusRWThreadParams *params);


void send_status_data_thread(struct CmdStatusRWThreadParams *params);

int writen_to_cmd_status_dev(int fd, char *data, int len);

struct ProcSource
{
    enum SystemStatus *sys_status;
    pid_t child_pid;
    int fd[20];
    char *mem[20];
    char desc[20];
};

void init_proc_source(struct ProcSource *source);
void clear_thread(struct ProcSource *sour);

#endif // SERVER_H
