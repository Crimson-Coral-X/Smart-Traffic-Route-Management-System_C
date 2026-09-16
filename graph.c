
#include "graph.h"

static bool valid(const Graph *g, int v) {
    return g && v >= 0 && v < g->vertices;
}

static Edge *new_edge(int to, int weight) {
    Edge *e = malloc(sizeof(*e));
    if (!e) return NULL;
    e->to = to;
    e->weight = weight;
    e->next = NULL;
    return e;
}

static Edge *find_edge(const Graph *g, int from, int to) {
    for (Edge *e = g->nodes[from].head; e; e = e->next)
        if (e->to == to) return e;
    return NULL;
}

static bool set_edge(Graph *g, int from, int to, int weight) {
    Edge *e = find_edge(g, from, to);
    if (e) {
        e->weight = weight;
        return true;
    }
    e = new_edge(to, weight);
    if (!e) return false;
    e->next = g->nodes[from].head;
    g->nodes[from].head = e;
    return true;
}

static bool delete_edge(Graph *g, int from, int to) {
    Edge *cur = g->nodes[from].head, *prev = NULL;
    while (cur) {
        if (cur->to == to) {
            if (prev) prev->next = cur->next;
            else g->nodes[from].head = cur->next;
            free(cur);
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

static bool resize(Graph *g) {
    int old = g->capacity, cap = old * 2;
    Vertex *nodes = realloc(g->nodes, (size_t)cap * sizeof(Vertex));
    if (!nodes) return false;

    int **m = malloc((size_t)cap * sizeof(int *));
    if (!m) { g->nodes = nodes; return false; }

    for (int i = 0; i < cap; ++i) {
        m[i] = calloc((size_t)cap, sizeof(int));
        if (!m[i]) {
            for (int j = 0; j < i; ++j) free(m[j]);
            free(m);
            g->nodes = nodes;
            return false;
        }
        for (int j = 0; j < i && j < old; ++j) m[i][j] = g->matrix[i][j];
    }
    for (int i = 0; i < old; ++i)
        for (int j = 0; j < old; ++j)
            m[i][j] = g->matrix[i][j];

    for (int i = old; i < cap; ++i) {
        nodes[i].id = i; nodes[i].name[0] = '\0';
        nodes[i].x = nodes[i].y = 0; nodes[i].head = NULL;
    }

    for (int i = 0; i < old; ++i) free(g->matrix[i]);
    free(g->matrix);
    g->matrix = m;
    g->nodes = nodes;
    g->capacity = cap;
    return true;
}

Graph *graph_create(int capacity) {
    if (capacity < 4) capacity = 8;
    Graph *g = calloc(1, sizeof(*g));
    if (!g) return NULL;

    g->capacity = capacity;
    g->nodes = calloc((size_t)capacity, sizeof(Vertex));
    g->matrix = malloc((size_t)capacity * sizeof(int *));
    if (!g->nodes || !g->matrix) { graph_free(g); return NULL; }

    for (int i = 0; i < capacity; ++i) {
        g->nodes[i].id = i;
        g->matrix[i] = calloc((size_t)capacity, sizeof(int));
        if (!g->matrix[i]) { graph_free(g); return NULL; }
    }
    return g;
}

void graph_free(Graph *g) {
    if (!g) return;
    if (g->nodes) {
        for (int i = 0; i < g->vertices; ++i) {
            Edge *e = g->nodes[i].head;
            while (e) { Edge *n = e->next; free(e); e = n; }
        }
    }
    if (g->matrix) {
        for (int i = 0; i < g->capacity; ++i) free(g->matrix[i]);
        free(g->matrix);
    }
    free(g->nodes);
    free(g);
}

int graph_add_vertex(Graph *g, const char *name, double x, double y) {
    if (!g || !name) return -1;
    if (g->vertices == g->capacity && !resize(g)) return -1;
    int id = g->vertices++;
    g->nodes[id].id = id;
    snprintf(g->nodes[id].name, MAX_NAME, "%s", name);
    g->nodes[id].x = x;
    g->nodes[id].y = y;
    g->nodes[id].head = NULL;
    return id;
}

bool graph_add_road(Graph *g, int from, int to, int weight, bool bidirectional) {
    if (!valid(g, from) || !valid(g, to) || from == to || weight <= 0) return false;
    if (!set_edge(g, from, to, weight)) return false;
    g->matrix[from][to] = weight;
    if (bidirectional) {
        if (!set_edge(g, to, from, weight)) return false;
        g->matrix[to][from] = weight;
    }
    return true;
}

bool graph_remove_road(Graph *g, int from, int to, bool bidirectional) {
    if (!valid(g, from) || !valid(g, to)) return false;
    bool ok = delete_edge(g, from, to);
    g->matrix[from][to] = 0;
    if (bidirectional) {
        ok = delete_edge(g, to, from) || ok;
        g->matrix[to][from] = 0;
    }
    return ok;
}

bool graph_update_weight(Graph *g, int from, int to, int weight, bool bidirectional) {
    if (!valid(g, from) || !valid(g, to) || weight <= 0 || !find_edge(g, from, to))
        return false;
    if (!set_edge(g, from, to, weight)) return false;
    g->matrix[from][to] = weight;
    if (bidirectional) {
        if (!find_edge(g, to, from)) return false;
        set_edge(g, to, from, weight);
        g->matrix[to][from] = weight;
    }
    return true;
}
