
#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME 64
#define INF 1000000000

typedef struct Edge {
    int to;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    int id;
    char name[MAX_NAME];
    double x, y;
    Edge *head;
} Vertex;

typedef struct {
    int vertices;
    int capacity;
    Vertex *nodes;
    int **matrix;
} Graph;

typedef struct {
    int *distance;
    int *parent;
    int source;
} DijkstraResult;

typedef struct {
    int from, to, weight;
} MSTEdge;

typedef struct {
    MSTEdge *edges;
    int edge_count;
    int total_cost;
    bool connected;
} MSTResult;

Graph *graph_create(int capacity);
void graph_free(Graph *g);
int graph_add_vertex(Graph *g, const char *name, double x, double y);
bool graph_add_road(Graph *g, int from, int to, int weight, bool bidirectional);
bool graph_remove_road(Graph *g, int from, int to, bool bidirectional);
bool graph_update_weight(Graph *g, int from, int to, int weight, bool bidirectional);

DijkstraResult *dijkstra(const Graph *g, int source);
void dijkstra_free(DijkstraResult *r);
void dijkstra_print_path(const Graph *g, const DijkstraResult *r, int target);

MSTResult *kruskal(const Graph *g);
void mst_free(MSTResult *r);

int bfs_count(const Graph *g, int source, int *order);
int dfs_count(const Graph *g, int source, int *order);

bool graph_connected(const Graph *g);

#endif
