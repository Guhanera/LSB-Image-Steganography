#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "types.h"


// Checks which operation to be done i.e encode or decode. If neither return unsupported
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

void print_help(){
    FILE *fp = fopen("help.txt","r");
    char ch;
    while((ch = fgetc(fp))!=EOF){
        printf("%c",ch);
    }
}

void animate(char *s)
{
    int ash = 0;
    int space = 10;
    int num = 0;

    while (ash <= 10)
    {
        printf("\r\t%s [", s);
        for (int i = 0; i < ash; i++)
        {
            printf("#");
        }
        for (int i = 0; i < space; i++)
        {
            printf("-");
        }
        printf("]\t%d %%", num);

        num += 10;
        fflush(stdout);
        ash++;
        space--;
        if (num <= 70)
        {
            usleep(50 * 1000);
        }
        else
        {
            usleep(800 * 1000);
        }
    }
}

#endif