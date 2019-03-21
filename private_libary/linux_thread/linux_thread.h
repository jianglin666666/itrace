#ifndef YUKK_PRIVATE_LIBARY_LINUX_LINUX_THREAD_LINUX_THREAD_H
#define YUKK_PRIVATE_LIBARY_LINUX_LINUX_THREAD_LINUX_THREAD_H

//#include "../log_type/log_type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <linux/unistd.h>
#include <semaphore.h>
#include <sys/times.h>
#include <sys/shm.h>
#include <pthread.h>

///
/// \brief create_detached_pthread 创建一个线程
/// \param fn: 函数指针
/// \param arg: 程序参数
/// \return 0: 创建成功，其他表示出错
///
int create_detached_pthread(void *(*fn)(void *), void * arg);

int thread_is_running(pthread_t pid);

#endif // YUKK_PRIVATE_LIBARY_LINUX_LINUX_THREAD_LINUX_THREAD_H
