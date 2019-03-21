#include "linux_thread.h"
#include <stdio.h>

int stop = 0;

void out_info_per_sec()
{
    int max_count = 10;
    while(max_count-- > 0)
    {
        printf("%d\n", 10 - max_count);
        stop = 0;
        sleep(1);
        stop = 0;
    }
    stop = 1;
}

int main()
{
    if(create_detached_pthread((void *)out_info_per_sec, NULL) == 0)
    {
        printf("Create 1 Thread Success\n");
    }else{
        printf("Create 1 Thread Failed\n");
    }

    if(create_detached_pthread((void *)out_info_per_sec, NULL) == 0)
    {
        printf("Create 2 Thread Success\n");
    }else{
        printf("Create 2 Thread Failed\n");
    }
    while(1)
    {
        if(stop == 1)
        {
            sleep(3);
            if(stop == 1)
            {
                break;
            }
        }
    }
    printf("Over\n");
    return 0;
}
