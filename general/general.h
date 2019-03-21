#ifndef GENERAL_H
#define GENERAL_H

#include "../private_libary/cstring_extern_lib/cstring_extern_lib.h"
#include "../private_libary/linux_socket/linux_net_socket.h"
#include "../private_libary/log_type/log_type.h"
#include "../private_libary/time_extern_lib/time_extern_lib.h"
#include "../private_libary/linux_thread/linux_thread.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <semaphore.h>

#define ServerPort        9200

#define CmdStatusDevicePath    "/dev/ammcd_csr"
#define DmaDevicePath 		"/dev/dma_ctrl"
#define iCTS_Arm_Version "0.12.24"

#define PIPE_WRITE 1
#define PIPE_READ 0
#define MAX_DMA_LENGTH  0x400000
#define MAX_CSR_LENGTH  0x1000
#define IOCTL_MAGIC_NUM       0xF7
#define CONFIG_ADDROFFSET     _IOW(IOCTL_MAGIC_NUM, 11, int)

#define SET_SOFT_RESET        _IOW(IOCTL_MAGIC_NUM, 12, int)
#define CSR_IOCTL_MAGIC_NUM   0xF8
#define CSR_SET_SOFT_RESET    _IOW(CSR_IOCTL_MAGIC_NUM, 1, int)
#define CSR_SET_CTRL_REGISTER _IOW(CSR_IOCTL_MAGIC_NUM, 2, int)

#define PACKAGE_NUM       16       //the maximum packets number
#define PAGE_SIZE         4096     //one packet size is PAGE_SIZE (4KB)
#define DATA_BUFF_LEN     (PACKAGE_NUM * PAGE_SIZE)

//ioctl command
#define DRDMA_RING_MAGIC  0xF7
#define RDR_SCHEDULE_TL   _IO(DRDMA_RING_MAGIC, 4)              /* schedule tasklet */
#define GPIO_SOFT_RESET   _IO(DRDMA_RING_MAGIC, 11)             /* control gpio reg to generate software reset */

enum SystemStatus
{
    Running,
    Stop
};

//#define SIMU_MODE
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readn(int fd, void *vptr, size_t n);

int is_process_alive(pid_t target_pid);

void print_log();

#define SPM_START_IP 101

#endif // GENERAL_H
