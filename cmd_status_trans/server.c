#include "server.h"
extern enum SystemStatus system_status;
extern sem_t *g_sem;

void create_cmd_status_trans_server(SocketInfo *server_info)
{
    int listen_fd = -1;
    int clt_addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in clt_addr;
    int conn_fd = -1;
    pid_t child_pid = 0;

    int has_clear_thread = 0;

    // check is spm or scm
    if(server_info->rsv >= SPM_START_IP)
    {
        int cmd_status_fd = -1;
#ifndef SIMU_MODE
        cmd_status_fd = open_cmd_status_device(CmdStatusDevicePath);
#else
        cmd_status_fd = 1;
#endif
        printc((cmd_status_fd > 0) ? MSG_INFO : MSG_ERROR, "open device %s %s\n", CmdStatusDevicePath, (cmd_status_fd > 0) ? "success" : "failed");
        if(cmd_status_fd > 0)
        {

            char ip_write_cmd[] = {0x16,
                                   0x20, 0x00, 0x00, 0x00,
                                   0x00, 0x00,
                                   0xff, 0xfd,
                                   0x0c, 0x00, 0x00, 0x00, 0x08, 0xb4, 0x00, 0x00, server_info->rsv&0xff, 0x00, 0x00, 0x00, 0x00,
                                   0x00};
#ifndef SIMU_MODE
            if(writen_to_cmd_status_dev(cmd_status_fd, ip_write_cmd, sizeof(ip_write_cmd)) != sizeof(ip_write_cmd))
            {
                printc(MSG_ERROR, "create_cmd_status_trans_server write ip to spm failed[fd: %d]. exit\n", cmd_status_fd);
            }else{
                printc(MSG_INFO, "create_cmd_status_trans_server has send ip[%d] frame to spm\n", server_info->rsv);
            }
            close(cmd_status_fd);
#endif
        }
        exit(0);
    }

    // create listen
    if((listen_fd = create_srv_netsock(server_info->port, server_info->desc)) < 0)
    {
        printc(MSG_FATAL, "%s: creat server net socket failed. exit...\n", server_info->desc);
        goto Exit;
    }

    printc(MSG_INFO, "%s server start listen...\n", server_info->desc);
    // try listen
LISTEN_AGAIN:
    if(listen(listen_fd, MAX_TRY_BIND_TIMES) < 0 )
    {
        printc(MSG_FATAL, "%s: listen failed. exit...\n", server_info->desc);
        perror(" ");
        goto ExitCloseListenFd;
    }

ACCEPT_AGAIN:
    if( (conn_fd =
         accept(listen_fd, (struct sockaddr *)&clt_addr, &clt_addr_len)) < 0)
    {
        printc(MSG_FATAL, "%s: accept new connect failed. exit...\n", server_info->desc);
        perror("       ");
        if(has_clear_thread == 0)
        {
            if(system_status == Stop)
            {
                goto ExitCloseListenFd;
            }
        }
        if(listen_fd > 0)
            goto ACCEPT_AGAIN;
        else
            goto LISTEN_AGAIN;
    }
    printc(MSG_INFO, "%s server get new connect: %s\n",server_info->desc, inet_ntoa(clt_addr.sin_addr));

    // close child process
    if(child_pid > 0)
    {
        int child_proc_status = 0;
        kill(child_pid, SIGINT);
        wait(&child_proc_status);
        while(is_process_alive(child_pid) == 1)
        {
            printc(MSG_INFO, "child process[acq data send] is alive\n");
            usleep(200000);
        }
    }

    child_pid = fork();
    if(child_pid == 0)
    {
        // child
        close_fd(1, &listen_fd);

        int cmd_status_fd = -1;
#ifndef SIMU_MODE
        cmd_status_fd = open_cmd_status_device(CmdStatusDevicePath);
#else
        cmd_status_fd = 1;
#endif
        printc((cmd_status_fd > 0) ? MSG_INFO : MSG_ERROR, "open device %s %s\n", CmdStatusDevicePath, (cmd_status_fd > 0) ? "success" : "failed");
        if(cmd_status_fd < 0)
        {
            goto Exit;
        }
        //reset first
        //    reset_hardware(cmd_status_fd);
        //        if(set_gtx_rx_ch_enable(cmd_status_fd, 1) < 0)
        //        {
        //            printc(MSG_ERROR, "set gtx rx enable failed");
        //        }

        struct CmdStatusRWThreadParams *read_params = (struct CmdStatusRWThreadParams *)malloc(sizeof(struct CmdStatusRWThreadParams));
        read_params->dev_fd = cmd_status_fd;
        read_params->socket_fd = conn_fd;
        read_params->sys_status = &system_status;

        pthread_t thread_id = 0;
        if(pthread_create(&thread_id, NULL, (void *)read_net_cmd_thread, (void *)(read_params)) != 0)
        {
            printc(MSG_ERROR, "create read_dma_thread failed!. exit\n");
            goto Exit;
        }

        if(pthread_create(&thread_id, NULL, (void *)send_status_data_thread, (void *)(read_params)) != 0)
        {
            printc(MSG_ERROR, "create send_acq_data_thread failed!. exit\n");
            goto Exit;
        }
        struct ProcSource *proc_source = (struct ProcSource *)malloc(sizeof(struct ProcSource));
        {
            init_proc_source(proc_source);
            // set fd
            int index = 0;
            proc_source->fd[index++] = cmd_status_fd;
            proc_source->fd[index++] = conn_fd;
            proc_source->sys_status = &system_status;
            proc_source->child_pid = 0;
            sprintf(proc_source->desc, "chi...");
        }

        if(pthread_create(&thread_id, NULL, (void *)clear_thread, (void *)(proc_source)) != 0)
        {
            printc(MSG_ERROR, "create clear_thread failed!. exit\n");
        }
        printc(MSG_INFO, "create all thread of child thread...\n");
        goto Exit;

    }else{
        close_fd(1, &conn_fd);
        if(child_pid < 0)
            printc(MSG_ERROR, "fork new process failed\n");
        if(has_clear_thread == 0)
        {
            // create thread
            struct ProcSource *proc_source = (struct ProcSource *)malloc(sizeof(struct ProcSource));
            {
                init_proc_source(proc_source);
                // set fd
                int index = 0;
                proc_source->fd[index++] = listen_fd;
                proc_source->sys_status = &system_status;
                proc_source->child_pid = child_pid;
                sprintf(proc_source->desc, "par...");
            }

            pthread_t thread_id = 0;
            if(pthread_create(&thread_id, NULL, (void *)clear_thread, (void *)(proc_source)) != 0)
            {
                printc(MSG_ERROR, "create create_cmd_status_trans_server failed!. exit\n");
                goto ExitCloseListenFd;
            }
            has_clear_thread = 1;
        }
        goto ACCEPT_AGAIN;
    }

ExitCloseListenFd:
    close_fd(1, &listen_fd);

    //ExitCloseDevice:
    //    close_fd(1, &cmd_status_fd);

Exit:
    printc(MSG_INFO, "create_cmd_status_trans_server exit...\n");
}


int open_cmd_status_device(const char *dev_path)
{
    int cmd_status_fd = -1;
    if((cmd_status_fd = open(dev_path, O_RDWR)) <= 0)
    {
        perror(" ");
        return -1;
    }
    sleep(1);
    ioctl(cmd_status_fd, CSR_SET_SOFT_RESET, 0);
    return cmd_status_fd;
}


void read_net_cmd_thread(struct CmdStatusRWThreadParams *params)
{
    unsigned char cmd_cache[512] = {0};
    char reset_frame[] = {0x0a, 0x53, 0xf8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};
    while(*(params->sys_status) != Stop)
    {
        if(readn(params->socket_fd, &cmd_cache, 1) == 1)
        {
            printc(MSG_INFO, "read_net_cmd_thread: get cmd len == > %d\n", cmd_cache[0]);
            if(cmd_cache[0] < 1)
            {
                printc(MSG_WARN, "read_net_cmd_thread: find header len is less than 2\n");
                continue;
            }
            if(readn(params->socket_fd, cmd_cache + 1, cmd_cache[0]) == cmd_cache[0])
            {
                printc(MSG_INFO, "Get cmd: ");
                for(int i = 0; i < cmd_cache[0] +1; i++)
                {
                    printf("%02X ", cmd_cache[i]);
                }
                printf("\n");

                if((cmd_cache[0] == 0x0a) && (strncmp(reset_frame, cmd_cache, sizeof(reset_frame) == 0)))
                {
                    reset_hardware(params->dev_fd);
                    printc(MSG_INFO, "recv system reset frame\n");
                }else{
#ifndef SIMU_MODE
                    if(writen_to_cmd_status_dev(params->dev_fd, cmd_cache, cmd_cache[0] + 1) != (cmd_cache[0] + 1))
                    {
                        perror(" ");
                        printc(MSG_ERROR, "read_net_cmd_thread write data to cmd device failed, thread exit\n");
                        break;
                    }
#endif
                }
            }else{
                printc(MSG_ERROR, "read_net_cmd_thread read cmd entity data from net failed\n");
                break;
            }
        }else{
            printc(MSG_ERROR, "read_net_cmd_thread read cmd data from net failed\n");
            break;
        }
    }
    printc(MSG_WARN, "read_net_cmd_thread exit...\n");
}


void send_status_data_thread(struct CmdStatusRWThreadParams *params)
{
    // limit by device
    const int max_read_len_once = 0x1000;
    char *status_buf = (char *)malloc(sizeof(char) * max_read_len_once);
    if(status_buf == NULL)
    {
        printc(MSG_ERROR, "send_status_data_thread applicate mem from system failed, thread exit\n");
        return;
    }
    int read_ret = 0;
    while(*(params->sys_status) != Stop)
    {
#ifndef SIMU_MODE
        printc(MSG_INFO, "start read....\n");
        read_ret = read(params->dev_fd, status_buf, max_read_len_once);
        printc(MSG_INFO, "read status, len is %d\n", read_ret);
#else
        read_ret = 100;
        sleep(1);
#endif
        if(read_ret == 0)
            continue;
        if(read_ret > 0)
        {
            if(writen(params->socket_fd, status_buf, read_ret) != read_ret)
            {
                if(params->socket_fd > 0)
                    printc(MSG_ERROR, "send_status_data_thread write status data to socket failed\n");
                params->socket_fd = -1;
            }
        }else{
            if((errno == EINTR || errno == EAGAIN))
                continue;
            else{
                printc(MSG_ERROR, "send_status_data_thread read status data from device failed\n");
                perror(" ");
                break;
            }
        }
    }
    printc(MSG_INFO, "send_status_data_thread thread exit...\n");
}

void init_proc_source(struct ProcSource *source)
{
    source->sys_status = NULL;
    source->child_pid = 0;
    for(int i = 0; i < sizeof(source->fd)/sizeof(int); i++)
    {
        source->fd[i] = -1;
    }

    for(int i = 0; i < sizeof(source->mem) / sizeof(char *); i++)
    {
        source->mem[i] = NULL;
    }
}

void clear_thread(struct ProcSource *sour)
{
    while(1)
    {
        printc(MSG_INFO, "%s* wait stop signal\n", sour->desc);
        sem_wait(g_sem);
        printc(MSG_INFO, "get signal check system status...\n");
        if(*(sour->sys_status) == Stop)
            break;
    }
    if(sour->child_pid > 0)
    {
        // parent
        int child_proc_status = 0;
        kill(sour->child_pid, SIGINT);
        wait(&child_proc_status);
    }else if(sour->child_pid == 0){
        // child
        printc(MSG_WARN, "clear_thread: reaset hardware\n");
        reset_hardware(sour->fd[0]);
    }

Clear:
    printc(MSG_INFO, "%s-cmd status send proc quit...\n", sour->desc);
    for(int i = 0; i < sizeof(sour->fd)/sizeof(int); i++)
    {
        if(sour->fd[i] > 0)
        {
            close(sour->fd[i]);
        }
    }

    for(int i = 0; i < sizeof(sour->mem) / sizeof(char *); i++)
    {
        if(sour->mem[i] != NULL)
        {
            free(sour->mem[i]);
        }
    }
    exit(0);
}

int writen_to_cmd_status_dev(int fd, char *data, int len)
{
    const int max_write_len_once = 0x1000;
    int left_len = len;
    while(left_len > 0)
    {
        int write_len = left_len > max_write_len_once ? max_write_len_once : left_len;
        int ret = writen(fd, data + len - left_len, write_len);
        if(ret != write_len)
        {
            perror("write cmd: ");
            return len - left_len;
        }
        left_len -= ret;
    }
    return len;
}

int reset_hardware(int cmd_status_fd)
{
    printc(MSG_WARN, "now start reset hardware...\n");
    ioctl(cmd_status_fd, CSR_SET_CTRL_REGISTER, 1);
    usleep(100000);
    ioctl(cmd_status_fd, CSR_SET_CTRL_REGISTER, 0);
    return 0;
}

// not used
int set_gtx_rx_ch_enable(int fd, int open)
{
    static char enable_dma_data_trans[] = {0x0a, 0x83, 0x80, 0x00, 0x00, 0x00, 0x00, 0xff, 0xa0, 0x00, 0x00};
    static char disable_dma_data_trans[] = {0x0a, 0x83, 0x80, 0x00, 0x00, 0x00, 0x00, 0xff, 0xa1, 0x00, 0x00};

    if(open == 1){
        return writen(fd, enable_dma_data_trans, sizeof(enable_dma_data_trans)) ==
                sizeof(enable_dma_data_trans) ? sizeof(enable_dma_data_trans) : -1;
    }else{
        return writen(fd, disable_dma_data_trans, sizeof(disable_dma_data_trans)) ==
                sizeof(disable_dma_data_trans) ? sizeof(disable_dma_data_trans) : -1;
    }
}

int write_ip_to_device(const char *dev_path)
{

}
