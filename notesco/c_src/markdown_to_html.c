#include "markdown_to_html.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

void print_buffer_size(size_t buffer_size) {
    double size_in_mb = (double)buffer_size / (1024 * 1024);  // convert bytes to MB
    printf("Buffer size: %.2f MB\n", size_in_mb);
}

// to detect if a line start with backticks
int is_code_block_delimeter(const char* line) {
    int backticks_count = 0;
    while (*line == '`') {
        backticks_count++;
        line++;
    }
    return backticks_count >= 3;
}

// function to extract language specification
char* get_language_spec(const char* line) {
    static char lang_buffer[32];
    const char* start = line;

    // skip backticks
    while (*start == '`') start++;

    // copy lang name
    int i = 0;
    while (*start && !isspace(*start) && *start != '\n' && i < 31) {
        lang_buffer[i++] = *start++;
    }
    lang_buffer[i] = '\0';

    return i > 0 ? lang_buffer : NULL;
}

char* preserve_indentation(const char* line) {
    static char indent_buffer[MAX_LINE_LENGTH];
    char* dest = indent_buffer;

    // count leading spaces
    while (*line == ' ' || *line == '\t') {
        if (*line == ' ') {
            strcpy(dest, "&nbsp;");
            dest += 6;
        } else {
            strcpy(dest, "&nbsp;&nbsp;&nbsp;&nbsp;");
            dest += 24;
        }
        line++;
    }

    // copy rest of the line
    while (*line) {
        if (*line == '<') {
            strcpy(dest, "&lt;");
            dest += 4;
        } else if (*line == '>') {
            strcpy(dest, "&gt;");
            dest += 4;
        } else if (*line == '&') {
            strcpy(dest, "&amp;");
            dest += 5;
        } else {
            *dest++ = *line;
        }
        line++;
    }
    *dest = '\0';

    return indent_buffer;
}

// function to handle code block parsing
void process_code_block(FILE* input, FILE* output, const char* language) {
    char line_buffer[MAX_LINE_LENGTH];
    char code_buffer[MAX_CODE_BUFFER] = "";
    size_t total_length = 0;

    // start code block with language class if specified
    if (language && *language) {
        fprintf(output, "<pre><code class=\"language-%s\">\n", language);
    } else {
        fprintf(output, "<pre><code>\n");
    }

    // read lines until closing code block is found
    while (fgets(line_buffer, sizeof(line_buffer), input)) {
        if (is_code_block_delimeter(line_buffer)) {
            break;
        }

        // preserve indentation and scape html special characters
        char* processed_line = preserve_indentation(line_buffer);

        // append to code buffer if there's space
        size_t line_length = strlen(processed_line);
        if (total_length + line_length < MAX_CODE_BUFFER - 1) {
            strcat(code_buffer, processed_line);
            total_length += line_length;
        }
    }

    // output the process_code_block
    fprintf(output, "%s", code_buffer);
    fprintf(output, "</code></pre>\n");
}

// Base64 encoding function
char* base64_encode(const uint8_t* data, size_t input_length, size_t* output_length) {
    const char base64_chars[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    *output_length = 4 * ((input_length + 2) / 3);
    char* encoded_data = malloc(*output_length + 1);
    if (!encoded_data)  {
        return NULL;
    }
    
    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded_data[j++] = base64_chars[(triple >> 18) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 12) & 0x3F];
        encoded_data[j++] = base64_chars[(triple >> 6) & 0x3F];
        encoded_data[j++] = base64_chars[triple & 0x3F];
    }

    // handle padding
    while (j < *output_length) {
        encoded_data[j++] = '=';
    }
    // encoded_data[*output_length] = '\0';
    encoded_data[j] = '\0';

    return encoded_data;
}

// file to base64 conversion function
char* file_to_base64(const char* filename, size_t* base64_length) {
    printf("%s\n", filename);
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    // get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // allocate buffer
    uint8_t* buffer = malloc(file_size);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    // read file
    size_t read_size = fread(buffer, 1, file_size, file);
    fclose(file);

    if (read_size != file_size) {
        free(buffer);
        return NULL;
    }

    // convert to base64
    char* base64 = base64_encode(buffer, read_size, base64_length);
    free(buffer);

    return base64;
}

char* process_image_block(const char* source) {
    // find label
    const char* label_start = source + 2;  // skip ![ 
    const char* label_end = strchr(label_start, ']');
    
    if (!label_end || *(label_end + 1) != '(') {
        return NULL;
    }

    // find URL
    const char* url_start = label_end + 2;
    const char* url_end = strchr(url_start, ')');
    
    if (!url_end) {
        return NULL;
    }
    size_t url_length = url_end - url_start;

    // check if local file
    if (strncmp(url_start, "http", 4) != 0) {
        // local file - convert to base64
        size_t base64_length;

        char local_url[1024];
        strncpy(local_url, url_start, url_length);
        local_url[url_length] = '\0';

        char* base64_data = file_to_base64(local_url, &base64_length);
        
        if (base64_data) {
            // calculate required buffer size
            size_t buffer_size = base64_length + 256 + (label_end - label_start) * 2;
            // print_buffer_size(buffer_size);
            char* buffer = malloc(buffer_size);
            if (!buffer) {
                free(base64_data);
                return NULL;
            }

            // write the HTML string
            snprintf(buffer, buffer_size, 
                "<div class=\"visual-content-block\">"
                "<img src=\"data:image/jpeg;base64,%s\" alt=\"%.*s\">"
                "<p>%.*s</p></div>",
                base64_data, 
                (int)(label_end - label_start), label_start,
                (int)(label_end - label_start), label_start);

            free(base64_data);
            return buffer;
        } else {
            // fallback for failed conversion
            size_t buffer_size = url_length + 256 + (label_end - label_start) * 2;
            char* buffer = malloc(buffer_size);
            if (!buffer) return NULL;

            snprintf(buffer, buffer_size, 
                "<div class=\"visual-content-block\">"
                "<img src=\"%.*s\" alt=\"%.*s\">"
                "<p>%.*s</p></div>",
                (int)url_length, url_start,
                (int)(label_end - label_start), label_start,
                (int)(label_end - label_start), label_start);

            return buffer;
        }
    } else {
        // remote URL
        size_t buffer_size = url_length + 256 + (label_end - label_start) * 2;
        char* buffer = malloc(buffer_size);
        if (!buffer) return NULL;

        snprintf(buffer, buffer_size, 
            "<div class=\"visual-content-block\">"
            "<img src=\"%.*s\" alt=\"%.*s\">"
            "<p>%.*s</p></div>",
            (int)url_length, url_start,
            (int)(label_end - label_start), label_start,
            (int)(label_end - label_start), label_start);

        return buffer;
    }
}


char* process_inline_formatting(const char* source) {
    static char buffer[MAX_LINE_LENGTH];
    char* dest = buffer;
    const char* src = source;

    int bold_open = 0;
    int italic_open = 0;
    int code_open = 0;

    while (*src) {

        // handle inline code blocks with single backticks
        if (*src == '`' && !code_open) {
            strcpy(dest, "<code>");
            dest += 6;
            code_open = 1;
            src++;
            continue;
        } else if (*src == '`' && code_open) {
            strcpy(dest, "</code>");
            dest += 7;
            code_open = 0;
            src++;
            continue;
        }
        // skip formatting inside code blocks
        if (code_open) {
            // escape HTML special characters inside code blocks
            if (*src == '<') {
                strcpy(dest, "&lt;");
                dest += 4;
            } else if (*src == '>') {
                strcpy(dest, "&gt;");
                dest += 4;
            } else if (*src == '&') {
                strcpy(dest, "&amp;");
                dest += 5;
            } else {
                *dest++ = *src;
            }
            src++;
            continue;
        }

        if (*src == '[') {
            const char* link_text_start = src + 1;
            const char* link_text_end = strchr(link_text_start, ']');
            
            if (link_text_end && *(link_text_end + 1) == '(') {
                const char* url_start = link_text_end + 2;
                const char* url_end = strchr(url_start, ')');
                
                if (url_end) {
                    // copy link text
                    strncpy(dest, "<a href=\"", 8);
                    dest += 8;
                    
                    // copy URL
                    strncpy(dest, url_start, url_end - url_start);
                    dest += url_end - url_start;
                    
                    strcpy(dest, "\">");
                    dest += 2;
                    
                    // copy link text
                    strncpy(dest, link_text_start, link_text_end - link_text_start);
                    dest += link_text_end - link_text_start;
                    
                    strcpy(dest, "</a>");
                    dest += 4;
                    
                    // move source pointer to end of link
                    src = url_end + 1;
                    continue;
                }
            }
        }

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

void parse_markdown_line(FILE* input, const char* line, FILE* output) {
    static int in_list = 0;             // track if we're inside a list
    static int list_stack[10] = {0};    // stack to track list types at each level
    static int stack_depth = 0;         // current depth of nested lists
    static int current_list_type = -1;  // current main list type (-1 = none, 0 = ul, 1 = ol)
    static int last_level = 0;          // track last indentation level
    static int in_paragraph = 0;        // track if currently on simple paragraph

    int indent = 0;
    const char* trimmed = line;

    // image processing
    if (line[0] == '!' && line[1] == '[') {
        char* processed_image = process_image_block(line);
        if (processed_image) {
            fprintf(output, "%s\n", processed_image);
            return;
        }
    }

    // check for code block start
    if (is_code_block_delimeter(line)) {
        char* language = get_language_spec(line);
        process_code_block(input, output, language);
        return;
    }


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
        }
    }
}

int process_markdown_file(const char* input_filename, const char* output_filename) {
    // Open input file
    FILE* input = fopen(input_filename, "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    // Open output file
    FILE* output = fopen(output_filename, "w");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    // Write HTML header
    fprintf(output, "<!DOCTYPE html>\n<html>\n<head>\n<title>Markdown to HTML</title>\n</head>\n<body>\n");

    // Process the file line by line
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), input)) {
        parse_markdown_line(input, line, output);
    }

    // Write HTML footer
    fprintf(output, "</body>\n</html>\n");

    // Close files
    fclose(input);
    fclose(output);

    printf("Markdown converted to HTML successfully.\n");
    return 0;
}
