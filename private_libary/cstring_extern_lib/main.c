#include "cstring_extern_lib.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Unit Test\n");
//    char *test_trim[] = {
//      "TEST", " TEST", "TEST ", " TEST ", " TEST Y ",
//        "  ", "", "V V ", "P","          P ", "0"
//    };
    char test_trim[] = "z s  aaSS vv x d ";
//    for(int i = 0; i < 10; i++)
//    {
        printf("Raw: *%s*==>*%s*\n", (test_trim), str2lower(test_trim));
        printf("Raw: *%s*==>*%s*\n", (test_trim), str2upper(test_trim));
//    }

     char result[100][32];
     int num = 0;
     printf("-->num: %d\n", num = c_splite(test_trim, " ", result, 100));
     for(int i = 0; i < num; i++)
     {
         printf("%d=>%s\n", i, result[i]);
     }
    return 0;
}
