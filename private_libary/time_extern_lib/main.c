#include "time_extern_lib.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    char time_str[32];
    printf("-->%s\n", get_date_str('_', time_str));
    printf("-->%s\n", get_time_str('_', time_str));
    printf("-->%s\n", get_date_time_tag(time_str));
    return 0;
}
