#include <helpers.h>
#include <stdio.h>

int main()
{
    char* args[] = {"ls", "/bin", NULL};
    int res = spawn("ls", args);
    printf("res = '%d'", res);
}
