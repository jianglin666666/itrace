#include "general.h"

int is_process_alive(pid_t target_pid)
{
    int ret = kill(target_pid, 0);
    if(ret == 0)
    {
        return 1;
    }else{
        if(errno == ESRCH)
        {
            return 0;
        }else{
            perror("func: test_process_alive() has error, can not check target process status, just return alive.");
        }
    }
    return 1;
}

ssize_t readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR || errno == EAGAIN)
            {
                nread = 0;        /* and call read() again */
                usleep(1000);
            }
            else
                return(-1);
        } else if (nread == 0)
            break;                /* EOF */

        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);        /* return >= 0 */
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    if(fd < 0)
        return 0;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && (errno == EINTR || errno == EAGAIN)){
                nwritten = 0;        /* and call write() again */
                usleep(1000);
            }
            else
                return(-1);            /* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

void print_log()
{
    printc(MSG_INFO,

                    "+==============================\n"
                    "####  ######  ########  ###### \n"
                    " ##  ##    ##    ##    ##    ##\n"
                    " ##  ##          ##    ##      \n"
                    " ##  ##          ##     ###### \n"
                    " ##  ##          ##          ##\n"
                    " ##  ##    ##    ##    ##    ##\n"
                    "####  ######     ##     ###### \n"
                    "+===========20181224==========+\n" );
}
