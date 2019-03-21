#include "cstring_extern_lib.h"
char *cstring_trim(char *src)
{
    if(strlen(src) == 0)
        return NULL;
    char *start_ch = src;

    while((*(start_ch) == ' ') )
    {
        start_ch++;
        if(start_ch == (src + strlen(src)))
            return NULL;
    }

    char *last_ch = start_ch + strlen(start_ch) - 1;

    while((*(last_ch) == ' ') || (*(last_ch) == '\n'))
    {
        last_ch--;
        if(last_ch < (start_ch))
            return NULL;
    }

    char *data = (char *)malloc(last_ch - start_ch  + 1 + 1);
    memcpy((void *)data, (void *)start_ch, last_ch - start_ch  + 1);
    *(data + (last_ch - start_ch)  + 1) = 0;
    return data;

}

char *str2upper(char *src)
{
    int i = strlen(src) - 1;
    if(i < 0)
        return NULL;
    while(i >= 0)
    {
        *(src + i) = toupper(*(src + i));
        i--;
    }
    return src;
}

char *str2lower(char *src)
{
    char  *origin = src;
    while(*(src) != '\0')
    {
        *src = tolower(*src);
        src++;
    }
    return origin;
}



int c_splite(char *str, char *delims, char result[][32], int max_element_num)
{
    if(strlen(str) == 0)
        return 0;
    for(int i = 0; i < max_element_num; i++)
    {
        memset(result[i], 0, sizeof(result[i]));
    }
    int i = 0;
    int j = 0;
    char *element = strtok(str, delims);
    while(element != NULL)
    {
        for(i = 0; (i < sizeof(result[j]))&&(i<strlen(element)); i++)
        {
            result[j][i] = *(element + i);
        }
        j++;
        element = strtok(NULL, delims);
    }
    return j;
}

void print_as_hex(void *data, int len, const int *num_per_line)
{
    if(data == NULL)
        return;
    int num = 16;
    if(num_per_line != NULL)
    {
        num = *num_per_line;
    }

    unsigned char *new_data = (unsigned char *)data;
    for(int i = 0; i < len; i++)
    {
        printf("%02X%c", *(new_data + i), (i + 1)%num != 0?' ':'\n');
    }
}
