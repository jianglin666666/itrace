#include "server.h"
extern enum SystemStatus system_status;
extern sem_t *g_sem;

void create_acq_data_trans_server(SocketInfo *server_info)
{
    int dma_fd  = -1;
    int listen_fd = -1;
    struct DmaBuffer *dma_buf_list[2] = {NULL, NULL};
    int clt_addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in clt_addr;
    int conn_fd = -1;
    pid_t child_pid = 0;

    int has_clear_thread = 0;

    // create listen
    if((listen_fd = create_srv_netsock(server_info->port, server_info->desc)) < 0)
    {
        printc(MSG_FATAL, "%s: creat server net socket failed. exit...\n", server_info->desc);
        goto Exit;
    }

    printc(MSG_INFO, "acq data send server start listen...\n");
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
    printc(MSG_INFO, "acq data send server get new connect: %s\n", inet_ntoa(clt_addr.sin_addr));

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
        // open device
#ifndef SIMU_MODE
        dma_fd = open_dma_device(DmaDevicePath);
#else
        dma_fd = 1;
#endif
        printc((dma_fd > 0) ? MSG_INFO : MSG_ERROR, "open device %s %s\n", DmaDevicePath, (dma_fd > 0) ? "success" : "failed");
        if(dma_fd < 0)
        {
            goto Exit;
        }
        printc(MSG_INFO, "success \n");
        // init mem
        printc(MSG_INFO, "init dma buffer list  \n");

        char *dma_fd_mmap_area = NULL;
#ifndef SIMU_MODE
        dma_fd_mmap_area = (char *) mmap(NULL, 2*MAX_DMA_LENGTH, PROT_READ, MAP_SHARED, dma_fd, 0);
#else
        dma_fd_mmap_area = (char *)malloc(2*MAX_DMA_LENGTH);
#endif
        if(dma_fd_mmap_area == NULL)
        {
            printc(MSG_ERROR, "applicate dma_device mem failed, process quit\n");
            close_fd(1, &dma_fd);
            exit(0);
        }
        for(int i = 0; i < 2; i++)
        {
            dma_buf_list[i] = init_dma_read_buffer(i, dma_fd_mmap_area);
            if(dma_buf_list[i] == NULL)
            {
                printc(MSG_ERROR, "init dma_buf_list failed, index is %d, max is %d\n", i, 2);
                clear_dma_dev_sour(dma_fd_mmap_area, &dma_fd);
                exit(0);
            }
        }
        // child
        close_fd(1, &listen_fd);

        int dma_index_pipe[2] = {-1, -1};
        int socket_index_pipe[2] = {-1, -1};

        // index pipe
        printc(MSG_INFO, "init index buffer...\n");
        if((pipe(dma_index_pipe) < 0) || (pipe(socket_index_pipe) < 0))
        {
            printc(MSG_ERROR, "create dma or socket index pipe failed\n");
            clear_dma_dev_sour(dma_fd_mmap_area, &dma_fd);
            exit(0);
        }else{
            struct ReadDmaThreadParams *read_params = (struct ReadDmaThreadParams *)malloc(sizeof(struct ReadDmaThreadParams));
            read_params->buff_list[0] = dma_buf_list[0];
            read_params->buff_list[1] = dma_buf_list[1];
            read_params->dma_fd = dma_fd;
            read_params->dma_index_r_fd = dma_index_pipe[0];
            read_params->socket_index_w_fd = socket_index_pipe[1];
            read_params->sys_status = &system_status;
            struct SendAcqDataThreadParams *send_params = (struct SendAcqDataThreadParams *)malloc(sizeof(struct SendAcqDataThreadParams));
            send_params->buff_list[0] = dma_buf_list[0];
            send_params->buff_list[1] = dma_buf_list[1];
            send_params->dma_index_w_fd = dma_index_pipe[1];
            send_params->socket_index_r_fd = socket_index_pipe[0];
            send_params->socket_fd = conn_fd;
            send_params->sys_status = &system_status;

            // init pipe
            {
                char init_index[2] = {0x00, 0x01};
                if(writen(dma_index_pipe[1], init_index, sizeof(init_index)) != sizeof(init_index))
                {
                    printc(MSG_ERROR, "write init index to dma index pipe failed.");
                    clear_dma_dev_sour(dma_fd_mmap_area, &dma_fd);
                    exit(0);
                }
            }
            pthread_t thread_id = 0;
            if(pthread_create(&thread_id, NULL, (void *)read_dma_thread, (void *)(read_params)) != 0)
            {
                printc(MSG_ERROR, "create read_dma_thread failed!. exit\n");
                clear_dma_dev_sour(dma_fd_mmap_area, &dma_fd);
                exit(0);
            }

            if(pthread_create(&thread_id, NULL, (void *)send_acq_data_thread, (void *)(send_params)) != 0)
            {
                printc(MSG_ERROR, "create send_acq_data_thread failed!. exit\n");
                clear_dma_dev_sour(dma_fd_mmap_area, &dma_fd);
                exit(0);
            }
            struct ProcSource *proc_source = (struct ProcSource *)malloc(sizeof(struct ProcSource));
            {
                init_proc_source(proc_source);
                // set fd
                int index = 0;
                proc_source->fd[index++] = dma_fd;
                proc_source->fd[index++] = conn_fd;
                proc_source->fd[index++] = dma_index_pipe[0];
                proc_source->fd[index++] = dma_index_pipe[1];
                proc_source->fd[index++] = socket_index_pipe[0];
                proc_source->fd[index++] = socket_index_pipe[1];
                index = 0;
                proc_source->mem[index++] = dma_buf_list[0]->data;
                proc_source->mem[index++] = dma_buf_list[1]->data;
                proc_source->sys_status = &system_status;
                sprintf(proc_source->desc, "chi...");
            }

            if(pthread_create(&thread_id, NULL, (void *)clear_thread, (void *)(proc_source)) != 0)
            {
                printc(MSG_ERROR, "create send_acq_data_thread failed!. exit\n");
                clear_dma_dev_sour(dma_fd_mmap_area, &dma_fd);
                exit(0);
            }
            printc(MSG_INFO, "create all thread of child thread...\n");
            goto Exit;
        }
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
                proc_source->fd[index++] = dma_fd;
                proc_source->sys_status = &system_status;
                sprintf(proc_source->desc, "par...");
            }

            pthread_t thread_id = 0;
            if(pthread_create(&thread_id, NULL, (void *)clear_thread, (void *)(proc_source)) != 0)
            {
                printc(MSG_ERROR, "create send_acq_data_thread failed!. exit\n");
                goto ExitCloseListenFd;
            }
            has_clear_thread = 1;
        }
        goto ACCEPT_AGAIN;
    }


ExitCloseListenFd:
    close_fd(1, &listen_fd);
Exit:
    printc(MSG_INFO, "create_acq_data_trans_server exit...\n");
}

int open_dma_device(const char *dev_path)
{
    int dma_fd = -1;
    if((dma_fd = open(dev_path, O_RDONLY)) <= 0)
    {
        perror(" ");
        return -1;
    }
    sleep(1);
    ioctl(dma_fd, SET_SOFT_RESET, 0);
    return dma_fd;
}


struct DmaBuffer *init_dma_read_buffer(int index, char *start_addr)
{
    struct DmaBuffer *buff = (struct DmaBuffer*)malloc(sizeof(struct DmaBuffer));
    if(buff == NULL)
    {
        perror("func: init_dma_read_buffer[struct] ");
        return NULL;
    }
    buff->index = index;
    buff->length = 0;
    buff->capity = MAX_DMA_LENGTH;
    buff->data = index * MAX_DMA_LENGTH + start_addr;
    return buff;
}

void read_dma_thread(struct ReadDmaThreadParams *params)
{
    char buf_index = 0;
    while(*(params->sys_status) != Stop)
    {
        if(readn(params->dma_index_r_fd, &buf_index, sizeof(buf_index)) == sizeof(buf_index))
        {
            printc(MSG_INFO, "read_dma_thread: get dma index == > %d\n", buf_index);
            struct DmaBuffer *curr_buf = params->buff_list[buf_index];
#ifndef SIMU_MODE
            ioctl(params->dma_fd, CONFIG_ADDROFFSET, buf_index);
            curr_buf->length = read(params->dma_fd, curr_buf->data, MAX_DMA_LENGTH);
            printc(MSG_INFO, "read dma len: %d, data is %02X %02X %02X %02X\n", curr_buf->length,
                   curr_buf->data[0], curr_buf->data[1], curr_buf->data[2], curr_buf->data[3]);
#else
            curr_buf->length = 4096;
            for(int i = 0; i < curr_buf->length; i++)
                curr_buf->data[i] = i + buf_index * i;
            sleep(1);
#endif
            if(curr_buf->length < 0)
            {
                perror(" ");
                printc(MSG_INFO, "read_dma_thread read dma failed.\n");
                break;
            }
            if(writen(params->socket_index_w_fd, &buf_index, sizeof(buf_index)) != sizeof(buf_index))
            {
                printc(MSG_ERROR, "read_dma_thread write index to socket pipe failed\n");
                break;
            }
            if(curr_buf->length == 0)
            {
                usleep(300000);
            }

            printc(MSG_INFO, "--> next\n");
        }else{
            printc(MSG_ERROR, "read_dma_thread read index from dma pipe failed\n");
            break;
        }
    }
    printc(MSG_INFO, "read_dma_thread thread exit...\n");
}


void send_acq_data_thread(struct SendAcqDataThreadParams *params)
{
    char buf_index = 0;
    while(*(params->sys_status) != Stop)
    {
        if(readn(params->socket_index_r_fd, &buf_index, sizeof(buf_index)) == sizeof(buf_index))
        {
            printc(MSG_INFO, "send_acq_data_thread: get socket index == > %d\n", buf_index);
            struct DmaBuffer * curr_dma_buf = params->buff_list[buf_index];
            if(curr_dma_buf->length > 0)
            {
                printc(MSG_INFO, "send socket len: %d, data is %02X %02X %02X %02X\n", curr_dma_buf->length,
                       curr_dma_buf->data[0], curr_dma_buf->data[1], curr_dma_buf->data[2], curr_dma_buf->data[3]);
                if((writen(params->socket_fd, curr_dma_buf->data, curr_dma_buf->length) != curr_dma_buf->length))
                {
                    printc(MSG_INFO, "send_acq_data_thread write to socket failed.\n");
                    break;
                }
            }
            if(writen(params->dma_index_w_fd, &buf_index, sizeof(buf_index)) != sizeof(buf_index))
            {
                printc(MSG_ERROR, "send_acq_data_thread write index to socket pipe failed\n");
                break;
            }
        }else{
            printc(MSG_ERROR, "send_acq_data_thread read index from socket pipe failed\n");
            perror(" ");
            break;
        }
    }
    printc(MSG_INFO, "send_acq_data_thread thread exit...\n");
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
            goto Clear;
    }
    if(sour->child_pid > 0)
    {
        int child_proc_status = 0;
        kill(sour->child_pid, SIGINT);
        wait(&child_proc_status);
    }else{
        clear_dma_dev_sour(sour->mem[0], &sour->fd[0]);
    }

Clear:

    printc(MSG_INFO, "acq process prepare quit...\n");
    for(int i = 0; i < sizeof(sour->fd)/sizeof(int); i++)
    {
        if(sour->fd[i] > 0)
        {
            close(sour->fd[i]);
        }
    }
    exit(0);
}

void clear_dma_dev_sour(char *mmap_data, int *fd)
{
#ifndef SIMU_MODE
    if(mmap_data != NULL)
        munmap(mmap_data, 2*MAX_DMA_LENGTH);
    close_fd(1, &fd);
#else
    if(mmap_data != NULL)
        free(mmap_data);
#endif
}
