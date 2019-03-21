#include "linux_serial_port.h"

int get_baud_rate_from_desc(const int baud_rate_input)
{

    int baud_rate = 0;
    switch(baud_rate_input)
    {
        case 4800:
        {
            baud_rate = B4800;
            break;
        }
        case 9600:
        {
            baud_rate = B9600;
            break;
        }
        case 19200:
        {
            baud_rate = B19200;
            break;
        }
        case 115200:
        {
            baud_rate = B115200;
            break;
        }
        case 0:
        {
            baud_rate = B0;
            break;
        }
        case 50:
        {
            baud_rate = B50;
            break;
        }
        case 75:
        {
            baud_rate = B75;
            break;
        }
        case 110:
        {
            baud_rate = B110;
            break;
        }
        case 134:
        {
            baud_rate = B134;
            break;
        }
        case 150:
        {
            baud_rate = B150;
            break;
        }
        case 200:
        {
            baud_rate = B200;
            break;
        }
        case 300:
        {
            baud_rate = B300;
            break;
        }
        case 600:
        {
            baud_rate = B600;
            break;
        }
        case 1200:
        {
            baud_rate = B1200;
            break;
        }
        case 1800:
        {
            baud_rate = B1800;
            break;
        }
        case 2400:
        {
            baud_rate = B2400;
            break;
        }
        case 38400:
        {
            baud_rate = B38400;
            break;
        }
        case 57600:
        {
            baud_rate = B57600;
            break;
        }
        case 230400:
        {
            baud_rate = B230400;
            break;
        }
        case 460800:
        {
            baud_rate = B460800;
            break;
        }
        case 500000:
        {
            baud_rate = B500000;
            break;
        }
        case 576000:
        {
            baud_rate = B576000;
            break;
        }
        case 921600:
        {
            baud_rate = B921600;
            break;
        }
        case 1000000:
        {
            baud_rate = B1000000;
            break;
        }
        case 1152000:
        {
            baud_rate = B1152000;
            break;
        }
        case 1500000:
        {
            baud_rate = B1500000;
            break;
        }
        case 2000000:
        {
            baud_rate = B2000000;
            break;
        }
        case 2500000:
        {
            baud_rate = B2500000;
            break;
        }
        case 3000000:
        {
            baud_rate = B3000000;
            break;
        }
        case 3500000:
        {
            baud_rate = B2500000;
            break;
        }
        case 4000000:
        {
            baud_rate = B4000000;
            break;
        }
        default:
        {
            baud_rate = -1;
            printc(MSG_ERROR, "%d is not support for baud rate.\n", baud_rate_input);
            return -1;
        }
    }
    return baud_rate;
}

int get_data_bits_from_desc(const int data_bits)
{
    switch(data_bits)
    {
        case 5:
        {
            return CS5;
        }
        case 6:
        {
            return CS6;
        }
        case 7:
        {
            return CS7;
        }
        case 8:
        {
            return CS8;
        }
        default:
        {
            printc(MSG_ERROR, "%d is not support for data bits.\n", data_bits);
            return -1;
        }
    }
    return -1;
}

int get_stop_bits_from_desc(const int stop_bits)
{
    switch (stop_bits) {
        case 1:
            return STOP_BITS_1;
        case 2:
            return STOP_BITS_2;
        default:
        {
            printc(MSG_ERROR, "%d is not support for stop bits\n", stop_bits);
            return -1;
        }
    }
    return -1;
}

int get_parity_from_desc(const char desc)
{
    if((desc == 'n') || (desc == 'N'))
    {
        return PARITY_NO;
    }else if((desc == 'o') || (desc == 'O'))
    {
        return PARITY_ODD;
    }else if((desc == 'e') || (desc == 'E')){
        return PARITY_EVEN;
    }else if((desc == 's') || (desc == 'S')){
        return PARITY_SPACE;
    }else{
        printc(MSG_ERROR, "%c is not support for parity.\n", desc);
        return -1;
    }
    return -1;
}


int set_serial_port_attr(int serial_dev_fd, const SerialPortParams *spp, struct termios *old_attr)
{
    if(serial_dev_fd < 0)
        return -1;
    if(old_attr)
        tcgetattr(serial_dev_fd, old_attr);

    struct termios new_attr;
    bzero(&new_attr, sizeof(struct termios));

    // 在旧的配置的基础上设置新的配置
    tcgetattr(serial_dev_fd, &new_attr);

    int ON, OFF;
    ON = 1;
    OFF = 0;

    char *msg_success = "Success";
    char *msg_failed = "Failed";
    printc(MSG_INFO, "Set Local Mode: %s\n", set_local_recv(&new_attr, ON) == 0?msg_success:msg_failed);
    printc(MSG_INFO, "Set Baud Rate: %s\n", set_baud_rate(&new_attr, spp->baud_rate) == 0?msg_success:msg_failed);
    printc(MSG_INFO, "Set Data Bits: %s\n", set_data_bits(&new_attr, spp->data_bit) == 0?msg_success:msg_failed);
    printc(MSG_INFO, "Set Parity Bits: %s\n", set_parity_bits(&new_attr, spp->parity_bits) == 0?msg_success:msg_failed);
    printc(MSG_INFO, "Set Stop Bits: %s\n", set_stop_bits(&new_attr, spp->stop_bit) == 0?msg_success:msg_failed);
    // no NL to CR translation
    new_attr.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |INLCR);

    // disable software flow control
    new_attr.c_iflag &= ~(IXON | IXOFF | IXANY);

    // output flags - turn off output processing - raw output
    new_attr.c_oflag &= ~(OPOST);

    // no line processing - raw input
    // echo off, echo newline off, canonical mode off,
    // extended input processing off, signal chars off
    new_attr.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    if (tcsetattr(serial_dev_fd, TCSANOW, &new_attr) != 0)
    {
        close(serial_dev_fd);
        printc(MSG_FATAL, "Update Serial Port Params Failed.\n");
        return -1;
    }
    ////清空串口输入输出队列
    tcflush(serial_dev_fd, TCIFLUSH);
    tcflush(serial_dev_fd, TCOFLUSH);
    return 0;
}

void get_curr_serial_attr(int serial_dev_fd, struct termios *old_attr)
{
    tcgetattr(serial_dev_fd, old_attr);
}

int set_baud_rate(struct termios *options, int baud_rate_desc)
{
    if((get_baud_rate_from_desc(baud_rate_desc)) < 0)
        return -1;

    cfsetispeed(options, get_baud_rate_from_desc(baud_rate_desc));
    cfsetospeed(options, get_baud_rate_from_desc(baud_rate_desc));
    return 0;
}

int set_stop_bits(struct termios *options, char stop_bits)
{

    if(get_stop_bits_from_desc(stop_bits) < 0)
        return -1;

    switch(get_stop_bits_from_desc(stop_bits))
    {
        case STOP_BITS_1:
        {
            options->c_cflag &= ~CSTOPB;
            break;
        }
        case STOP_BITS_2:
        {
            options->c_cflag |= CSTOPB;
            break;
        }
    }
    return 0;
}


int set_data_bits(struct termios *options, char data_bits)
{

    if(get_data_bits_from_desc(data_bits) < 0)
        return -1;

    options->c_cflag &= ~CSIZE;
    options->c_cflag |= get_data_bits_from_desc(data_bits);
    return 0;
}

int set_parity_bits(struct termios *options, char parity_desc)
{
    if(get_parity_from_desc(parity_desc) < 0)
        return -1;

    options->c_iflag |= INPCK;//使能校验检查
    switch (get_parity_from_desc(parity_desc)) {
        case PARITY_EVEN:
            options->c_cflag |= PARENB;   // 打开校验使能
            options->c_cflag &= ~PARODD;  // 转换为偶校验
            options->c_iflag |= (INPCK|PARMRK|ISTRIP);// disable input parity check and parity bit strip
            break;
        case PARITY_ODD:
            options->c_cflag |= PARENB;
            options->c_cflag |= PARODD;
            options->c_iflag |= (INPCK|PARMRK|ISTRIP);
            break;
        case PARITY_NO:
            options->c_cflag &= ~PARENB;
            options->c_iflag &= ~(INPCK|PARMRK|ISTRIP);
//            options->c_iflag &= ~INPCK;
        case PARITY_SPACE:
            options->c_cflag &= ~PARENB;
            options->c_iflag &= ~(INPCK|PARMRK|ISTRIP);
//            options->c_cflag &= ~CSTOPB;
    }
    return 0;
}

int set_hardware_flow_ctl(struct termios *options, int on)
{
    if(on)
    {
        options->c_cflag |= CRTSCTS;
    }else{
        options->c_cflag &= ~CRTSCTS;
    }
    return 0;
}

int set_over_time_and_char_recved(struct termios *options, int max_sec, int max_data_size)
{
    options->c_cc[VTIME] = max_sec;
    options->c_cc[VMIN] = max_data_size;

    return 0;
}

int set_raw_input_mode(struct termios *options, int on)
{
//    if(!on)
//    {
//        // 暂时不应该使用标准模式
//        options->c_lflag |= (ICANON | ECHO | ECHOE);
//    }else{
//        options->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
//        options->c_oflag &= ~OPOST; //
//    }
    return 0;
}

int set_software_flow_ctl(struct termios *options, int on)
{
    if(on)
    {
        options->c_iflag |= (IXON | IXOFF | IXANY);
    }else{
        options->c_iflag &= ~(IXON | IXOFF | IXANY);
    }
    return 0;
}

int set_local_recv(struct termios *options, int on)
{

//    if(!on)
//    {
//        options->c_cflag &= ~CLOCAL;
//    }else{
//        options->c_cflag |= CLOCAL;
//        options->c_cflag |= CREAD;
//    }
    return 0;
}

int set_serial_port_quick(int serial_dev_fd, int baud_rate, char parity, int data_bits, int stop_bit, struct termios *old_attr)
{
    SerialPortParams spp;
    spp.baud_rate = baud_rate;
    spp.parity_bits = parity;
    spp.stop_bit = stop_bit;
    spp.data_bit = data_bits;
    spp.flow_type = FLOW_TYPE_HARDWARE;
    spp.max_char_recved = 1;
    spp.max_over_time = 0;
    if(old_attr == NULL)
        set_serial_port_attr(serial_dev_fd, &spp, NULL);
    else{
        set_serial_port_attr(serial_dev_fd, &spp, old_attr);
    }
    return 0;
}
