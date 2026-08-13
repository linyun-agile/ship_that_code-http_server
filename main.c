#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char *trim(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char *e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1])) *--e = 0;
    return s;
}

int main(void) {
    char line[256];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char *bar = strchr(line, '|'); if (!bar) continue;
        *bar = 0;
        long size = atol(line);
        char *spec = trim(bar + 1);
        char *dash = strchr(spec, '-');
        if (!dash) { printf("416 */%ld\n", size); continue; }
        *dash = 0;
        char *a = trim(spec); char *b = trim(dash + 1);
        
        /* TODO: implement range logic */
        long left = -1, right = -1;
        if (a[0] != '\0') left = atol(a);
        if (b[0] != '\0') right = atol(b);

        if (left == -1 && right == -1) {
            printf("416 */%ld\n", size);
            continue;
        }

        if (left == -1) {
            long suffix_len = right;
            if (suffix_len <= 0) {
                printf("416 */%ld\n", size);
                continue;
            }
            if (suffix_len > size) suffix_len = size;
            left = size - suffix_len;
            right = size - 1;
        } else {
            if (left >= size) {
                printf("416 */%ld\n", size);
                continue;
            }
            if (right == -1 || right >= size) right = size - 1;
            if (right < left) {
                printf("416 */%ld\n", size);
                continue;
            }
        }

        printf("206 %ld-%ld/%ld %ld\n", left, right, size, right - left + 1);
    }
    return 0;
}
