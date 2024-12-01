#include <curses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "md_parser.h"
#include "viewer.h"

#define MAX_FILES 100
#define MAX_FILENAME 256
#define max(a,b) ((a) > (b) ? (a) : (b))

void get_markdown_files(char *files[], int *file_count) {
    DIR *dir;
    struct dirent *ent;
    *file_count = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Cannot open directory");
        exit(1);
    }

    while ((ent = readdir(dir)) != NULL && *file_count < MAX_FILES) {
        if (strstr(ent->d_name, ".md") != NULL) {
            files[*file_count] = malloc(MAX_FILENAME);
            strncpy(files[*file_count], ent->d_name, MAX_FILENAME - 1);
            (*file_count)++;
        }
    }
    closedir(dir);
}

int main(void) {
    char *files[MAX_FILES];
    int file_count = 0;
    get_markdown_files(files, &file_count);

    if (file_count == 0) {
        fprintf(stderr, "No markdown files found in current directory\n");
        return 1;
    }

    // Initialize ncurses
    initscr();
    start_color();
    use_default_colors();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);  // Hide cursor

    // Initialize color pairs
    init_viewer_colors();

    int current_file = 0;
    int scroll_pos = 0;

    while (1) {
        MdDocument *doc = parse_markdown_file(files[current_file]);
        display_document(doc, files[current_file], scroll_pos);

        int ch = getch();
        switch (ch) {
            case KEY_UP:
                scroll_pos = (int)fmax(0, scroll_pos - 1);
                break;
            case KEY_DOWN:
                scroll_pos++;
                break;
            case KEY_LEFT:
                if (current_file > 0) {
                    current_file--;
                    scroll_pos = 0;
                }
                break;
            case KEY_RIGHT:
                if (current_file < file_count - 1) {
                    current_file++;
                    scroll_pos = 0;
                }
                break;
            case 'q':
                goto cleanup;
        }
        free_markdown_document(doc);
    }

cleanup:
    endwin();
    for (int i = 0; i < file_count; i++) {
        free(files[i]);
    }
    return 0;
} 