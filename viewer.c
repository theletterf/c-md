#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "viewer.h"

#define COLOR_TITLE 1
#define COLOR_STATUS 2
#define COLOR_H1 3
#define COLOR_H2 4
#define COLOR_H3 5
#define COLOR_H4 6
#define COLOR_H5 7
#define COLOR_H6 8
#define COLOR_BOLD 9
#define COLOR_ITALIC 10
#define COLOR_TABLE_BORDER 11
#define COLOR_TABLE_HEADER 12

void init_viewer_colors(void) {
    init_pair(COLOR_TITLE, COLOR_WHITE, COLOR_BLUE);
    init_pair(COLOR_STATUS, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_H1, COLOR_RED, -1);
    init_pair(COLOR_H2, COLOR_YELLOW, -1);
    init_pair(COLOR_H3, COLOR_GREEN, -1);
    init_pair(COLOR_H4, COLOR_CYAN, -1);
    init_pair(COLOR_H5, COLOR_MAGENTA, -1);
    init_pair(COLOR_H6, COLOR_BLUE, -1);
    init_pair(COLOR_BOLD, COLOR_WHITE, -1);
    init_pair(COLOR_ITALIC, COLOR_CYAN, -1);
    init_pair(COLOR_TABLE_BORDER, COLOR_BLUE, -1);
    init_pair(COLOR_TABLE_HEADER, COLOR_GREEN, -1);
}

static void render_table_row(const char *content, int y, int x, int max_x, int is_header) {
    char *line = strdup(content);
    char *token = strtok(line, "|");
    int pos = x;
    
    attron(COLOR_PAIR(COLOR_TABLE_BORDER));
    mvaddch(y, pos++, '|');
    attroff(COLOR_PAIR(COLOR_TABLE_BORDER));
    
    while (token && pos < max_x - 1) {
        // Trim whitespace
        while (isspace(*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace(*end)) end--;
        *(end + 1) = '\0';
        
        if (is_header) {
            attron(COLOR_PAIR(COLOR_TABLE_HEADER) | A_BOLD);
        }
        
        mvprintw(y, pos, " %s ", token);
        pos += strlen(token) + 2;
        
        if (is_header) {
            attroff(COLOR_PAIR(COLOR_TABLE_HEADER) | A_BOLD);
        }
        
        attron(COLOR_PAIR(COLOR_TABLE_BORDER));
        mvaddch(y, pos++, '|');
        attroff(COLOR_PAIR(COLOR_TABLE_BORDER));
        
        token = strtok(NULL, "|");
    }
    
    free(line);
}

static void render_styled_text(const char *content, int y, int x, TextStyle style) {
    if (style & STYLE_BOLD) {
        attron(COLOR_PAIR(COLOR_BOLD) | A_BOLD);
    }
    if (style & STYLE_ITALIC) {
        attron(COLOR_PAIR(COLOR_ITALIC) | A_ITALIC);
    }
    
    mvprintw(y, x, "%s", content);
    
    if (style & STYLE_BOLD) {
        attroff(COLOR_PAIR(COLOR_BOLD) | A_BOLD);
    }
    if (style & STYLE_ITALIC) {
        attroff(COLOR_PAIR(COLOR_ITALIC) | A_ITALIC);
    }
}

void display_document(const MdDocument *doc, const char *filename, int scroll_pos) {
    clear();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Display title bar
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
    mvhline(0, 0, ' ', max_x);
    mvprintw(0, (max_x - strlen(filename)) / 2, "%s", filename);
    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    // Display content
    int display_line = 1;
    for (int i = scroll_pos; i < doc->line_count && display_line < max_y - 1; i++) {
        int color_pair = 0;
        
        switch (doc->lines[i].type) {
            case TABLE_HEADER:
            case TABLE_ROW:
                render_table_row(doc->lines[i].content, display_line, 0, max_x,
                               doc->lines[i].type == TABLE_HEADER);
                break;
            
            case TABLE_SEPARATOR:
                // Skip separator lines in display
                continue;
            
            case TEXT_WITH_STYLE:
                render_styled_text(doc->lines[i].content, display_line, 0,
                                 doc->lines[i].style);
                break;
            
            case HEADING1: color_pair = COLOR_H1; goto render_normal;
            case HEADING2: color_pair = COLOR_H2; goto render_normal;
            case HEADING3: color_pair = COLOR_H3; goto render_normal;
            case HEADING4: color_pair = COLOR_H4; goto render_normal;
            case HEADING5: color_pair = COLOR_H5; goto render_normal;
            case HEADING6: color_pair = COLOR_H6; goto render_normal;
            case TEXT:
            default:
                render_normal:
                if (color_pair) {
                    attron(COLOR_PAIR(color_pair) | A_BOLD);
                }
                
                mvprintw(display_line, 0, "%s", doc->lines[i].content);
                
                if (color_pair) {
                    attroff(COLOR_PAIR(color_pair) | A_BOLD);
                }
                break;
        }
        display_line++;
    }

    // Display status bar
    attron(COLOR_PAIR(COLOR_STATUS));
    mvhline(max_y - 1, 0, ' ', max_x);
    mvprintw(max_y - 1, 0, "Use arrows to navigate | q to quit");
    attroff(COLOR_PAIR(COLOR_STATUS));

    refresh();
} 