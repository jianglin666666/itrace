#ifndef YUKK_PRIVATE_LIBARY_LINUX_LINUX_SERIAL_PORT_LINUX_SERIAL_PORT_H
#define YUKK_PRIVATE_LIBARY_LINUX_LINUX_SERIAL_PORT_LINUX_SERIAL_PORT_H

#include "../log_type/log_type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>			  //文件控制定义
#include <termios.h>			//POSIX终端控制定义

/// see more at https://www.cmrr.umn.edu/~strupp/serial.html

#define MAX_BAUD_RATE 4000000

/// 停止位
#define STOP_BITS_1 1
#define STOP_BITS_2 2

/// 校验位
#define PARITY_ODD      0
#define PARITY_EVEN     1
#define PARITY_NO     2
#define PARITY_SPACE    3

#define FLOW_TYPE_HARDWARE  0
#define FLOW_TYPE_SOFTWARE  1
#define FLOW_TYPE_NULL  3
#define FLOW_TYPE_NOT_SET 4

struct serial_port_params{
    int baud_rate;
    int data_bit;
    int stop_bit;
    char parity_bits;

    int max_over_time;
    int max_char_recved;
    int flow_type; // 未使用
};
typedef struct serial_port_params SerialPortParams;

///
/// \brief get_baud_rate_from_desc 从描述字里得到实际的波特率的值
/// \param baud_rate_input 描述字，描述字仅限于：“9600”，“115200”等纯数字形式，必须是标准的
/// \return -1表示出错
///
int get_baud_rate_from_desc(const int baud_rate_input);

///
/// \brief get_data_bits_from_desc 从描述字段取得数据位
/// \param data_bits 数据位描述字段，仅限于5——8存1位数字
/// \return -1表示出错
///
int get_data_bits_from_desc(const int data_bits);

///
/// \brief get_stop_bits_from_desc 从描述字段取得停止位
/// \param stop_bits 仅限于1或者2
/// \return -1表示出错
///
int get_stop_bits_from_desc(const int stop_bits);

///
/// \brief get_parity_from_desc 从描述字段取得校验位
/// \param desc 仅限于偶校验(even)，奇校验(odd)，无校验(no)，空校验(space)
/// \return -1表示出错
///
int get_parity_from_desc(const char desc);

void get_curr_serial_attr(int serial_dev_fd, struct termios *old_attr);

int set_baud_rate(struct termios *options, int baud_rate_desc);
int set_stop_bits(struct termios *options, char stop_bits);
int set_data_bits(struct termios *options, char data_bits);
int set_parity_bits(struct termios *options, char parity_desc);
int set_local_recv(struct termios *options, int on);
int set_hardware_flow_ctl(struct termios *options, int on);
int set_software_flow_ctl(struct termios *options, int on);
int set_over_time_and_char_recved(struct termios *options, int max_sec, int max_data_size);
int set_raw_input_mode(struct termios *options, int on);

int set_serial_port_attr(int serial_dev_fd, const SerialPortParams *spp, struct termios *old_attr);

int set_serial_port_quick(int serial_dev_fd, int baud_rate, char parity, int data_bits, int stop_bit, struct termios *old_attr);

#endif // YUKK_PRIVATE_LIBARY_LINUX_LINUX_SERIAL_PORT_LINUX_SERIAL_PORT_H
