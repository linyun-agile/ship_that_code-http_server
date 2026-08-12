#include <stdio.h>
#include <string.h>

static const char *MONTHS[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

/* TODO: convert "2025-03-17T09:15:23" -> "17/Mar/2025:09:15:23 +0000" */
static void to_clf(const char *iso, char *out) { 
    int year, month, day, hour, minute, second;
    sscanf(iso, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    sprintf(out, "%02d/%s/%04d:%02d:%02d:%02d +0000",
            day, MONTHS[month - 1], year, hour, minute, second);
}

int main(void) {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char *f[9]; int n = 0; f[n++] = line;
        for (char *p = line; *p && n < 9; p++) if (*p == '|') { *p = 0; f[n++] = p + 1; }
        char ts[64]; to_clf(f[1], ts);
        printf("%s - - [%s] \"%s %s %s\" %s %s \"-\" \"%s\" %sms\n", f[0], ts, f[2], f[3], f[4], f[5], f[6], f[7], f[8]);
    }
    return 0;
}
