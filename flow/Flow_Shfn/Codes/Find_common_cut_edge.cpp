#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
using namespace std;

const int MAXN = 105;
int capacity[MAXN][MAXN];
int original_capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];
bool visited[MAXN];
int n, m;

struct Edge
{
    int u, v, cap;
};
vector<Edge> allEdges;

void reset()
{
    for (int i = 0; i <= n; i++)
    {
        adj[i].clear();
    }
    memset(capacity, 0, sizeof(capacity));
    memset(original_capacity, 0, sizeof(original_capacity));
}

void addEdge(int u, int v, int cap)
{
    if (capacity[u][v] == 0)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    capacity[u][v] = cap;
    original_capacity[u][v] = cap;
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

void dfsReachable(int u)
{
    visited[u] = true;
    for (int v : adj[u])
    {
        if (!visited[v] && capacity[u][v] > 0)
        {
            dfsReachable(v);
        }
    }
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    cin >> n >> m;

    reset();

    for (int i = 0; i < m; i++)
    {
        int u, v, c;
        cin >> u >> v >> c;
        addEdge(u, v, c);
        allEdges.push_back({u, v, c});
    }

    // Find original max flow
    int originalFlow = edmondsKarp(1, n);

    // Find nodes reachable from source in residual graph
    memset(visited, false, sizeof(visited));
    dfsReachable(1);

    // Find edges in the minimum cut
    vector<Edge> cutEdges;
    for (const Edge &e : allEdges)
    {
        int u = e.u;
        int v = e.v;

        // Edge (u,v) is in cut if u is reachable and v is not
        if (visited[u] && !visited[v])
        {
            cutEdges.push_back({u, v, e.cap});
        }
    }

    // Try increasing each cut edge by 1
    bool found = false;
    int bestU = -1, bestV = -1;

    for (const Edge &e : cutEdges)
    {
        // Reset capacity to original
        for (int i = 0; i <= n; i++)
        {
            for (int j = 0; j <= n; j++)
            {
                capacity[i][j] = original_capacity[i][j];
            }
        }

        // Increase this edge by 1
        capacity[e.u][e.v]++;

        // Calculate new max flow
        int newFlow = edmondsKarp(1, n);

        if (newFlow > originalFlow)
        {
            found = true;
            bestU = e.u;
            bestV = e.v;
            break; // Found one, can stop
        }
    }

    if (found)
    {
        cout << bestU << " " << bestV << "\n";
    }
    else
    {
        cout << "doesn't exist\n";
    }

    return 0;
}