#ifndef MARKDOWN_TO_HTML_H
#define MARKDOWN_TO_HTML_H

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 1024
#define MAX_CODE_BUFFER 16384

void print_buffer_size(size_t buffer_size);

int is_code_block_delimeter(const char* line);

char* get_language_spec(const char* line);

char* preserve_indentation(const char* line);

void process_code_block(FILE* input, FILE* output, const char* language);

char* base64_encode(const uint8_t* data, size_t input_length, size_t* output_length);

char* file_to_base64(const char* filename, size_t* base64_length);

char* process_image_block(const char* source);

char* process_inline_formatting(const char* source);

void parse_markdown_line(FILE* input, const char* line, FILE* output);

int process_markdown_file(const char* input_filename, const char* output_filename);

#endif 
