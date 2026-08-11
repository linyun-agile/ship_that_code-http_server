#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char *status_text(int s) {
    switch (s) {
        case 200: return "OK"; case 201: return "Created"; case 204: return "No Content";
        case 301: return "Moved Permanently"; case 302: return "Found"; case 304: return "Not Modified";
        case 400: return "Bad Request"; case 401: return "Unauthorized"; case 403: return "Forbidden";
        case 404: return "Not Found"; case 405: return "Method Not Allowed"; case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}

int main(void) {
    char buf[65536]; size_t n = fread(buf, 1, sizeof(buf)-1, stdin); buf[n] = 0;
    char *p = buf;
    int status, hc;
    if (sscanf(p, "%d %d", &status, &hc) != 2) return 0;
    char *nl = strchr(p, '\n'); if (!nl) return 0; p = nl + 1;
    char *headers[64]; int hi = 0;
    for (int i = 0; i < hc; i++) {
        nl = strchr(p, '\n');
        if (!nl) { headers[hi++] = p; p += strlen(p); break; }
        *nl = 0; 
        if(nl > p && nl[-1] == '\r') nl[-1] = 0; // remove \r
        headers[hi++] = p; p = nl + 1;
    }
    char *body = p; size_t bl = strlen(body);
    while (bl && (body[bl-1]=='\n' || body[bl-1]=='\r')) body[--bl] = 0;
    /* TODO: scan headers for case-insensitive "content-length:" prefix */
    int has_cl = 0;

    for(int i = 0; i < hi; i++){
        char *name = headers[i];
        char tmp_name[64];
        strcpy(tmp_name, name);
        for (int j = 0; tmp_name[j]; j++) tmp_name[j] = tolower((unsigned char)tmp_name[j]);
        if(strncmp(tmp_name, "content-length:", 14) == 0){
            has_cl = 1;
            break;
        }
    }
    printf("HTTP/1.1 %d %s\r\n", status, status_text(status));
    for (int i = 0; i < hi; i++) printf("%s\r\n", headers[i]);
    if (!has_cl) printf("Content-Length: %zu\r\n", bl);
    printf("\r\n%s", body);
    return 0;
}
