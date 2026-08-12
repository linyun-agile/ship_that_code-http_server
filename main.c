#include <stdio.h>
#include <string.h>

typedef struct { char m[16], p[256], h[64]; } Route;
typedef struct { char p[256]; int gets[16]; int n; char methods[16][16]; } Allow;

int main(void) {
    Route routes[128]; int rn = 0;
    char allow_path[128][256]; char allow_methods[128][16][16]; int allow_mn[128] = {0}; int an = 0;
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        if (strncmp(line, "ROUTE ", 6) == 0) {
            char m[16], p[256], h[64]; sscanf(line + 6, "%15s %255s %63s", m, p, h);
            strcpy(routes[rn].m, m); strcpy(routes[rn].p, p); strcpy(routes[rn].h, h); rn++;
            int idx = -1;
            for (int i = 0; i < an; i++) if (!strcmp(allow_path[i], p)) { idx = i; break; }
            if (idx < 0) { strcpy(allow_path[an], p); strcpy(allow_methods[an][0], m); allow_mn[an] = 1; an++; }
            else { strcpy(allow_methods[idx][allow_mn[idx]++], m); }
        } else if (strncmp(line, "REQUEST ", 8) == 0) {
            char m[16], p[256]; char *body = "";
            char *pos = line + 8;
            sscanf(pos, "%15s %255s", m, p);
            char *sp1 = strchr(pos, ' '); char *sp2 = sp1 ? strchr(sp1 + 1, ' ') : NULL;
            if (sp2) body = sp2 + 1;
            char *q = strchr(p, '?'); if (q) *q = 0;
            int body_len = (int)strlen(body);
            /* TODO: search routes for exact (m, p) match, emit "<h> <body_len>" and LOG,
                     else allow_path -> 405, else 404. */
            int hit = -1;
            for (int i = 0; i < rn; i++) {
                if (!strcmp(routes[i].m, m) && !strcmp(routes[i].p, p)) {
                    hit = i;
                    break;
                }
            }
            if (hit >= 0) {
                printf("%s %d\n", routes[hit].h, body_len);
                printf("LOG %s %s 200 %d\n", m, p, body_len);
                continue;
            }

            int path_known = 0;
            for(int i = 0; i < an; i++){
                if(!strcmp(allow_path[i], p)) {
                    path_known = 1;
                    break;
                }
            }
            if (path_known) {
                printf("405\n");
                printf("LOG %s %s 405 0\n", m, p);
                continue;
            }

            printf("404\nLOG %s %s 404 0\n", m, p);
        }
    }
    return 0;
}
