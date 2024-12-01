#ifndef MD_PARSER_H
#define MD_PARSER_H

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 1000

typedef enum {
    TEXT,
    HEADING1,
    HEADING2,
    HEADING3,
    HEADING4,
    HEADING5,
    HEADING6,
    TABLE_HEADER,
    TABLE_SEPARATOR,
    TABLE_ROW,
    TEXT_WITH_STYLE
} LineType;

typedef enum {
    STYLE_NONE = 0,
    STYLE_BOLD = 1,
    STYLE_ITALIC = 2
} TextStyle;

typedef struct {
    char *content;
    LineType type;
    TextStyle style;
} MdLine;

typedef struct {
    MdLine lines[MAX_LINES];
    int line_count;
} MdDocument;

MdDocument* parse_markdown_file(const char *filename);
void free_markdown_document(MdDocument *doc);

#endif 