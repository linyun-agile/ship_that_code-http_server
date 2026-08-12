#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const char *mime_for(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";
    char ext[16] = {0}; size_t n = strlen(dot); if (n >= sizeof ext) n = sizeof ext - 1;
    for (size_t i = 0; i < n; i++) ext[i] = tolower((unsigned char)dot[i]);
    if (!strcmp(ext, ".html")) return "text/html; charset=utf-8";
    if (!strcmp(ext, ".css"))  return "text/css";
    if (!strcmp(ext, ".js"))   return "application/javascript";
    if (!strcmp(ext, ".json")) return "application/json";
    if (!strcmp(ext, ".png"))  return "image/png";
    if (!strcmp(ext, ".jpg"))  return "image/jpeg";
    if (!strcmp(ext, ".gif"))  return "image/gif";
    if (!strcmp(ext, ".svg"))  return "image/svg+xml";
    if (!strcmp(ext, ".txt"))  return "text/plain; charset=utf-8";
    return "application/octet-stream";
}

/* posix normalize: collapse "//", "/./" and "/x/../" segments */
static void normalize(const char *in, char *out) {
    char *segs[64]; int sn = 0;
    char buf[1024]; size_t bl = 0;
    int abs = (in[0] == '/');
    for (size_t i = 0; in[i]; ) {
        if (in[i] == '/') { i++; continue; }
        char *start = buf + bl;
        while (in[i] && in[i] != '/') buf[bl++] = in[i++];
        buf[bl++] = 0;
        if (!strcmp(start, ".")) continue;
        if (!strcmp(start, "..")) { if (sn > 0) sn--; continue; }
        segs[sn++] = start;
    }
    char *o = out; if (abs) *o++ = '/';
    for (int i = 0; i < sn; i++) {
        if (i > 0) *o++ = '/';
        size_t L = strlen(segs[i]); memcpy(o, segs[i], L); o += L;
    }
    *o = 0;
}

int main(void) {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char *bar = strchr(line, '|'); if (!bar) continue;
        *bar = 0; const char *root = line, *rel = bar + 1;
        char concat[2048]; snprintf(concat, sizeof concat, "%s%s", root, rel);
        char full[2048]; normalize(concat, full);
        /* TODO: ensure full is inside root: full == root OR strncmp(full, root, rL) == 0 && full[rL] == '/' */
        size_t rL = strlen(root);
        if (!(strcmp(full, root) == 0 || (strncmp(full, root, rL) == 0 && full[rL] == '/'))) { puts("403"); continue; }
        printf("%s|%s\n", full, mime_for(full));
    }
    return 0;
}
