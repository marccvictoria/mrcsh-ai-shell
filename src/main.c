#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int shell_on = 0;
    char cwd[100];
    int inp;

    while (shell_on == 0)
    {
        char *curr_dir = getcwd(cwd, sizeof(cwd));

        printf("MRCSH %s$ ", curr_dir);
        scanf("%d", &inp);
        printf("%d\n", inp);
    }
    return 0;
}