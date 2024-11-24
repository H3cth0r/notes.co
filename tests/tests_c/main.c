#include <stdio.h>
#include <stdlib.h>
#include "../../notesco/c_src/markdown_to_html.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file.md> <output_file.html>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    FILE* output = fopen(argv[2], "w");
    if (!output) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    fprintf(output, "<!DOCTYPE html>\n<html>\n<head>\n<title>Markdown to HTML</title>\n</head>\n<body>\n");

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), input)) {
        parse_markdown_line(input, line, output);
    }

    fprintf(output, "</body>\n</html>\n");

    fclose(input);
    fclose(output);

    printf("Markdown converted to HTML successfully.\n");
    return 0;
}
