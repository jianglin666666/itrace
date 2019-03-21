#include "log_type.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello World!\n");
    printc(MSG_INFO, "MSG_INFO\n");
    printc(MSG_WARN, "MSG_WARN\n");
    printc(MSG_DEBUG, "MSG_DEBUG\n");
    printc(MSG_ERROR, "MSG_ERROR\n");
    printc(MSG_FATAL, "MSG_FATAL\n");
    printc(MSG_TRACE, "MSG_TRACE\n");
    return 0;
}
