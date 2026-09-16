
# Smart Traffic Route Management System

A complete college-project workspace combining a **real interactive web application** with a **pure C graph/algorithm engine**.

## What you get

- Professional dark cyberpunk dashboard
- Interactive city graph visualization
- Dijkstra fastest-route calculation
- Dynamic traffic-weight controls
- BFS emergency traversal
- DFS connectivity validation
- Kruskal MST infrastructure planning
- Dynamic adjacency list and adjacency matrix in C
- Dynamic memory allocation with pointers and structs
- C engine can be compiled/tested independently
- No external JavaScript framework is required

## Folder structure

```text
Smart_Traffic_Route_Management_System/
├── backend/
│   ├── graph.h
│   ├── graph.c
│   ├── algorithms.c
│   ├── main.c
│   └── Makefile
├── frontend/
│   ├── index.html
│   ├── style.css
│   └── app.js
├── data/
│   └── city.json
└── README.md
```

## Run the application

The frontend is a self-contained browser application, so no Flask, Node.js or package installation is required.

### Easiest method

Open the `frontend` folder and double-click:

```text
index.html
```

It will open the complete application in your browser.

### Better VS Code method

Install the **Live Server** extension in VS Code.

1. Open the project folder.
2. Open `frontend/index.html`.
3. Right-click the file.
4. Select **Open with Live Server**.
5. The application opens in your browser.

## Test the C engine

Open a VS Code terminal:

```powershell
cd backend
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 main.c graph.c algorithms.c -o traffic_engine.exe
```

Test Dijkstra:

```powershell
.\traffic_engine.exe route 0 5
```

Test BFS:

```powershell
.\traffic_engine.exe bfs 0
```

Test DFS:

```powershell
.\traffic_engine.exe dfs 0
```

Test Kruskal:

```powershell
.\traffic_engine.exe mst
```

Or use the Makefile if `mingw32-make` is installed:

```powershell
mingw32-make
.\traffic_engine.exe mst
```

## Important architecture note

The browser UI contains the same algorithms in JavaScript so the application can operate immediately without requiring a native process server.

The `backend/` directory contains the **academic C implementation** of the required DSA. This is the code to show during a C/DSA evaluation:

- `graph.h` — graph structs and API
- `graph.c` — dynamic adjacency list + matrix and graph mutation
- `algorithms.c` — Dijkstra, Kruskal/DSU, BFS, DFS
- `main.c` — executable test interface

For a full production deployment, the C engine can be exposed to the browser through an HTTP/WebSocket service layer. The current workspace intentionally keeps the demo dependency-free and easy to run.

## Algorithms and complexity

### Dijkstra

Uses non-negative traffic weights.

This educational implementation is:

`O(V² + E)`

### Kruskal

Sorts edges and uses Disjoint Set Union / Union-Find.

`O(E log E)`

### BFS

Queue-based graph traversal.

`O(V + E)`

### DFS

Recursive depth-first traversal.

`O(V + E)`

## Why both adjacency list and matrix?

The adjacency list is efficient when iterating through a sparse road network. The matrix gives constant-time direct access to a road weight.

Every graph mutation updates both representations.

## How to demonstrate it

1. Open Dashboard.
2. Show the city graph.
3. Go to Route Optimizer.
4. Select two intersections.
5. Run Dijkstra.
6. Show the highlighted green route.
7. Open Traffic Control.
8. Increase a road's weight.
9. Run Dijkstra again and demonstrate the route can change.
10. Open Emergency Mode and demonstrate BFS/DFS.
11. Open Infrastructure and run Kruskal.
12. Explain the C source files and data structures.

## Viva explanation

**Graph:** A city is modeled as a weighted graph. Intersections are vertices and roads are weighted edges.

**Traffic weight:** A larger weight means a slower/more expensive road. It can represent estimated travel time or congestion-adjusted cost.

**Dijkstra:** Finds the minimum-cost route between intersections.

**MST:** Finds a minimum-cost network connecting all intersections, suitable conceptually for sensors, cameras or communication cables.

**BFS:** Traverses the emergency network level-by-level.

**DFS:** Performs deep connectivity validation.

**Dynamic memory:** The C graph allocates nodes, matrix rows and linked-list edges dynamically and releases them with `free()`.

## Future upgrades

The architecture can later be extended with:

- Real map tiles / Leaflet
- GPS coordinates
- Live traffic APIs
- One-way roads
- Road closures
- Ambulance priority routing
- Traffic-signal timing
- Database persistence
- Native C HTTP service
- A* search
- Priority queue / binary heap Dijkstra
- Authentication and multi-user operation
