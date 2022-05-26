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
char *get_current_highlighted_file(char path[], int *user_position, char highlighted_name[], int *current_highlighted_file_is_dir)
{
    DIR *d;
    struct dirent *dir;
    int i = 0;
    d = opendir(path);
    while ((dir = readdir(d)) != NULL)
    {
        if((strcmp(dir->d_name, ".")))
        {
            if((strcmp(dir->d_name, "..")))
            {
                i++;
                if(i == *user_position)
                {
                    if(dir->d_type == 4)
                        *current_highlighted_file_is_dir = 1;
                    else
                        *current_highlighted_file_is_dir = 0;

                    strcpy(highlighted_name, dir->d_name);
                }
            }
        }
    }

}

void print_current_dir(char path[], int *user_position, char full_path[], char highlighted_name[], int *current_highlighted_file_is_dir)
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
    get_current_highlighted_file(path, user_position, highlighted_name, current_highlighted_file_is_dir);
    strcpy(full_path, path);
    strcat(full_path, "/");
    strcat(full_path, highlighted_name);
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
                {
                    printf("\033[0;31m");

                }
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
    int user_position = 1;
    char cwd[PATH_MAX];
    char full_path[PATH_MAX];
    char highlighted_name[PATH_MAX];
    int current_highlighted_file_is_dir = 1;
    char c;

    hide_cursor();
    clear();
    getcwd(cwd, sizeof(cwd));
    printf("----------------\n");
    printf("%s\n", cwd);
    printf("----------------\n");
    print_current_dir(cwd, &user_position, full_path, highlighted_name, &current_highlighted_file_is_dir);
    while((c = getchar()) != EOF)
    {
        clear();
        if(c == 'h')
        {
            user_position = 1;
            go_up_dir(cwd);
        }
        if(c == 'l')
        {
            if(current_highlighted_file_is_dir)
            {
                user_position = 1;
                strcpy(cwd, full_path);
            }
        }
        if(c == 'j')
        {
            user_position++;
        }
        if(c == 'k')
        {
            user_position--;
        }
        printf("----------------\n");
        printf("%s\n", cwd);
        printf("----------------\n");
        print_current_dir(cwd, &user_position, full_path, highlighted_name, &current_highlighted_file_is_dir);
    }



}
