#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

char *curr_dir;

void sh_clear()
{
    system("clear");
}

void sh_pwd()
{
    char cwd[BUFSIZ];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void sh_echo(char **us_direc_inp)
{
    char *arg = us_direc_inp[1];

    // Check if argument starts with '$'
    if (arg[0] == '$')
    {
        const char *env_name = arg + 1; // skip '$'
        char *value = getenv(env_name);
        if (value)
            printf("%s\n", value);
        else
            printf("\n"); // or print a message like "undefined variable"
    }
    else
    {
        for (int i = 0; i < sizeof(arg) / sizeof(arg[0]); i++)
        {
            printf("%s", us_direc_inp[i]);
        }
        printf("\n");
    }
}

void sh_ex()
{
    exit(EXIT_SUCCESS);
}

// us_direc_inp is a pointer to an array of char *
void sh_cd(char **us_direc_inp) // equiv to parsed_str, gets the second string after the command word
{
    // remove surrounding quotes
    // path is now a pointer to the first character in the string ex: "HOME"
    char *path = us_direc_inp[1]; // cd "PATH" wherein '"' is at [1]
    // us_direc_inp[1] is of type char *, and we want path to point to the same string.
    // So we declare path as *path
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

char parse(char us_inp[BUFSIZ])
{
    char *parsed_str[BUFSIZ]; // parsed_str is an array of BUFSIZ pointers to char
    char *us_inp_m, *token;   // var us_inp_m, token are pointers to char
    int i = 0;                // not defining this causes segfaults

    // make str modifiable by duplicating
    us_inp_m = strdup(us_inp);

    // split us_inp_m by spaces
    // execute while as long as it is not NULL
    /*
    char *strsep(char **stringp, const char *delim);
    - returns the next token (as a char *) from the string you’re parsing i.e. us_inp[BUFSIZ].
    - strsep() returns a pointer to the start of the current token, and modifies the original string by
    inserting '\0' (null terminators) in place of delimiters.
    */
    while ((token = strsep(&us_inp_m, " ")) != NULL) // strsep slices between spaces: it moves to next delimeter, replaces with '\0' and returns the token
    {
        // dereferencing: *token access value
        if (*token == '\0')      // there's nothing between a delimeter and a delimeter, returning empty string '\0'
            continue;            // we skip these empty tokens
        parsed_str[i++] = token; // parsed_str[i] = token; i++;
    }
    // after the while loop termindated, i value is updated

    char *builtin[] = {"cd", "exit", "echo", "pwd", "clear"}; // builtin array of each element in it is a pointer to char
    // Each element of builtin[] is a char * — a pointer to the first character of a string.
    char *external[] = {};
    int size_builtin = sizeof(builtin) / sizeof(builtin[0]);
    int command_found = 0;

    // checking if the command is available, if yes, classify and execute
    for (int k = 0; k < size_builtin; k++)
    {
        // print the parsed tokens
        for (int j = 0; j < i; j++)
        {
            // find if command exits in builtin[], returns 0 if equal
            if (strcmp(parsed_str[j], builtin[k]) == 0)
            {
                /*
                Function Pointers
                - func_ptr is an array of pointers to functions, where each function takes a char **
                as an argument and returns void.
                */
                void (*func_ptr[])(char **) = {sh_cd, sh_ex, sh_echo, sh_pwd, sh_clear};
                // k is index in function_ptr[] which matches position of keywords in builtin[]
                func_ptr[k](parsed_str); // parsed_str: an array of char * decays into char **
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
    char cwd[100];       // buffer, array of characters where cwd path will be stored
    char us_inp[BUFSIZ]; // input buffer, constant defined in <stdio.h>, has 512 bytes

    curr_dir = getcwd(cwd, sizeof(cwd)); // gets current working directory
    /* If a function returns T *, you must assign it to a T * variable (or equivalent)
     - getcwd() fills the buffer with path of cwd
     - returns a pointer (char *) to the first character of the filled buffer (cwd[0]) if SUCCESSFUL;
       NULL otherwise
     - the pointer points to the filled buffer holding the current working directory

     Strings in C
     - Is just a pointer to the first character
     - C can’t return entire arrays from functions — only pointers to them.
     - C returns the memory address of where the string starts and tells us to go read it yourself
     - ex: printf follows that pointer, reading one character at a time until it hits the '\0'.

     - thus, curr_dir = cwd
     - it takes a *buf, where the path will be stored
     - and size of that buffer
    */

    while (shell_on == 0)
    {
        printf("MRCSH-AI %s$ ", curr_dir);
        // Format specifier %s in printf print characters at that address, not the address itself
        // %p if address

        // read user input
        fgets(us_inp, sizeof(us_inp), stdin); // fgets() read up to n - 1 characters from the given stream into the array str, and automatically append '\0' at the end.
        us_inp[strcspn(us_inp, "\n")] = 0;    // strcspn aka string complement span, searches for '\n' in us_inp, returns the index, and removes it by setting it to 0

        // parse and execute
        parse(us_inp);
    }
    return 0;
}