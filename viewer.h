#ifndef VIEWER_H
#define VIEWER_H

#include "md_parser.h"

void init_viewer_colors(void);
void display_document(const MdDocument *doc, const char *filename, int scroll_pos);

#endif 