#ifndef TIME_EXTERN_LIB_H
#define TIME_EXTERN_LIB_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

char *get_date_str(char delime, char *time_str);
char *get_time_str(char delime, char *time_str);
char *get_date_time_tag(char *time_tag);

#endif // TIME_EXTERN_LIB_H
