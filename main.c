#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    int workers = 0;
    int busy_cid[256]; int busy_ms[256]; int bn = 0;
    int q_cid[256]; int q_ms[256]; int qh = 0, qt = 0;
    int done = 0;
    char line[256];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char cmd[16]; int a, b;
        if (sscanf(line, "%15s %d %d", cmd, &a, &b) >= 1) {
            if (!strcmp(cmd, "WORKERS")) workers = a;
            else if (!strcmp(cmd, "ARRIVE")) {
                if (bn < workers) { busy_cid[bn] = a; busy_ms[bn] = b; bn++; printf("STARTED %d\n", a); }
                else { q_cid[qt] = a; q_ms[qt] = b; qt++; }
            } else if (!strcmp(cmd, "TICK")) {
                /* TODO: for each busy worker, decrement ms; if <= 0 remove and print DONE, then dispatch from queue */
                int write = 0;
                for (int i = 0; i < bn; i++) {
                    busy_ms[i]--;
                    if (busy_ms[i] <= 0) {
                        done++;
                        printf("DONE %d\n", busy_cid[i]);
                    } else {
                        busy_cid[write] = busy_cid[i];
                        busy_ms[write] = busy_ms[i];
                        write++;
                    }
                }
                bn = write;
                while (bn < workers && qh < qt) {
                    busy_cid[bn] = q_cid[qh];
                    busy_ms[bn] = q_ms[qh];
                    printf("STARTED %d\n", busy_cid[bn]);
                    bn++;
                    qh++;
                }
            } else if (!strcmp(cmd, "STATUS")) {
                printf("free=%d busy=%d queued=%d done=%d\n", workers - bn, bn, qt - qh, done);
            }
        }
    }
    return 0;
}
