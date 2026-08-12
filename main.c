#include <stdio.h>
#include <string.h>
#include <ctype.h>

static char *trim(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char *e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1])) *--e = 0;
    return s;
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char *bar = strchr(line, '|'); if (!bar) { puts("200"); continue; }
        *bar = 0;
        char *etag = trim(line);
        char *header = trim(bar + 1);
        /* TODO: empty -> 200; "*" -> 304; comma list comparison with W/ normalization */
        if (header[0] == '\0') { puts("200"); continue; }
        if (strcmp(header, "*") == 0) { puts("304"); continue; }

        int matched = 0;
        char *part = header;
        while (part && *part) {
            char *next = strchr(part, ',');
            if (next) *next = 0;

            char *candidate = trim(part);
            if (strncmp(candidate, "W/", 2) == 0) candidate += 2;
            if (strcmp(candidate, etag) == 0) {
                matched = 1;
                break;
            }

            part = next ? next + 1 : NULL;
        }

        if (matched) { puts("304"); continue; }
        puts("200");
    }
    return 0;
}
