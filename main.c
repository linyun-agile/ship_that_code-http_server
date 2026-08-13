#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
/* For real C: link openssl for SHA1 + EVP_EncodeBlock. */
#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

static uint32_t rol32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static void lower(char* s) {
    for (; *s; s++) *s = (char)tolower((unsigned char)*s);
}

static void sha1_block(uint32_t h[5], const unsigned char block[64]) {
    uint32_t w[80];
    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[i * 4] << 24) |
               ((uint32_t)block[i * 4 + 1] << 16) |
               ((uint32_t)block[i * 4 + 2] << 8) |
               (uint32_t)block[i * 4 + 3];
    }
    for (int i = 16; i < 80; i++) {
        w[i] = rol32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        uint32_t temp = rol32(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = rol32(b, 30);
        b = a;
        a = temp;
    }

    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
}

static void sha1(const unsigned char *msg, size_t len, unsigned char digest[20]) {
    uint32_t h[5] = {
        0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0
    };

    size_t off = 0;
    while (off + 64 <= len) {
        sha1_block(h, msg + off);
        off += 64;
    }

    unsigned char block[128] = {0};
    size_t rem = len - off;
    memcpy(block, msg + off, rem);
    block[rem] = 0x80;

    size_t final_off = (rem >= 56) ? 64 : 0;
    uint64_t bits = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++) {
        block[final_off + 63 - i] = (unsigned char)(bits >> (i * 8));
    }

    sha1_block(h, block);
    if (final_off) sha1_block(h, block + 64);

    for (int i = 0; i < 5; i++) {
        digest[i * 4] = (unsigned char)(h[i] >> 24);
        digest[i * 4 + 1] = (unsigned char)(h[i] >> 16);
        digest[i * 4 + 2] = (unsigned char)(h[i] >> 8);
        digest[i * 4 + 3] = (unsigned char)h[i];
    }
}

static void base64_encode(const unsigned char *in, size_t len, char *out) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t j = 0;
    for (size_t i = 0; i < len; i += 3) {
        unsigned int v = (unsigned int)in[i] << 16;
        int remain = (int)(len - i);
        if (remain > 1) v |= (unsigned int)in[i + 1] << 8;
        if (remain > 2) v |= in[i + 2];

        out[j++] = table[(v >> 18) & 63];
        out[j++] = table[(v >> 12) & 63];
        out[j++] = (remain > 1) ? table[(v >> 6) & 63] : '=';
        out[j++] = (remain > 2) ? table[v & 63] : '=';
    }
    out[j] = 0;
}

static void websocket_accept(const char *key, char *out) {
    char msg[1200];
    unsigned char digest[20];
    snprintf(msg, sizeof(msg), "%s%s", key, GUID);
    sha1((const unsigned char *)msg, strlen(msg), digest);
    base64_encode(digest, sizeof(digest), out);
}

static int contains_upgrade(char* s) {
    lower(s);
    for(char *p = s; *p; p++) {
        if(strncmp(p, "upgrade", 7) == 0) {
            return 1;
        }
    }
    return 0;
}


int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        size_t L = strlen(line); while (L && (line[L-1]=='\n'||line[L-1]=='\r')) line[--L]=0;
        if (L == 0) continue;
        char *f[5]; int n = 0; f[n++] = line;
        for (char *p = line; *p && n < 5; p++) if (*p == '|') { *p = 0; f[n++] = p + 1; }
        if (n != 5) { puts("400"); continue; }
        /* TODO: validate; compute base64(sha1(key + GUID)); emit "101 <accept>" */
        int valid = 1;
        if(valid && strcmp(f[0], "GET") != 0) {valid = 0;}
        lower(f[1]);
        if(valid && strcmp(f[1], "websocket") != 0) {valid = 0;}
        if(valid && !contains_upgrade(f[2])) {valid = 0;}
        if(valid && f[3][0] == 0) {valid = 0;}
        if(valid && strcmp(f[4], "13") != 0) {valid = 0;}
        if(valid) {
            char accept[32];
            websocket_accept(f[3], accept);
            printf("101 %s\n", accept);
            continue;
        }
        puts("400");
    }
    return 0;
}
