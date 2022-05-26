#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <linux/limits.h>
#define clear() printf("\033[H\033[J")

//hide cursor
#define hide_cursor() printf("\e[?25l")

void enable_raw_mode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void print_current_dir(char path[], int *user_position)
{
    DIR *d;
    struct dirent *dir;
    int i = 0;
    int user_position_upper_bound = 0;
    int user_position_lower_bound = 1;
    d = opendir(path);
    if (d == NULL)
    {
        printf("Unable to open dir\n");
    }
    while ((dir = readdir(d)) != NULL)
    {
        if((strcmp(dir->d_name, ".")))
        {
            if((strcmp(dir->d_name, "..")))
            {
                user_position_upper_bound++;
            }
        }
    }
    d = opendir(path);
    if(*user_position > user_position_upper_bound)
        *user_position = user_position_upper_bound;
    if(*user_position < user_position_lower_bound)
        *user_position = user_position_lower_bound;
    while ((dir = readdir(d)) != NULL)
    {
        if((strcmp(dir->d_name, ".")))
        {
            if((strcmp(dir->d_name, "..")))
            {
                i++;
                //ANYTHING PRINTED AFTER WILL BE RED
                printf("\033[0;31m");
                //yellow on blue background
                printf("\033[33;44m");
                //default colors
                printf("\033[0m");


                if(i == *user_position)
                    printf("\033[0;31m");
                else 
                    printf("\033[0m");

                if(dir->d_type == 4)
                        printf("\033[44;44m");

                printf("%s\n", dir->d_name);
                printf("\033[0m");
            }
        }
    }
}

char *go_up_dir(char *cwd)
{
    int slashes_count = 0;
    int i;
    int j = 0;
    int str_len = strlen(cwd);
    //Calculate number of slashes
    for(i = 0; i < str_len; i++)
    {
        if(cwd[i] == '/')
        {
            slashes_count++;
        }
    }
    if(slashes_count == 1)
    {
        cwd[1] = '\0';
        return cwd;
    }
    //return string with everything after lash slash removed
    for(i = 0; i < str_len; i++)
    {
        cwd[i] = cwd[i];
        if(cwd[i] == '/')
        {
            j++;
        }
        if(slashes_count == j)
            break;
    }
    cwd[i] = '\0';

    return cwd;
}

int main (void)
{
    enable_raw_mode();
    //char path[] = "/";
    //char path2[] = ".";
    int user_position = 1;
    char cwd[PATH_MAX];
    char c;

    hide_cursor();
    clear();
    printf("----------------\n");
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    print_current_dir(cwd, &user_position);
    printf("----------------\n");
    while((c = getchar()) != EOF)
    {
        if(c == 'h')
        {
            user_position = 0;
            clear();
            go_up_dir(cwd);
            printf("----------------\n");
            printf("%s\n", cwd);
           // printf("%s\n", cwd);
            print_current_dir(cwd, &user_position);
            printf("----------------\n");
        }
        if(c == 'j')
        {
            user_position++;
            clear();
            printf("----------------\n");
            printf("%s\n", cwd);
           // printf("%s\n", cwd);
            print_current_dir(cwd, &user_position);
            printf("----------------\n");
        }
        if(c == 'k')
        {
            user_position--;
            clear();
            printf("----------------\n");
            printf("%s\n", cwd);
           // printf("%s\n", cwd);
            print_current_dir(cwd, &user_position);
            printf("----------------\n");
        }

    }



}
