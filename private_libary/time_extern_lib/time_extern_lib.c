#include "time_extern_lib.h"

char *get_date_str(char delime, char *time_str)
{
    time_t nowtime = time(NULL);
    struct tm *now = localtime(&nowtime);

    if(delime != 0)
        sprintf(time_str, "%04d%c%02d%c%02d%c", now->tm_year + 1900, delime,  now->tm_mon + 1, delime, now->tm_mday, 0);
    else
        sprintf(time_str, "%04d%02d%02d%c", now->tm_year + 1900,  now->tm_mon + 1, now->tm_mday, 0);
    return time_str;
}

char *get_time_str(char delime, char *time_str)
{
    time_t nowtime = time(NULL);
    struct tm *now = localtime(&nowtime);

    if(delime != 0)
        sprintf(time_str, "%04d%c%02d%c%02d%c", now->tm_hour, delime, now->tm_min, delime, now->tm_sec, 0);
    else
        sprintf(time_str, "%04d%02d%02d%c", now->tm_hour, now->tm_min, now->tm_sec, 0);
    return time_str;
}

char *get_date_time_tag(char *date_time_tag)
{
    char time_tag_tmp[32];
    char date_tag_tmp[32];
    sprintf(date_time_tag, "%s_%s%c", get_date_str(0, date_tag_tmp), get_time_str(0, time_tag_tmp), 0);
    return date_time_tag;
}
