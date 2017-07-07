#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * blocksize - a tool that calculates optimal blocksizes for dd
 *
 *      blocksize help
 *
 *      blocksize [-c] <FILESIZE>
 *
 *
 * This "blocksize" tool takes an integer argument and calculates the largest
 * power-of-two value of 1MB or smaller that yields no remainder when you
 * divide a byte count filesize (e.g. by checking the size using "ls -l") by
 * that blocksize number.
 *
 * Copyright 2017 Chad Perrin
 * This work is released under the terms of the COIL.
 *
 * # Copyfree Open Innovation License
 *
 * This is version 0.5 of the Copyfree Open Innovation License.
 *
 * ## Terms and Conditions
 *
 * Redistributions, modified or unmodified, in whole or in part, must retain
 * applicable copyright or other legal privilege notices, these conditions, and
 * the following license terms and disclaimer.  Subject to these conditions,
 * the holder(s) of copyright or other legal privileges, author(s) or
 * assembler(s), and contributors of this work hereby grant to any person who
 * obtains a copy of this work in any form:
 *
 * 1. Permission to reproduce, modify, distribute, publish, sell, sublicense,
 * use, and/or otherwise deal in the licensed material without restriction.
 *
 * 2. A perpetual, worldwide, non-exclusive, royalty-free, irrevocable patent
 * license to reproduce, modify, distribute, publish, sell, use, and/or
 * otherwise deal in the licensed material without restriction, for any and all
 * patents:
 *
 *    a. Held by each such holder of copyright or other legal privilege, author
 *    or assembler, or contributor, necessarily infringed by the contributions
 *    alone or by combination with the work, of that privilege holder, author
 *    or assembler, or contributor.
 *
 *    b. Necessarily infringed by the work at the time that holder of copyright
 *    or other privilege, author or assembler, or contributor made any
 *    contribution to the work.
 *
 * NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS
 * LICENSE OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE, AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS,
 * ASSEMBLERS, OR HOLDERS OF COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR
 * ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT,
 * OR OTHERWISE ARISING FROM, OUT OF, OR IN CONNECTION WITH THE WORK OR THE USE
 * OF OR OTHER DEALINGS IN THE WORK.
 */

int is_numeric(char *string_argument);
int last_newline(int length, char *string, int start);
int match_count(char *string_argument);
int match_help(char *string_argument);
int match_string(char *s, char *m);
int power(int base, unsigned int exponent);

unsigned long blocksize(unsigned long filesize);
unsigned long get_filesize(char *filesize_string);

void print_help();
void print_help_message();
void print_indent(int indent);
void print_indent_wrap(char *string, int indent, int line_length);
void print_input_error();
void print_size(char *filesize_string);
void print_size_error(char *filesize_string);
void print_size_with_count(char *filesize_string);
void print_usage();

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_usage();
    } else if (argc == 2 && match_help(argv[1])) {
        print_help();
    } else if (argc == 2 && is_numeric(argv[1])) {
        print_size(argv[1]);
    } else if (argc == 3 && match_count(argv[1]) && is_numeric(argv[2])) {
        print_size_with_count(argv[2]);
    } else {
        print_input_error();
    }

    return 0;
}

int is_numeric(char *s) {
    for (int i = 0; i < strlen(s); ++i) {
        if (*(s+i) < '0' || *(s+i) > '9') return 0;
    }

    return 1;
}

int last_newline(int length, char *string, int start) {
    int newline = 0;
    int position = length;

    for (int c = length; c > 0 && !newline; --c) {
        if (string[start+c] == '\n') {
            length = c;
            newline = 1;
        }
    }

    return length;
}

int match_count(char* s) {
    if (match_string(s, "-c")) return 1;
    else if (match_string(s, "--count")) return 1;
    else if (match_string(s, "count")) return 1;
    else return 0;
}

int match_help(char* s) {
    if (match_string(s, "-h")) return 1;
    else if (match_string(s, "--help")) return 1;
    else if (match_string(s, "help")) return 1;
    else return 0;
}

int match_string(char *s, char *m) {
    int length = strlen(s);
    int match = 1;

    if (length != strlen(m)) {
        match = 0;
    } else {
        for (int i = 0; i < length; ++i) {
            if (*(s+i) != *(m+i)) match = 0;
        }
    }

    return match;
}

int power(int base, unsigned int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) result *= base;
    return result;
}

void print_help() {
    print_usage();
    printf("\n");
    print_help_message();
}

void print_help_message() {
    printf("SYNOPSIS:\n\n");

    print_indent_wrap(
        "Enter a FILESIZE (integer filesize in bytes, as given by 'ls -l') to "
        "get the largest blocksize that evenly divides the file blocksize.\n",
        8, 80
    );

    printf("\nOPTIONS:\n\n");

    print_indent_wrap("-h, [--]help    Print this help information.\n", 8, 80);

    print_indent_wrap(
        "-c, [--]count   Print block count with calculated blocksize.\n", 8, 80
    );

    printf("\nEXAMPLES:\n\n");

    print_indent_wrap("$ blocksizer 4587520\n", 8, 80);
    print_indent_wrap("131072\n\n", 8, 80);

    print_indent_wrap("$ blocksizer count 4587520\n", 8, 80);
    print_indent_wrap("35 blocks of 131072 blocksize\n\n", 8, 80);
}

void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) printf(" ");
}

void print_indent_wrap(char *string, int indent, int line_length) {
    int string_length = strlen(string);
    int wrap = line_length - indent;

    for (int p = 0; p < string_length; ++p) {
        int length = string_length - p;

        if (length > 0) {
            print_indent(indent);

            if (length > wrap) {
                length = wrap;
            } else {
                length = last_newline(length, string, p);
            }

            for (int c = 0; c < length; ++c) printf("%c", string[p+c]);
            printf("\n");

            p += length;
        }
    }
}

void print_input_error() {
    printf("You gave this program invalid input.\n\n");
    print_usage();
}

void print_size(char *filesize_string) {
    unsigned long size = blocksize(get_filesize(filesize_string));
    if (size) printf("%lu\n", size);
    else print_size_error(filesize_string);
}

void print_size_error(char *filesize_string) {
    printf("The only powers-of-two blocksizes for your filesize, ");
    printf("%s, ", filesize_string);
    printf("are smaller than 2 bytes or larger than 2^20 bytes.\n");
}

void print_size_with_count(char *filesize_string) {
    unsigned long filesize = get_filesize(filesize_string);
    unsigned long size = blocksize(filesize);
    if (size) printf("%lu blocks of %lu blocksize\n", (filesize / size), size);
    else print_size_error(filesize_string);
}

void print_usage() {
    printf("USAGE:  blocksizer [[--]count] <FILESIZE>\n");
    printf("        blocksizer -c <FILESIZE>\n");
    printf("        blocksizer [--]help\n");
    printf("        blocksizer -h\n");
}

unsigned long blocksize(unsigned long filesize) {
    for (int i = 20; i > 0; --i) {
        if ((filesize % power(2, i)) == 0) {
            return power(2, i);
        }
    }

    return 0;
}

unsigned long get_filesize(char *filesize_string) {
    char *endptr;
    unsigned long filesize = strtoul(filesize_string, &endptr, 0);

    if (errno == ERANGE || *endptr != '\0') {
        fprintf(stderr, "Not a valid number or out of range.");
        exit(EXIT_FAILURE);
    }

    return filesize;
}