#include "libhelper.h"

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
        for (int i = 0; i < length; ++i) if (*(s+i) != *(m+i)) match = 0;
    }

    return match;
}

int print_wrap(int wrap, char *string, int position) {
    int remaining_string = strlen(string) - position;

    if (wrap > remaining_string) wrap = remaining_string;
    wrap = wrap_whitespace(wrap, string, position);

    for (int c = 0; c < wrap; ++c) printf("%c", *(string+position+c));
    printf("\n");

    return wrap;
}

void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) printf(" ");
}

void print_indent_wrap(char *string, int indent, int line_length) {
    int string_length = strlen(string);
    int wrap = line_length - indent;

    for (int p = 0; p < string_length; ++p) {
        print_indent(indent);
        p += print_wrap(wrap, string, p);
    }
}
