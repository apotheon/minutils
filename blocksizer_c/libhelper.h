#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

extern int match_count(char *string_argument);
extern int match_help(char *string_argument);
extern int match_string(char *s, char *m);
extern int print_wrap(int wrap, char *string, int position);
extern int wrap_whitespace(int length, char *string, int start);
extern void print_indent(int indent);
extern void print_indent_wrap(char *string, int indent, int line_length);
#endif
