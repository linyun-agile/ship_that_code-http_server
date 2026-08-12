#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void lower(char *s) { for (; *s; s++) *s = tolower((unsigned char)*s); }

int main(void) {
    char line[512];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r'||line[L-1]==' ')) line[--L]=0;
        if (L == 0) continue;
        char version[32] = {0}, conn[32] = {0};
        int fields = sscanf(line, "%31s %31s", version, conn);
        if (fields < 1) continue;
        lower(conn);
        /* TODO: HTTP/1.1 default KEEP, "close" forces CLOSE.
                 HTTP/1.0 default CLOSE, "keep-alive" forces KEEP. */
        if(strcmp(version, "HTTP/1.0") == 0){
            if(fields == 2 && strcmp(conn, "keep-alive") == 0) puts("KEEP");
            else puts("CLOSE");
        } else if (strcmp(version, "HTTP/1.1") == 0){
            if(fields == 2 && strcmp(conn, "close") == 0) puts("CLOSE");
            else puts("KEEP");
        }
        else continue;
    }
    return 0;
}
