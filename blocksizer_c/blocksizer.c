#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libhelper.h"

#define MAXBITSIZE 1048576

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
 * All credit for the inspiration to write this utility rightly adheres to the
 * august personage of Stonish Ogham, who also pointed out a bug (since
 * squashed).  All blame and dire consequence for actually writing it resides
 * solely with the 2017 Copyright of Chad Perrin.  So mote it be.
 *
 * This work is released under the terms of the COIL.  See end of file for
 * explicit license terms.
 */

int is_numeric(char *string_argument);

unsigned long blocksize(unsigned long filesize, unsigned long max_blocksize);
unsigned long get_filesize(char *filesize_string);
unsigned long get_number_from_string(const char *numeric_string);
unsigned long get_size_from_file(const char *filename);

void print_help();
void print_help_message();
void print_input_error();
void print_size(char *filesize_string);
void print_size_error(unsigned long filesize);
void print_size_with_count(char *filesize_string);
void print_usage();

int main(int argc, char *argv[]) {
    if (argc == 1) print_usage();
    else if (argc == 2 && match_help(argv[1])) print_help();
    else if (argc == 2) print_size(argv[1]);
    else if (argc == 3 && match_count(argv[1])) print_size_with_count(argv[2]);
    else print_input_error();

    return 0;
}

int is_numeric(char *s) {
    int length = strlen(s);
    for (int i = 0; i < length; ++i) {
        if (*(s+i) < '0' || *(s+i) > '9') return 0;
    }

    return 1;
}

int wrap_whitespace(int length, char *string, int start) {
    int newline = 0;
    for (int c = 0; c < length && !newline; ++c) {
        if (*(string+start+c) == '\n') {
            length = c;
            newline = 1;
        }
    }
    
    int whitespace = 0;
    for (int c = length; c > 0 && !newline && !whitespace; --c) {
        if (*(string+start+c) == ' ') {
            length = c;
            whitespace = 1;
        }
    }
    
    return length;
}

void print_help() {
    print_usage();
    puts("");
    print_help_message();
}

void print_help_message() {
    puts("SYNOPSIS:\n");

    print_indent_wrap(
        "Enter a FILENAME to get the largest blocksize that evenly divides "
        "the byte size of the file whose name you supply.\n", 8, 80
    );

    puts("");

    print_indent_wrap(
        "Enter a FILESIZE (integer filesize in bytes, as given by 'ls -l') to "
        "get the largest blocksize that evenly divides the file size.  If the "
        "number matches an existing filename, you will get results as though "
        "you meant to enter a FILENAME instead.\n", 8, 80
    );

    puts("\nOPTIONS:\n");

    print_indent_wrap("-h, [--]help    Print this help information.\n", 8, 80);

    print_indent_wrap(
        "-c, [--]count   Print block count with calculated blocksize.\n", 8, 80
    );

    puts("\nEXAMPLES:\n");

    print_indent_wrap("$ blocksizer size_4587520_file.img\n", 8, 80);
    print_indent_wrap("131072\n\n", 8, 80);

    print_indent_wrap("$ blocksizer 4587520\n", 8, 80);
    print_indent_wrap("131072\n\n", 8, 80);

    print_indent_wrap("$ blocksizer count 4587520\n", 8, 80);
    print_indent_wrap("35 blocks of 131072 blocksize\n\n", 8, 80);
}

void print_input_error() {
    puts("You gave this program invalid input.\n");
    print_usage();
}

void print_size(char *filesize_string) {
    unsigned long filesize = get_filesize(filesize_string);

    if (filesize) {
        unsigned long size = blocksize(filesize, MAXBITSIZE);
        if (size < 512) print_size_error(filesize);
        else  printf("%lu\n", size);
    } else {
        print_input_error();
    }
}

void print_size_error(unsigned long filesize) {
    fprintf(stderr, "The only powers-of-two blocksizes for your filesize, ");
    fprintf(stderr, "%lu, ", filesize);
    fprintf(stderr, "are smaller than 512 bytes or larger than 2^20 bytes.\n");
}

void print_size_with_count(char *filesize_string) {
    unsigned long fsize = get_filesize(filesize_string);

    if (fsize) {
        unsigned long size = blocksize(fsize, MAXBITSIZE);
        if (size < 512) print_size_error(fsize);
        else printf("%lu blocks of %lu blocksize\n", (fsize / size), size);
    } else {
        print_input_error();
    }
}

void print_usage() {
    puts("USAGE:  blocksizer [[--]count] <FILENAME|FILESIZE>");
    puts("        blocksizer -c <FILENAME|FILESIZE>");
    puts("        blocksizer [--]help");
    puts("        blocksizer -h");
}

unsigned long blocksize(unsigned long filesize, unsigned long max_blocksize) {
    for (int i = 0; i < 20; ++i) {
        int product = max_blocksize >> i;
        if ((filesize % product) == 0) return product;
    }

    return 0;
}

unsigned long get_filesize(char *fsize_arg) {
    unsigned long fsize;

    if (access(fsize_arg, F_OK) != -1) fsize = get_size_from_file(fsize_arg);
    else if (is_numeric(fsize_arg)) fsize = get_number_from_string(fsize_arg);
    else fsize = 0;

    return fsize;
}

unsigned long get_number_from_string(const char *numeric_string) {
    char *endptr;
    unsigned long numeric_value = strtoul(numeric_string, &endptr, 0);

    if (errno == ERANGE || *endptr != '\0') {
        fprintf(stderr, "Number invalid or out of range: %s.\n", numeric_string);
        exit(EXIT_FAILURE);
    }

    return numeric_value;
}

unsigned long get_size_from_file(const char *filename) {
    struct stat st;
    unsigned long bytesize;

    if (stat(filename, &st) == 0) {
        bytesize = (unsigned long) st.st_size;
    } else {
        fprintf(stderr, "Filename invalid: %s.\n", filename);
        exit(EXIT_FAILURE);
    }

    return bytesize;
}

/*
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
