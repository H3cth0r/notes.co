#include "markdown_to_html.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char* process_inline_formatting(const char* source) {
    static char buffer[MAX_LINE_LENGTH];
    char* dest = buffer;
    const char* src = source;

    int bold_open = 0;
    int italic_open = 0;

    while (*src) {
        if (*src == '*' && *(src + 1) == '*') {
            if (bold_open) {
                strcpy(dest, "</b>");
                dest += 4;
                bold_open = 0;
            } else {
                strcpy(dest, "<b>");
                dest += 3;
                bold_open = 1;
            }
            src += 2; // skip **
        } else if (*src == '*') {
            if (italic_open) {
                strcpy(dest, "</i>");
                dest += 4;
                italic_open = 0;
            } else {
                strcpy(dest, "<i>");
                dest += 3;
                italic_open = 1;
            }
            src++;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
    return buffer;
}

void parse_markdown_line(const char* line, FILE* output) {
    static int in_list = 0;       // track if we're inside a list
    static int list_stack[10] = {0}; // stack to track list types at each level
    static int stack_depth = 0;    // current depth of nested lists
    static int current_list_type = -1; // current main list type (-1 = none, 0 = ul, 1 = ol)
    static int last_level = 0;    // track last indentation level
    static int in_paragraph = 0;        // track if currently on simple paragraph

    int indent = 0;
    const char* trimmed = line;

    // determine indentation level
    while (*trimmed == ' ') {
        indent++;
        trimmed++;
    }

    // skip empty lines
    // if (*trimmed == '\n' || *trimmed == '\0') {
    //     fprintf(output, "\n");
    //     return;
    // }

    // detect list item
    int is_list_item = 0;
    int new_list_type = -1;
    int marker_length = 0;

    if (*trimmed == '-' || *trimmed == '*') {
        marker_length = 1;
        is_list_item = 1;
        new_list_type = 0; // unordered list
    } else if (isdigit(*trimmed)) {
        const char* dot = trimmed;
        while (isdigit(*dot)) dot++;
        if (*dot == '.') {
            marker_length = dot - trimmed + 1;
            is_list_item = 1;
            new_list_type = 1; // ordered list
        }
    }

    if (is_list_item && isspace(*(trimmed + marker_length))) {
        trimmed += marker_length + 1;  // Skip marker and space
        int level = indent / 2;  // Assume 2-space indentation

        // start new list if needed
        if (!in_list) {
            fprintf(output, "%s\n", new_list_type == 1 ? "<ol>" : "<ul>");
            current_list_type = new_list_type;
            list_stack[0] = new_list_type;
            stack_depth = 1;
            in_list = 1;
        }
        // handle nested lists
        else if (level > last_level) {
            // only create one new list level regardless of indent difference
            fprintf(output, "%s\n", new_list_type == 1 ? "<ol>" : "<ul>");
            list_stack[stack_depth] = new_list_type;
            stack_depth++;
        }
        // Handle moving back to previous level
        else if (level < last_level) {
            // close one list level
            fprintf(output, "%s\n", list_stack[stack_depth - 1] == 1 ? "</ol>" : "</ul>");
            stack_depth--;
            
            // if we're back at root level and list type changes
            if (level == 0 && new_list_type != current_list_type) {
                while (stack_depth > 0) {
                    fprintf(output, "%s\n", list_stack[stack_depth - 1] == 1 ? "</ol>" : "</ul>");
                    stack_depth--;
                }
                fprintf(output, "%s\n", new_list_type == 1 ? "<ol>" : "<ul>");
                current_list_type = new_list_type;
                list_stack[0] = new_list_type;
                stack_depth = 1;
            }
        }
        // same level but different list type (only for root level)
        else if (level == 0 && new_list_type != current_list_type) {
            while (stack_depth > 0) {
                fprintf(output, "%s\n", list_stack[stack_depth - 1] == 1 ? "</ol>" : "</ul>");
                stack_depth--;
            }
            fprintf(output, "%s\n", new_list_type == 1 ? "<ol>" : "<ul>");
            current_list_type = new_list_type;
            list_stack[0] = new_list_type;
            stack_depth = 1;
        }

        // output the list item
        fprintf(output, "<li>%s</li>\n", process_inline_formatting(trimmed));
        last_level = level;
    } else if (in_list) {
        // close all open lists when transitioning to non-list content
        while (stack_depth > 0) {
            fprintf(output, "%s\n", list_stack[stack_depth - 1] == 1 ? "</ol>" : "</ul>");
            stack_depth--;
        }
        in_list = 0;
        current_list_type = -1;
        last_level = 0;

        // process the non-list content
        if (line[0] == '#') {
            int level = 0;
            while (line[level] == '#') level++;
            fprintf(output, "<h%d>%s</h%d>\n", level, 
                   process_inline_formatting(line + level + 1), level);
        } else {
            // normal paragraph
            if (in_paragraph == 0) {
                in_paragraph = 1;
                fprintf(output, "<p>%s", process_inline_formatting(line));
            } else {
                if (line[0] == '\n') {
                    in_paragraph = 0;
                    fprintf(output, "</p>%s", process_inline_formatting(line));
                } else {
                    fprintf(output, "%s", process_inline_formatting(line));
                }
            }
        }
    } else {
        // regular non-list content
        if (line[0] == '#') {
            int level = 0;
            while (line[level] == '#') level++;
            fprintf(output, "<h%d>%s</h%d>\n", level, 
                   process_inline_formatting(line + level + 1), level);
        } else {
            // normal paragraph
            if (in_paragraph == 0) {
                in_paragraph = 1;
                fprintf(output, "<p>%s", process_inline_formatting(line));
            } else {
                if (line[0] == '\n') {
                    in_paragraph = 0;
                    fprintf(output, "</p>%s", process_inline_formatting(line));
                } else {
                    fprintf(output, "%s", process_inline_formatting(line));
                }
            }
            // printf("%d ", line[0]);
            // fprintf(output, "<p>%s</p>\n", process_inline_formatting(line));
        }
    }
}
