#ifndef SERVER_H
#define SERVER_H
#include "../general/general.h"
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

int open_dma_device(const char *dev_path);

void create_acq_data_trans_server(SocketInfo *server_info);

struct DmaBuffer
{
    int index;
    char *data;
    int length;
    int capity;
};

struct DmaBuffer* init_dma_read_buffer(int index, char *start_addr);

void clear_dma_dev_sour(char *mmap_data, int *fd);

struct ReadDmaThreadParams
{
    struct DmaBuffer *buff_list[2];
    int dma_index_r_fd;
    int socket_index_w_fd;
    int dma_fd;
    enum SystemStatus *sys_status;
};

void read_dma_thread(struct ReadDmaThreadParams *params);

struct SendAcqDataThreadParams
{
    struct DmaBuffer *buff_list[2];
    int dma_index_w_fd;
    int socket_index_r_fd;
    int socket_fd;
    enum SystemStatus *sys_status;
};
void send_acq_data_thread(struct SendAcqDataThreadParams *params);

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
