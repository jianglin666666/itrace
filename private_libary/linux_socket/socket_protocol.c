#include "socket_protocol.h"

int close_fd(const int fd_num, ...)
{
    va_list ap;
    va_start(ap, fd_num);

    int  *curr_fd = NULL;
    for(int i = 0; i < fd_num; i++)
    {
        curr_fd = va_arg(ap, int *);
        if(*curr_fd )
        {
            //close(*curr_fd);
            *curr_fd = -1;
        }
    }
    printf("\n");
    return 0;
}

int read_data(const int fd, void *buf, const unsigned int max_buf_len)
{
    int left_len = max_buf_len;
    int ret = 0;
    int max_read_count = 0;

    while(left_len)
    {
        if((ret = read(fd, buf + max_buf_len - left_len, left_len)) > 0)
        {
            max_read_count = 0;
            left_len -= ret;
        }else if(ret == 0){
            usleep(30000);
            max_read_count++;
            if(max_read_count > 10)
                break;
        }else{
            break;
        }
    }
    if(left_len == 0)
        return 1;
    else
        return -1;
}

int write_data(const int fd, const void *buf, const int buf_len)
{
    int left_len = buf_len;
    int ret = 0;
    int max_write_count = 0;

    while(left_len)
    {
        if((ret = write(fd, buf + buf_len - left_len, left_len)) > 0)
        {
            max_write_count = 0;
            left_len -= ret;
        }else if(ret == 0){
            usleep(30000);
            max_write_count++;
            if(max_write_count > 10)
                break;
        }else{
            break;
        }
    }
    if(left_len == 0)
        return 1;
    else
        return -1;
}

int read_head_frame(const int fd, HeadFrame *hf)
{
    return read_data(fd, (void *)hf, sizeof(HeadFrame));
}

void init_head_frame(HeadFrame *head_frame, int id, int flag, int res, int len)
{
    head_frame->flag = flag;
    head_frame->id = id;
    head_frame->res = res;
    head_frame->len = len;
}

void init_socket_info(SocketInfo *socket_info, const char *ip, const unsigned int port, const char *socket_desc)
{
    sprintf(socket_info->desc, "%s%c", socket_desc, 0);
    sprintf(socket_info->ip, "%s%c", ip, 0);
    socket_info->port = port;
}
