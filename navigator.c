#include <ctype.h>
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
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
char *get_current_highlighted_file(char path[], int *user_position, char highlighted_name[], int *current_highlighted_file_is_dir, int show_hidden_files)
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

void center_vertically()
{
    int i;
    for(i = 0; i < 5; i++)
        printf("\n");
}

void count_children_files(char path[], char folder_name[], int *file_count)
{
    DIR *d;
    struct dirent *dir;
    int i = 0;
    char full_path[PATH_MAX];
    strcpy(full_path, path);
    strcat(full_path, "/");
    strcat(full_path, folder_name);
    d = opendir(full_path);
    while ((dir = readdir(d)) != NULL)
    {
        if(*file_count < 9)
        {
            if((strcmp(dir->d_name, ".")))
                if((strcmp(dir->d_name, "..")))
                {
                    *file_count += 1;
                    i++;
                    //printf("%s\n", dir->d_name);
                }
        }

    }
}

void print_current_dir(char path[], int *user_position, char full_path[], char highlighted_name[], int *current_highlighted_file_is_dir, int *min_visible_files, int range_visible, int show_hidden_files)
{
    DIR *d;
    struct dirent *dir;
    int i = 0;
    int user_position_upper_bound = 0;
    int user_position_lower_bound = 1;
    int file_count = 0;
    clear();
    center_vertically();
    printf("\t----------------\n");
    printf("\t%s\n", path);
    printf("\t----------------\n");
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
    get_current_highlighted_file(path, user_position, highlighted_name, current_highlighted_file_is_dir, show_hidden_files);
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

                if(i <= range_visible)
                {
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
                    {
                        //count_children_files(path, dir->d_name, &file_count);
                        printf("\033[44;44m");
                        printf("\t%-2d %s %-*s %d\n", i, ":", 45, dir->d_name, file_count);
                    }
                    else 
                    {
                        printf("\t%-2d %s %-*s\n", i, ":", 45, dir->d_name);
                    }
                        printf("\033[0m");
                    file_count = 0;
                }
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
void handle_command(char *path, char c, int *user_position, int *command_bar_active)
{
    const char *s = getenv("HOME");
    switch(c)
    {
        case 'g':
            *user_position = 0;
            break;
        case 'h':
            strcpy(path, s);
            break;
        case '/':
            strcpy(path, "/");
            break;
        case 'G':
            *user_position = 100;
            break;
    }
    *command_bar_active = 0;
}

void toggle_command_bar(int *command_bar_active)
{
    if(*command_bar_active)
        *command_bar_active = 0;
    else
        *command_bar_active = 1;
}

void toggle_hidden_files(int *show_hidden_files)
{
    if(*show_hidden_files)
        *show_hidden_files = 0;
    else
        *show_hidden_files = 1;
}

void handle_input(char c, int *user_position, int current_highlighted_file_is_dir, char path[], char full_path[], int *command_bar_active, int *show_hidden_files)
{
    if(!*command_bar_active)
    {
        if(c == 'h')
        {
            *user_position = 1;
            go_up_dir(path);
        }
        if(c == 'l' && current_highlighted_file_is_dir)
        {
            *user_position = 1;
            strcpy(path, full_path);
        }
        if(c == 'j')
        {
            *user_position += 1;
        }
        if(c == 'k')
        {
            *user_position -= 1;
        }
        if(c == 'g' || c == 'g')
        {
            toggle_command_bar(command_bar_active);
        }
        if(c == 8)
            toggle_hidden_files(show_hidden_files);
    }
    else
    {
        handle_command(path, c, user_position, command_bar_active);
    }
}

void print_jump_commands()
{
    center_vertically();
    printf("g -> go to top");
    printf("\n");
    printf("G -> go to bottom");
    printf("\n");
    printf("h -> go to home dir");
    printf("\n");
    printf("/ -> go to root");
    printf("\n");
}

void print_commands(int command_bar_active)
{
    switch(command_bar_active)
    {
        case 1:
            print_jump_commands();
            break;
        case 2:
            print_jump_commands;
            break;
    }
}

int main (void)
{
    enable_raw_mode();
    hide_cursor();

    int user_position = 1;
    char cwd[PATH_MAX];
    char full_path[PATH_MAX];
    char highlighted_name[PATH_MAX];
    int current_highlighted_file_is_dir = 1;
    int min_visible_files = 0;
    int range_visible = 45;
    int command_bar_active = 0;
    int show_hidden_files = 0;

    char c;

    getcwd(cwd, sizeof(cwd));
    print_current_dir(cwd, &user_position, full_path, highlighted_name, &current_highlighted_file_is_dir, &min_visible_files, range_visible, show_hidden_files);
    while((c = getchar()) != EOF && c != 'q')
    {
        handle_input(c, &user_position, current_highlighted_file_is_dir, cwd, full_path, &command_bar_active, &show_hidden_files);
        clear();
        print_current_dir(cwd, &user_position, full_path, highlighted_name, &current_highlighted_file_is_dir, &min_visible_files, range_visible, show_hidden_files);
        print_commands(command_bar_active);

    }
    clear();
    return 0;
}
