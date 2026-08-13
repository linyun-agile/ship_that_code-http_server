#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static void lower(char *s) { for (; *s; s++) *s = tolower((unsigned char)*s); }

int main(void) {
    char exact_pat[64][128]; char exact_site[64][64]; int en = 0;
    char wc_suffix[64][128]; char wc_site[64][64]; int wn = 0;
    char line[256]; int phase = 0;
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) {
            if (phase == 0) phase = 1;
            else puts("400");
            continue;
        }
        if (phase == 0) {
            char pat[128], site[64];
            if (sscanf(line, "%127s %63s", pat, site) != 2) continue;
            lower(pat);
            /* TODO: if pat starts with "*.", store pat+1 in wc_suffix; else store in exact */
            if(strncmp(pat, "*.", 2) == 0){
                int exists = 0;
                for(int i = 0; i < wn; i++){
                    if(strcmp(wc_suffix[i], pat+1) == 0) { exists = 1; break; }
                }
                if (exists) continue;
                strcpy(wc_suffix[wn], pat+1); strcpy(wc_site[wn], site); wn++;
                continue;
            }else{
                int exists = 0;
                for(int i = 0; i < en; i++){
                    if(strcmp(exact_pat[i], pat) == 0) { exists = 1; break; }
                }
                if (exists) continue;
                strcpy(exact_pat[en], pat); strcpy(exact_site[en], site); en++;
            }
        } else {
            char host[128]; strncpy(host, line, sizeof host - 1); host[sizeof host - 1] = 0;
            /* TODO: empty -> 400; lowercase + strip :port; exact then wildcard search */
            lower(host);
            char *port = strchr(host, ':');
            if (port) *port = 0;
            if (host[0] == 0) { puts("400"); continue; }
            int exact_match = 0;
            char* exact_site_match = 0;
            int wc_match = 0;
            char* wc_site_match = 0;
            for(int i = 0; i < en; i++){
                if(strcmp(host, exact_pat[i]) == 0) {exact_match = 1; exact_site_match = exact_site[i]; break;}
            }
            if(exact_match) {printf("%s\n", exact_site_match); continue;}
            size_t host_len = strlen(host);
            for(int i = 0; i < wn; i++){
                size_t suffix_len = strlen(wc_suffix[i]);
                if(host_len > suffix_len && strcmp(host + host_len - suffix_len, wc_suffix[i]) == 0) {
                    wc_match = 1; wc_site_match = wc_site[i]; break;
                }
            }
            if(wc_match) {printf("%s\n", wc_site_match); continue;}
            puts("404");
        }
    }
    return 0;
}
