#ifndef YUKK_PRIVATE_LIBARY_LINUX_LOG_TYPE_LOG_TYPE_H
#define YUKK_PRIVATE_LIBARY_LINUX_LOG_TYPE_LOG_TYPE_H
#include "console_color.h"
// 程序中使用的信息类型以及色彩
// 信息类型：
#define MSG_FATAL       0   // 致命信息     高亮红色
#define MSG_ERROR       1   // 错误信息     红色
#define MSG_WARN        2   // 警告信息     黄色
#define MSG_INFO        3   // 一般信息     绿色
#define MSG_DEBUG       4   // 调试信息     青色
#define MSG_TRACE       5   // 追踪信息     蓝色
#define MSG_NONE        6   // 默认信息

#define printc(MSG_LEVEL, fmt, args...) do{\
    if(MSG_LEVEL == MSG_INFO)\
    printf(F_GREEN F_BG_REVERSE  "[Info]" F_GREEN ": " fmt COLOR_NONE,  ##args);\
    else if(MSG_LEVEL == MSG_DEBUG)\
    {\
    /*printf(F_CYAN F_BG_REVERSE "[Debug] " F_CYAN ": " fmt COLOR_NONE,  ##args);*/\
    ;\
    }\
    else if(MSG_LEVEL == MSG_WARN)\
    printf(F_YELLO F_BG_REVERSE "[Warning]" F_YELLO ": " fmt COLOR_NONE,  ##args);\
    else if(MSG_LEVEL == MSG_ERROR)\
    printf(F_RED F_BG_REVERSE "[Error]" F_RED ": " fmt COLOR_NONE,  ##args);\
    else if(MSG_LEVEL == MSG_FATAL)\
    printf(F_B_RED F_BG_REVERSE "[Fatal]" COLOR_NONE ": " F_B_RED fmt COLOR_NONE,  ##args);\
    else if(MSG_LEVEL == MSG_TRACE)\
    printf(F_BLUE F_BG_REVERSE "[Trace]" F_BLUE ": " fmt COLOR_NONE,  ##args);\
    else if(MSG_LEVEL == MSG_NONE)\
    printf(F_BLUE F_BG_REVERSE "" F_BLUE "" fmt COLOR_NONE,  ##args);\
    }while(0);

#endif // YUKK_PRIVATE_LIBARY_LINUX_LOG_TYPE_LOG_TYPE_H
