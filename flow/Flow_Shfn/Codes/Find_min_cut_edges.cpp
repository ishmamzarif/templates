#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAXN = 105;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];
bool visited[MAXN];

struct Edge
{
    int u, v, cap;
};

vector<Edge> allEdges;

bool bfs(int s, int t, int n)
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

void edmondsKarp(int s, int t, int n)
{
    while (bfs(s, t, n))
    {
        int pathFlow = INT_MAX;

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
    }
}

void dfsReachable(int u, int n)
{
    visited[u] = true;

    for (int v : adj[u])
    {
        if (!visited[v] && capacity[u][v] > 0)
        {
            dfsReachable(v, n);
        }
    }
}

int main()
{
    int n, m;
    cin >> n >> m;

    // Reset
    memset(capacity, 0, sizeof(capacity));
    for (int i = 0; i < MAXN; i++)
    {
        adj[i].clear();
    }

    for (int i = 0; i < m; i++)
    {
        int u, v, c;
        cin >> u >> v >> c;

        allEdges.push_back({u, v, c});

        if (capacity[u][v] == 0)
        {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        capacity[u][v] += c;
    }

    int s, t;
    cin >> s >> t;

    // Run max flow
    edmondsKarp(s, t, n);

    // Find reachable nodes from source in residual graph
    memset(visited, false, sizeof(visited));
    dfsReachable(s, n);

    // Min-cut edges: edges from reachable to non-reachable nodes
    cout << "Min-cut edges:" << endl;
    for (auto &edge : allEdges)
    {
        int u = edge.u;
        int v = edge.v;

        // Edge is in min-cut if u is reachable but v is not
        if (visited[u] && !visited[v])
        {
            cout << u << " -> " << v << " (capacity: " << edge.cap << ")" << endl;
        }
    }

    return 0;
}
