#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

char *curr_dir;

void sh_ex()
{
    exit(EXIT_SUCCESS);
}

void sh_cd(char **us_direc_inp)
{
    // remove surrounding quotes
    char *path = us_direc_inp[1];
    if (path[0] == '"' && path[strlen(path) - 1] == '"')
    {
        path[strlen(path) - 1] = '\0'; // remove trailing quote
        path++;                        // skip leading quote
    }

    struct stat stats;
    printf("Checking path: [%s]\n", us_direc_inp[1]);

    if ((stat(path, &stats) == 0) && (S_ISDIR(stats.st_mode)))
    {
        curr_dir = path;
    }
    else if (us_direc_inp == NULL)
    {
        printf("cd [missing argument]\n");
    }
    else
    {
        printf("PATH is not found\n");
    }
}

char parse(char us_inp[64])
{
    char *parsed_str[64];
    char *us_inp_m, *token;
    int i = 0; // not defining this causes segfaults

    // make str modifiable
    us_inp_m = strdup(us_inp);

    // split us_inp_m by spaces
    // execute while as long as it is not NULL
    while ((token = strsep(&us_inp_m, " ")) != NULL) // // strsep slices between spaces: it moves to next delimeter, replaces with '\0' and returns the token
    {
        if (*token == '\0') // there's nothing between a delimeter and a delimeter, returning empty string '\0'
            continue;       // we skip these empty tokens
        parsed_str[i++] = token;
    }
    // after the while loop termindated, i value is updated

    char *builtin[] = {"cd", "exit", "help", "set", "unset"};
    char *external[] = {};
    int size_builtin = sizeof(builtin) / sizeof(builtin[0]);
    int command_found = 0;

    for (int k = 0; k < size_builtin; k++)
    {
        // print the parsed tokens
        for (int j = 0; j < i; j++)
        {
            if (strcmp(parsed_str[j], builtin[k]) == 0) // returns 0 if equal
            {
                printf("Builtin found: %s\n", builtin[k]);
                void (*func_ptr[])(char **) = {sh_cd, sh_ex};
                func_ptr[k](parsed_str);
                command_found = 1;
            }
        }
    }

    if (!command_found)
    {
        printf("Command is not recognized.\n");
    }

    return 0;
}

int main(void)
{
    int shell_on = 0;
    char cwd[100];
    char us_inp[64];

    curr_dir = getcwd(cwd, sizeof(cwd));

    while (shell_on == 0)
    {
        printf("MRCSH-AI %s$ ", curr_dir);

        // read user input
        fgets(us_inp, sizeof(us_inp), stdin);
        us_inp[strcspn(us_inp, "\n")] = 0; // strcspn aka string complement span, searches for '\n' in us_inp, returns the index, and removes it by setting it to 0

        // parse
        parse(us_inp);
    }
    return 0;
}