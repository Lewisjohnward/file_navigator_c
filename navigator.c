#define _XOPEN_SOURCE 500

#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

#define clear() printf("\033[H\033[J")

//hide cursor
#define hide_cursor() printf("\e[?25l")

struct termios original;

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &original);
    struct termios raw = original;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    printf("\033[0m");
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
                if(dir->d_name[0] == '.' && show_hidden_files)
                    i++;
                else if (dir->d_name[0] != '.')
                    i++;

                if(i == *user_position)
                {
                    if(dir->d_type == 4)
                    {
                        if(dir->d_name[0] == '.' && show_hidden_files)
                            *current_highlighted_file_is_dir = 1;
                        else if (dir->d_name[0] != '.')
                            *current_highlighted_file_is_dir = 1;
                    }
                    else
                    {
                        if(dir->d_name[0] == '.' && show_hidden_files)
                            *current_highlighted_file_is_dir = 0;
                        else if (dir->d_name[0] != '.')
                            *current_highlighted_file_is_dir = 0;
                    }

                    if(dir->d_name[0] == '.' && show_hidden_files)
                        strcpy(highlighted_name, dir->d_name);
                    else if (dir->d_name[0] != '.')
                        strcpy(highlighted_name, dir->d_name);

                }
            }
        }
    }

}

void center_vertically(int num)
{
    int i;
    for(i = 0; i < num; i++)
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

void print_current_dir(
        char path[], 
        int *user_position, 
        char full_path[], 
        char highlighted_name[], 
        int *current_highlighted_file_is_dir, 
        int *min_visible_files, 
        int range_visible, 
        int show_hidden_files, 
        int accepting_user_input, 
        char *user_command
        )
{
    DIR *d;
    struct dirent *dir;
    int i = 0;
    int user_position_upper_bound = 0;
    int user_position_lower_bound = 1;
    int file_count = 0;
    clear();
    center_vertically(5);
    printf("\t\t----------------\n");
    printf("\t\t%s\n", path);
    printf("\t\t----------------\n");
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
                    if(dir->d_name[0] == '.' && show_hidden_files)
                    {
                        user_position_upper_bound++;
                    } else if(dir ->d_name[0] != '.')
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
                    if(dir->d_name[0] == '.' && show_hidden_files)
                    {
                        i++;
                    } else if(dir ->d_name[0] != '.')
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
                        printf("\033[44;44m");
                        //count_children_files(path, dir->d_name, &file_count);
                        if(dir->d_name[0] == '.' && show_hidden_files)
                        {
                            printf("\t\t%-2d %s %-*s %d\n", i, ":", 45, dir->d_name, file_count);
                        } else if(dir ->d_name[0] != '.')
                            printf("\t\t%-2d %s %-*s %d\n", i, ":", 45, dir->d_name, file_count);
                            

                    }
                    else 
                    {
                        if(dir->d_name[0] == '.' && show_hidden_files)
                        {
                            printf("\t\t%-2d %s %-*s\n", i, ":", 45, dir->d_name);
                        } else if(dir ->d_name[0] != '.')
                            printf("\t\t%-2d %s %-*s\n", i, ":", 45, dir->d_name);
                    }
                        printf("\033[0m");
                    file_count = 0;
                }
            }
        }
    }
    if(accepting_user_input)
        printf(": %s\n", user_command);
    
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

void handle_move_command(char *path, char c, int *user_position, int *command_bar_active)
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
}

void make_dir(char *path, char *name)
{
    char make_dir_buffer[PATH_MAX];
    strcpy(make_dir_buffer, path);
    strcat(make_dir_buffer, "/");
    strcat(make_dir_buffer, name);
    mkdir(make_dir_buffer, 0777);
}
void make_file(char *path, char *name)
{
    FILE *fp;
    char make_file_buffer[PATH_MAX];
    strcpy(make_file_buffer, path);
    strcat(make_file_buffer, "/");
    strcat(make_file_buffer, name);
    fp = fopen(make_file_buffer, "w");
    fclose(fp);
}

void handle_create_file_command(char *path, char c, int *user_position, int *command_bar_active, int *accepting_user_input, int *create_new_folder)
{
    
    switch(c)
    {
        case 'f':
            *create_new_folder = 2;
            *accepting_user_input = 1;
            break;
        case 'd':
            *create_new_folder = 1;
            *accepting_user_input = 1;
            break;
    }
}


int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(char *path)
{
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

void handle_delete_file_command(char *path, char c, int *command_bar_active, int *accepting_user_input, int *create_new_folder, char *full_path)
{
    //rmrf(full_path);
    switch(c)
    {
        case 'd':
            *create_new_folder = 3;
            break;
    }
}



void handle_command(char *path, char c, int *user_position, int *command_bar_active, int *accepting_user_input, int *create_new_folder, char *full_path)
{
    switch(*command_bar_active)
    {
        case 1:
            handle_move_command(path, c, user_position, command_bar_active);
            break;
        case 2:
            handle_create_file_command(path, c, user_position, command_bar_active, accepting_user_input, create_new_folder);
            break;
        case 3:
            handle_delete_file_command(path, c, command_bar_active, accepting_user_input, create_new_folder, full_path);
            break;
    }
    *command_bar_active = 0;
}

void toggle_command_bar(int *command_bar_active, char c)
{
    switch(c)
    {
        case 'g':
            *command_bar_active = 1;
            break;
        case 'c':
            *command_bar_active = 2;
            break;
        case 'd':
            *command_bar_active = 3;
            break;
        case '?':
            *command_bar_active = 4;
            break;
        default:
            *command_bar_active = 0;
    }
}

void toggle_hidden_files(int *show_hidden_files)
{
    if(*show_hidden_files)
        *show_hidden_files = 0;
    else
        *show_hidden_files = 1;
}

void toggle_select_file(char *highlighted_files[], int *highlighted_files_count, const char *highlighted_name_full_path)
{
    char temp_highlighted_files[10];
    int count = *highlighted_files_count;
    int i = 0;
    int entry_removed = 1;
    for(i = 0; i < count; i++)
        if(strcmp(highlighted_files[i], highlighted_name_full_path) == 0)
        {
            *highlighted_files_count -= 1;
            entry_removed = 0;
        }

    if(entry_removed)
    {
        char *str_ptr = (char *) malloc(strlen(highlighted_name_full_path) + 1);
        strcpy(str_ptr, highlighted_name_full_path);
        highlighted_files[*highlighted_files_count] = str_ptr;
        *highlighted_files_count += 1;
    }
}

void print_selected_files(char *highlighted_files[], int *highlighted_files_count, const char *highlighted_name_full_path)
{
    int i = 0;
    printf("\n");
    printf("\t\tSelected files\n");
    printf("\t\t-----------------\n");
    
    printf("\t\t highlighted name: %s\n", highlighted_name_full_path);
    printf("\t\t highlighted file count: %d\n", *highlighted_files_count);

    for(; i < *highlighted_files_count; i++)
        printf("\t\t%s\n", highlighted_files[i]);
}

void handle_open_file(char *highlighted_files[], int *highlighted_files_count, const char *highlighted_name_full_path)
{
    pid_t pid;
    int w = 0;
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
    {
        wait(&w);
        printf("hello\n");
    }
    if (pid == 0)
    {
        char *args[] = {"vim", highlighted_files[0], NULL};
        execvp(args[0], args);
    }
    
}


void handle_input(
        char c, 
        char *highlighted_files[], 
        int *highlighted_files_count, 
        const char *highlighted_name_full_path,
        int *user_position, 
        int current_highlighted_file_is_dir, 
        char path[], 
        char full_path[], 
        int *command_bar_active, 
        int *show_hidden_files, 
        int *accepting_user_input, 
        int *create_new_folder)
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
        if(c == 'g' || c == 'c' || c == 'd' || c == '?')
        {
            toggle_command_bar(command_bar_active, c);
        }
        //ctrl -h
        if(c == 8)
            toggle_hidden_files(show_hidden_files);
        //space
        if(c == 32)
            toggle_select_file(highlighted_files, highlighted_files_count, highlighted_name_full_path);
        //enter
        if(c == 10)
            handle_open_file(highlighted_files, highlighted_files_count, highlighted_name_full_path);
    }
    else
    {
        handle_command(path, c, user_position, command_bar_active, accepting_user_input, create_new_folder, full_path);
    }
}

void print_jump_commands()
{
    printf("\t\tg -> go to top");
    printf("\n");
    printf("\t\tG -> go to bottom");
    printf("\n");
    printf("\t\th -> go to home dir");
    printf("\n");
    printf("\t\t/ -> go to root");
    printf("\n");
}

void print_create_file_commands()
{
    printf("\t\td -> Create new dir");
    printf("\n");
    printf("\t\tf -> Create new file");
    printf("\n");
}

void print_delete_file_commands()
{
    printf("\t\td -> delete file");
    printf("\n");
}
void print_help_commands()
{
    printf("\t\td -> help commands");
    printf("\n");

}

void print_commands(int command_bar_active)
{
    center_vertically(2);
    switch(command_bar_active)
    {
        case 1:
            print_jump_commands();
            break;
        case 2:
            print_create_file_commands();
            break;
        case 3:
            print_delete_file_commands();
            break;
        case 4:
            print_help_commands();
            break;
    }
}
void handle_user_command_input(char c, int *user_command_position, char *user_command, int *accepting_user_input, int *handle_command)
{
    if(c == 127)
    {
        *user_command_position -= 1;
        user_command[*user_command_position] = '\0';
    }
    else if(c == 10)
    {
        *accepting_user_input = 0;
        *handle_command = 1;
    } else
    {
        user_command[*user_command_position] = c;
        *user_command_position += 1;
        user_command[*user_command_position] = '\0';
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


    int accepting_user_input = 0;
    char user_command[20];
    int user_command_position = 0;

    int create_new_folder = 0;
    int handle_command =  0;

    char *highlighted_files[10];
    int highlighted_files_count = 0;

    char c;

    getcwd(cwd, sizeof(cwd));
    do
    {
        if(accepting_user_input)
        {
            handle_user_command_input(c, &user_command_position, user_command, &accepting_user_input, &handle_command);
        }else 
            handle_input(c,highlighted_files, &highlighted_files_count, full_path, &user_position, current_highlighted_file_is_dir, cwd, full_path, &command_bar_active, &show_hidden_files, &accepting_user_input, &create_new_folder);

        if (handle_command)
        {
            if(create_new_folder == 1)
            {
                make_dir(cwd, user_command);
            }
            if(create_new_folder == 2)
            {
                make_file(cwd, user_command);
            }
            user_command_position = 0;
            user_command[0] = '\0';
            create_new_folder = 0;
            handle_command = 0;
        }
        if(create_new_folder == 3)
        {
            rmrf(full_path);
            create_new_folder = 0;
        }
        print_current_dir(cwd, &user_position, full_path, highlighted_name, &current_highlighted_file_is_dir, &min_visible_files, range_visible, show_hidden_files, accepting_user_input, user_command);
        print_commands(command_bar_active);
        print_selected_files(highlighted_files, &highlighted_files_count, full_path);
    } while((c = getchar()) != EOF && c != 'q');
    clear();

    disable_raw_mode();
    return 0;
}
