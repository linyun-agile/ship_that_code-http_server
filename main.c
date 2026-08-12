#include <stdio.h>
#include <string.h>

#define MAX_ROUTES 1024
#define MAX_SEGMENTS 64
#define MAX_BINDINGS 128
#define MAX_LINE 4096

typedef struct {
    int is_param;
    char text[256];
} Segment;

typedef struct {
    char method[64];
    char handler[64];
    Segment segments[MAX_SEGMENTS];
    int segment_count;
} Route;

typedef struct {
    char name[256];
    char value[1024];
} Binding;

static void strip_newline(char *line) {
    size_t len = strlen(line);
    while (len && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[--len] = '\0';
    }
}

static int split_path(const char *path, Segment segments[], int parse_params) {
    int count = 0;
    const char *start = path;

    if (*start == '/') {
        start++;
    }
    if (*start == '\0') {
        return 0;
    }

    while (count < MAX_SEGMENTS) {
        const char *slash = strchr(start, '/');
        size_t len = slash ? (size_t)(slash - start) : strlen(start);
        if (len >= sizeof(segments[count].text)) {
            len = sizeof(segments[count].text) - 1;
        }

        segments[count].is_param = 0;
        memcpy(segments[count].text, start, len);
        segments[count].text[len] = '\0';

        if (parse_params && segments[count].text[0] == '{') {
            size_t text_len = strlen(segments[count].text);
            if (text_len >= 2 && segments[count].text[text_len - 1] == '}') {
                segments[count].is_param = 1;
                memmove(segments[count].text, segments[count].text + 1, text_len - 2);
                segments[count].text[text_len - 2] = '\0';
            }
        }

        count++;
        if (!slash) {
            break;
        }
        start = slash + 1;
    }

    return count;
}

static void add_binding(Binding bindings[], int *count, const char *name, const char *value) {
    if (*count >= MAX_BINDINGS) {
        return;
    }
    snprintf(bindings[*count].name, sizeof(bindings[*count].name), "%s", name);
    snprintf(bindings[*count].value, sizeof(bindings[*count].value), "%s", value);
    (*count)++;
}

static void sort_bindings(Binding bindings[], int count) {
    for (int i = 1; i < count; i++) {
        Binding current = bindings[i];
        int j = i - 1;
        while (j >= 0 && strcmp(bindings[j].name, current.name) > 0) {
            bindings[j + 1] = bindings[j];
            j--;
        }
        bindings[j + 1] = current;
    }
}

static void parse_query(char *query, Binding bindings[], int *binding_count) {
    char *part = query;

    while (part && *part) {
        char *next = strchr(part, '&');
        if (next) {
            *next = '\0';
        }

        if (*part) {
            char *equals = strchr(part, '=');
            if (equals) {
                *equals = '\0';
                add_binding(bindings, binding_count, part, equals + 1);
            } else {
                add_binding(bindings, binding_count, part, "");
            }
        }

        part = next ? next + 1 : NULL;
    }
}

static int route_matches(const Route *route, const char *method, Segment request_segments[],
                         int request_segment_count, Binding bindings[], int *binding_count) {
    if (strcmp(route->method, method) != 0) {
        return 0;
    }
    if (route->segment_count != request_segment_count) {
        return 0;
    }

    for (int i = 0; i < route->segment_count; i++) {
        if (route->segments[i].is_param) {
            if (request_segments[i].text[0] == '\0') {
                return 0;
            }
            add_binding(bindings, binding_count, route->segments[i].text, request_segments[i].text);
        } else if (strcmp(route->segments[i].text, request_segments[i].text) != 0) {
            return 0;
        }
    }

    return 1;
}

static int add_route(Route routes[], int *route_count, const char *line) {
    char method[64];
    char path[2048];
    char handler[64];

    if (sscanf(line, "%63s %2047s %63s", method, path, handler) != 3) {
        return 0;
    }
    if (*route_count >= MAX_ROUTES) {
        return 0;
    }

    snprintf(routes[*route_count].method, sizeof(routes[*route_count].method), "%s", method);
    snprintf(routes[*route_count].handler, sizeof(routes[*route_count].handler), "%s", handler);
    routes[*route_count].segment_count = split_path(path, routes[*route_count].segments, 1);
    (*route_count)++;
    return 1;
}

static void handle_request(Route routes[], int route_count, char *line) {
    char method[64];
    char path[2048];

    if (sscanf(line, "%63s %2047s", method, path) != 2) {
        puts("404");
        return;
    }

    char *query = strchr(path, '?');
    if (query) {
        *query++ = '\0';
    }

    Segment request_segments[MAX_SEGMENTS];
    int request_segment_count = split_path(path, request_segments, 0);

    for (int i = 0; i < route_count; i++) {
        Binding bindings[MAX_BINDINGS];
        int binding_count = 0;

        if (!route_matches(&routes[i], method, request_segments, request_segment_count,
                           bindings, &binding_count)) {
            continue;
        }

        if (query) {
            parse_query(query, bindings, &binding_count);
        }
        sort_bindings(bindings, binding_count);

        printf("%s", routes[i].handler);
        for (int j = 0; j < binding_count; j++) {
            printf(" %s=%s", bindings[j].name, bindings[j].value);
        }
        putchar('\n');
        return;
    }

    puts("404");
}

int main(void) {
    static Route routes[MAX_ROUTES];
    int route_count = 0;
    int phase = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), stdin)) {
        strip_newline(line);

        if (line[0] == '\0') {
            phase = 1;
            continue;
        }

        if (phase == 0) {
            add_route(routes, &route_count, line);
        } else {
            handle_request(routes, route_count, line);
        }
    }

    return 0;
}
