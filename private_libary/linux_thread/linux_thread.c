#include "linux_thread.h"

int create_detached_pthread(void *(*fn)(void *), void * arg)
{
    int             err;
    pthread_t       tid;
    pthread_attr_t  attr;

    err = pthread_attr_init(&attr);
    if (err != 0)
        return(err);

    err = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err == 0)
        err = pthread_create(&tid, &attr, fn, arg);

    pthread_attr_destroy(&attr);
    return(err);
}

int thread_is_running(pthread_t pid)
{
    int kill_rc = kill(pid, 0);

    int status = 0;
    if(!((kill_rc == ESRCH)  || (kill_rc == EINVAL)))
    {
        status = 1;
    }
    return status;
}
