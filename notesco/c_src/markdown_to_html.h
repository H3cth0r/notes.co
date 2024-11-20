#ifndef MARKDOWN_TO_HTML_H
#define MARKDOWN_TO_HTML_H

#include <stdio.h>

#define MAX_LINE_LENGTH 1024

char* process_inline_formatting(const char* source);

void parse_markdown_line(const char* line, FILE* output);

#endif 
