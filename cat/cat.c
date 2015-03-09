#include"../lib/helpers.h"
#include<stdio.h>
int main()
{
    char buf[50];
    int c = read_(STDIN_FILENO, buf, 0);
    write_(STDOUT_FILENO, buf, c);
    return 0;
}
