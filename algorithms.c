
#include "graph.h"

/* ---------------- Dijkstra ----------------
   Finds minimum non-negative traffic cost from source to every node.
   This simple implementation uses O(V^2 + E), which is excellent for
   a small educational city graph and keeps the algorithm transparent. */
DijkstraResult *dijkstra(const Graph *g, int source) {
    if (!g || source < 0 || source >= g->vertices) return NULL;
    int n = g->vertices;

    DijkstraResult *r = calloc(1, sizeof(*r));
    if (!r) return NULL;
    r->distance = malloc((size_t)n * sizeof(int));
    r->parent = malloc((size_t)n * sizeof(int));
    bool *used = calloc((size_t)n, sizeof(bool));

    if (!r->distance || !r->parent || !used) {
        free(r->distance); free(r->parent); free(used); free(r);
        return NULL;
    }

    r->source = source;
    for (int i = 0; i < n; ++i) {
        r->distance[i] = INF;
        r->parent[i] = -1;
    }
    r->distance[source] = 0;

    for (int step = 0; step < n; ++step) {
        int u = -1;
        for (int i = 0; i < n; ++i)
            if (!used[i] && (u == -1 || r->distance[i] < r->distance[u]))
                u = i;

        if (u == -1 || r->distance[u] == INF) break;
        used[u] = true;

        for (Edge *e = g->nodes[u].head; e; e = e->next) {
            if (r->distance[u] + e->weight < r->distance[e->to]) {
                r->distance[e->to] = r->distance[u] + e->weight;
                r->parent[e->to] = u;
            }
        }
    }
    free(used);
    return r;
}

void dijkstra_free(DijkstraResult *r) {
    if (!r) return;
    free(r->distance); free(r->parent); free(r);
}

void dijkstra_print_path(const Graph *g, const DijkstraResult *r, int target) {
    if (!g || !r || target < 0 || target >= g->vertices) return;
    if (r->distance[target] == INF) {
        printf("UNREACHABLE\n");
        return;
    }
    int *path = malloc((size_t)g->vertices * sizeof(int));
    if (!path) return;
    int count = 0;
    for (int v = target; v != -1 && count < g->vertices; v = r->parent[v])
        path[count++] = v;

    printf("Cost: %d | Path: ", r->distance[target]);
    for (int i = count - 1; i >= 0; --i) {
        printf("%s", g->nodes[path[i]].name);
        if (i) printf(" -> ");
    }
    putchar('\n');
    free(path);
}

/* ---------------- BFS / DFS ---------------- */
int bfs_count(const Graph *g, int source, int *order) {
    if (!g || !order || source < 0 || source >= g->vertices) return 0;
    bool *seen = calloc((size_t)g->vertices, sizeof(bool));
    int *queue = malloc((size_t)g->vertices * sizeof(int));
    if (!seen || !queue) { free(seen); free(queue); return 0; }

    int front = 0, rear = 0, count = 0;
    queue[rear++] = source; seen[source] = true;

    while (front < rear) {
        int u = queue[front++];
        order[count++] = u;
        for (Edge *e = g->nodes[u].head; e; e = e->next) {
            if (!seen[e->to]) {
                seen[e->to] = true;
                queue[rear++] = e->to;
            }
        }
    }
    free(seen); free(queue);
    return count;
}

static void dfs_visit(const Graph *g, int u, bool *seen, int *order, int *count) {
    seen[u] = true;
    order[(*count)++] = u;
    for (Edge *e = g->nodes[u].head; e; e = e->next)
        if (!seen[e->to]) dfs_visit(g, e->to, seen, order, count);
}

int dfs_count(const Graph *g, int source, int *order) {
    if (!g || !order || source < 0 || source >= g->vertices) return 0;
    bool *seen = calloc((size_t)g->vertices, sizeof(bool));
    if (!seen) return 0;
    int count = 0;
    dfs_visit(g, source, seen, order, &count);
    free(seen);
    return count;
}

bool graph_connected(const Graph *g) {
    if (!g || g->vertices == 0) return false;
    int *order = malloc((size_t)g->vertices * sizeof(int));
    if (!order) return false;
    bool connected = bfs_count(g, 0, order) == g->vertices;
    free(order);
    return connected;
}

/* ---------------- Kruskal + DSU ---------------- */
typedef struct { int parent, rank; } DSU;

static int find_set(DSU *d, int x) {
    if (d[x].parent != x) d[x].parent = find_set(d, d[x].parent);
    return d[x].parent;
}

static void unite(DSU *d, int a, int b) {
    a = find_set(d, a); b = find_set(d, b);
    if (a == b) return;
    if (d[a].rank < d[b].rank) d[a].parent = b;
    else if (d[a].rank > d[b].rank) d[b].parent = a;
    else { d[b].parent = a; d[a].rank++; }
}

static int edge_cmp(const void *a, const void *b) {
    const MSTEdge *x = a, *y = b;
    return (x->weight > y->weight) - (x->weight < y->weight);
}

MSTResult *kruskal(const Graph *g) {
    if (!g || g->vertices == 0) return NULL;
    int n = g->vertices, maxe = n * (n - 1) / 2, count = 0;

    MSTEdge *all = malloc((size_t)maxe * sizeof(MSTEdge));
    MSTResult *r = calloc(1, sizeof(MSTResult));
    DSU *d = malloc((size_t)n * sizeof(DSU));
    if (!all || !r || !d) { free(all); free(r); free(d); return NULL; }

    for (int u = 0; u < n; ++u)
        for (Edge *e = g->nodes[u].head; e; e = e->next)
            if (u < e->to) all[count++] = (MSTEdge){u, e->to, e->weight};

    qsort(all, (size_t)count, sizeof(MSTEdge), edge_cmp);
    r->edges = malloc((size_t)(n - 1) * sizeof(MSTEdge));
    if (!r->edges) { free(all); free(r); free(d); return NULL; }

    for (int i = 0; i < n; ++i) d[i] = (DSU){i, 0};

    for (int i = 0; i < count && r->edge_count < n - 1; ++i) {
        if (find_set(d, all[i].from) != find_set(d, all[i].to)) {
            unite(d, all[i].from, all[i].to);
            r->edges[r->edge_count++] = all[i];
            r->total_cost += all[i].weight;
        }
    }
    r->connected = (r->edge_count == n - 1);
    free(all); free(d);
    return r;
}

void mst_free(MSTResult *r) {
    if (!r) return;
    free(r->edges); free(r);
}
