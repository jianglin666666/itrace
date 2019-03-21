#include "linux_serial_port.h"
#include "../linux_thread/linux_thread.h"
#include <stdio.h>

#define DEV_PATH "/dev/ttyS1"

int serial_port_fd = -1;

void read_port()
{
    char read_ch;
    while(1)
    {
        if(serial_port_fd)
        {
            if(read(serial_port_fd, &read_ch, 1) > 0)
            {
                printf("->%02X \n", read_ch);
            }
        }
    }
}

int main(int argc, char *argv[])
{

    SerialPortParams spp;
    spp.baud_rate = 9600;
    spp.parity_bits = 'N';
    spp.data_bit = 8;
    spp.stop_bit = 1;
    spp.flow_type = FLOW_TYPE_HARDWARE;
    spp.max_over_time = 0;
    spp.max_char_recved = 0;

    if((serial_port_fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        printc(MSG_FATAL, "Open Serial Port Failed!\n");
        return -1;
    }else{
        printc(MSG_INFO, "Open Serial Port Success! ID: %d\n", serial_port_fd);
    }

//    close(serial_port_fd);
//    return ;
    if( fcntl(serial_port_fd, F_SETFL, 0) < 0 )
    {
        close(serial_port_fd);
        perror("fcntl failed!");
        return -1;
    }

    printc(MSG_INFO, "Update Serial Port Attr.%s\n", set_serial_port_attr(serial_port_fd, &spp, NULL) == 0? "OK":"FAILED");

    if((create_detached_pthread(read_port, NULL)) != 0)
    {
        printc(MSG_FATAL, "Creat Read Thread Failed!\n");
        return -1;
    }else{
        printc(MSG_INFO, "Creat Read Thread Success!\n");
    }

    char buf[128];
    for(int i = 0; i < 128; i++)
        buf[i] = i;
    while(1)
    {
//        memset(buf, 0, sizeof(buf));
//        fgets(buf, sizeof(buf), stdin);
        printf("> %d\n",write(serial_port_fd, buf, sizeof(buf)));
        getchar();
    }
    return 0;
}
