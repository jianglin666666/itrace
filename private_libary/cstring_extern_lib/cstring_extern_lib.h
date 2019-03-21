#ifndef CSTRING_EXTERN_LIB_H
#define CSTRING_EXTERN_LIB_H
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
///
/// \brief cstring_trim 去除字符串两边的空格
/// \param src
/// \return
///
char *cstring_trim(char *src);

char *str2upper(char *src);
char *str2lower(char *src);


int c_splite(char *str, char *delims, char result[][32], int max_element_num);

void print_as_hex(void *data, int len, const int *num_per_line);

#endif // CSTRING_EXTERN_LIB_H
