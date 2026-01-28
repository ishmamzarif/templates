#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAXN = 105;
int capacity[MAXN][MAXN];
int original_capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];
int n, m;

void reset()
{
    for (int i = 0; i <= n; i++)
    {
        adj[i].clear();
    }
    memset(capacity, 0, sizeof(capacity));
    memset(original_capacity, 0, sizeof(original_capacity));
}

void addEdge(int u, int v)
{
    if (original_capacity[u][v] == 0)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    // For edge connectivity, each edge has capacity 1
    original_capacity[u][v] = 1;
    original_capacity[v][u] = 1; // Undirected graph
}

bool bfs(int s, int t)
{
    memset(parent, -1, sizeof(parent));
    queue<int> q;
    q.push(s);
    parent[s] = -2;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : adj[u])
        {
            if (parent[v] == -1 && capacity[u][v] > 0)
            {
                parent[v] = u;
                if (v == t)
                    return true;
                q.push(v);
            }
        }
    }
    return false;
}

int edmondsKarp(int s, int t)
{
    // Reset capacity from original
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            capacity[i][j] = original_capacity[i][j];
        }
    }

    int maxFlow = 0;

    while (bfs(s, t))
    {
        int pathFlow = 1e9;
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            pathFlow = min(pathFlow, capacity[u][v]);
        }

        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            capacity[u][v] -= pathFlow;
            capacity[v][u] += pathFlow;
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}

int findEdgeConnectivity()
{
    int minCut = 1e9;

    // Run max-flow from vertex 1 to all other vertices
    // The minimum among these flows is the edge connectivity
    for (int t = 2; t <= n; t++)
    {
        int flow = edmondsKarp(1, t);
        minCut = min(minCut, flow);

        // Early termination: if we find a cut of size 1, that's the minimum
        if (minCut == 1)
        {
            return 1;
        }
    }

    return minCut;
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    cin >> n >> m;

    reset();

    for (int i = 0; i < m; i++)
    {
        int u, v;
        cin >> u >> v;
        addEdge(u, v);
    }

    int edgeConnectivity = findEdgeConnectivity();

    cout << "Edge Connectivity: " << edgeConnectivity << "\n";

    return 0;
}

/*
EXPLANATION:
============

Edge Connectivity Definition:
- The minimum number of edges that must be removed to disconnect the graph

Key Insight:
- Edge connectivity = minimum cut over all pairs of vertices
- By max-flow min-cut theorem: min cut = max flow

Algorithm:
1. For each edge, assign capacity 1 (since we can remove it once)
2. Run max-flow from a fixed vertex (vertex 1) to all other vertices
3. The minimum flow value is the edge connectivity

Why this works:
- The global minimum cut will be found by testing from one vertex to all others
- We don't need to test all O(n²) pairs, just O(n) pairs from vertex 1
- Each max-flow gives us the minimum edges to separate that pair
- The minimum among all these is the global edge connectivity

Time Complexity:
- O(|V|) flow computations
- Each flow: O(|V| × |E|²) using Edmonds-Karp
- Total: O(|V|² × |E|²)

Examples:
1. Tree: edge connectivity = 1 (remove any edge disconnects it)
2. Cycle: edge connectivity = 2 (must remove 2 edges)
3. Complete graph K_n: edge connectivity = n-1

Optimization:
- If we find edge connectivity = 1, we can stop early
- For dense graphs, use better max-flow algorithms (Dinic, Push-Relabel)
*/