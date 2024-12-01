#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "md_parser.h"

static int is_table_separator(const char *line) {
    if (*line != '|') return 0;
    line++;
    while (*line) {
        if (*line != ' ' && *line != '-' && *line != '|') return 0;
        line++;
    }
    return 1;
}

static int is_table_row(const char *line) {
    return line[0] == '|';
}

static TextStyle determine_style(const char *line) {
    TextStyle style = STYLE_NONE;
    
    // Check for bold (either **text** or __text__)
    if ((strstr(line, "**") != NULL && strstr(strstr(line, "**") + 2, "**") != NULL) ||
        (strstr(line, "__") != NULL && strstr(strstr(line, "__") + 2, "__") != NULL)) {
        style |= STYLE_BOLD;
    }
    
    // Check for italic (either *text* or _text_)
    if ((strstr(line, "*") != NULL && strstr(strstr(line, "*") + 1, "*") != NULL) ||
        (strstr(line, "_") != NULL && strstr(strstr(line, "_") + 1, "_") != NULL)) {
        style |= STYLE_ITALIC;
    }
    
    return style;
}

static LineType determine_line_type(const char *line) {
    // First check for tables
    if (is_table_separator(line)) return TABLE_SEPARATOR;
    if (is_table_row(line)) {
        static int last_was_header = 0;
        if (last_was_header) {
            last_was_header = 0;
            return TABLE_SEPARATOR;
        }
        if (strstr(line, "---") != NULL) {
            last_was_header = 1;
            return TABLE_HEADER;
        }
        return TABLE_ROW;
    }

    // Check for headings (existing code)
    int heading_level = 0;
    while (line[heading_level] == '#' && heading_level < 6) {
        heading_level++;
    }
    
    if (heading_level > 0 && (line[heading_level] == ' ' || line[heading_level] == '\t')) {
        switch (heading_level) {
            case 1: return HEADING1;
            case 2: return HEADING2;
            case 3: return HEADING3;
            case 4: return HEADING4;
            case 5: return HEADING5;
            case 6: return HEADING6;
        }
    }
    
    // Check for styled text
    if (determine_style(line) != STYLE_NONE) {
        return TEXT_WITH_STYLE;
    }
    
    return TEXT;
}

MdDocument* parse_markdown_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    MdDocument *doc = malloc(sizeof(MdDocument));
    doc->line_count = 0;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && doc->line_count < MAX_LINES) {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        doc->lines[doc->line_count].content = strdup(line);
        doc->lines[doc->line_count].type = determine_line_type(line);
        doc->line_count++;
    }

    fclose(file);
    return doc;
}

void free_markdown_document(MdDocument *doc) {
    if (!doc) return;
    
    for (int i = 0; i < doc->line_count; i++) {
        free(doc->lines[i].content);
    }
    free(doc);
} 