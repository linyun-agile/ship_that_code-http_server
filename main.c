#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(void) {
    char line[8192];
    char body[65536]; size_t blen = 0;
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line);
        while (L && (line[L-1]=='\n' || line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char *end; long size = strtol(line, &end, 16);
        if (end == line) continue;
        /* TODO: if size == 0, break; else read next line and copy first 'size' bytes into body */
        if (size == 0) break;
        if (!fgets(line, sizeof line, stdin)) break;
        L = strlen(line);
        while (L && (line[L-1]=='\n' || line[L-1]=='\r')) line[--L]=0;
        size_t n = (size_t)size < L ? (size_t)size : L;
        memcpy(body + blen, line, n); blen += n;
    }
    body[blen] = 0;
    printf("%s\n", body);
    return 0;
}
