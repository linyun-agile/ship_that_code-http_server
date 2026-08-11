#include <stdio.h>
#include <string.h>
#include <ctype.h>

static char *strip(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char *e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1])) *--e = 0;
    return s;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof line, stdin)) {
        char *nl = strpbrk(line, "\r\n"); if (nl) *nl = 0;
        if (line[0] == 0) break;
        char *colon = strchr(line, ':');
        if (!colon) { printf("ERR malformed: %s\n", line); continue; }
        *colon = 0;
        char *name = strip(line);
        char *value = strip(colon + 1);
        /* TODO: lowercase name in-place */
        for (int i = 0; name[i]; i++) name[i] = tolower((unsigned char)name[i]);
        

        printf("%s: %s\n", name, value);
    }
    return 0;
}
