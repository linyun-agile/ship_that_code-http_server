#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int is_method(const char *s) {
    static const char *m[] = {"GET","POST","PUT","DELETE","HEAD","OPTIONS","PATCH",NULL};
    for (int i = 0; m[i]; i++) if (!strcmp(s, m[i])) return 1;
    return 0;
}
/* TODO: implement is_version: return 1 iff s == "HTTP/<digit>.<digit>" */
static int is_version(const char *s) { return strlen(s) == 8 && strncmp(s, "HTTP/", 5) == 0 \
                     && isdigit(s[5]) && s[6] == '.' && isdigit(s[7]); }

int main(void) {
    char line[4096];
    while (fgets(line, sizeof line, stdin)) {
        char *nl = strpbrk(line, "\r\n"); if (nl) *nl = 0;
        if (line[0] == 0) continue;
        char m[64], p[2048], v[64];
        int n = sscanf(line, "%63s %2047s %63s", m, p, v);
        char* s1 = strchr(line, ' ');
        char *s2 = s1 ? strchr(s1+1, ' ') : NULL;
        if(!s1 || !s2 || s2 == s1 + 1 || strchr(s2+1, ' ') != NULL) { puts("INVALID"); continue; }
        /* TODO: must have exactly 3 tokens; method valid; p begins with '/'; version valid */
        if (n != 3 || !is_method(m) || p[0] != '/' || !is_version(v)) { puts("INVALID"); continue; }
        printf("METHOD=%s PATH=%s VERSION=%s\n", m, p, v);
    }
    return 0;
}
