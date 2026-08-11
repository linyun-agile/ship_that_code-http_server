#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct { char method[16]; char path[256]; char handler[64]; } Route;
typedef struct { char path[256]; int methods; } Allow;
/* methods bitmask: GET=1 POST=2 PUT=4 DELETE=8 HEAD=16 OPTIONS=32 PATCH=64 */
static int method_bit(const char *m) {
    if (!strcmp(m,"GET")) return 1; if (!strcmp(m,"POST")) return 2;
    if (!strcmp(m,"PUT")) return 4; if (!strcmp(m,"DELETE")) return 8;
    if (!strcmp(m,"HEAD")) return 16; if (!strcmp(m,"OPTIONS")) return 32;
    if (!strcmp(m,"PATCH")) return 64; return 0;
}/* C path-params is verbose; skeleton with TODO at the segment-match step */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    Route routes[128]; int rn = 0;
    Allow allows[128]; int an = 0;
    char line[1024]; int phase = 0;
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) { phase = 1; continue; }
        if (phase == 0) {
            char m[16], p[256], h[64];
            if (sscanf(line, "%15s %255s %63s", m, p, h) != 3) continue;
            strcpy(routes[rn].method, m); strcpy(routes[rn].path, p); strcpy(routes[rn].handler, h); rn++;
            int found = -1;
            for (int i = 0; i < an; i++) if (!strcmp(allows[i].path, p)) { found = i; break; }
            if (found < 0) { strcpy(allows[an].path, p); allows[an].methods = method_bit(m); an++; }
            else allows[found].methods |= method_bit(m);
        } else {
            char m[16], p[512];
            if (sscanf(line, "%15s %511s", m, p) != 2) { puts("404"); continue; }
            /* TODO: strip "?..." from p */
            char *q = strchr(p, '?'); if (q) *q = 0;
            int hit = -1;
            for (int i = 0; i < rn; i++) if (!strcmp(routes[i].method, m) && !strcmp(routes[i].path, p)) { hit = i; break; }
            if (hit >= 0) printf("200 %s\n", routes[hit].handler);
            else {
                int path_known = 0;
                for (int i = 0; i < an; i++) if (!strcmp(allows[i].path, p)) { path_known = 1; break; }
                puts(path_known ? "405" : "404");
            }
        }
    }
    return 0;
}
