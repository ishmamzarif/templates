#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
using namespace std;

const int MAXN = 505;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];
bool visited[MAXN];
int n, m;

struct Edge
{
    int u, v;
};
vector<Edge> allEdges;

void addEdge(int u, int v)
{
    if (capacity[u][v] == 0)
    {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    capacity[u][v] = 1;
    capacity[v][u] = 1; // Undirected graph
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

    memset(capacity, 0, sizeof(capacity));

    for (int i = 0; i < m; i++)
    {
        int u, v;
        cin >> u >> v;
        addEdge(u, v);
        allEdges.push_back({u, v});
    }

    // Find max flow = min cut
    int minCut = edmondsKarp(1, n);

    // Find all nodes reachable from source in residual graph
    memset(visited, false, sizeof(visited));
    dfsReachable(1);

    // Find edges in the cut
    vector<pair<int, int>> cutEdges;
    for (const Edge &e : allEdges)
    {
        int u = e.u;
        int v = e.v;

        // Edge is in cut if one endpoint is reachable and other is not
        if ((visited[u] && !visited[v]) || (!visited[u] && visited[v]))
        {
            cutEdges.push_back({u, v});
        }
    }

    cout << minCut << "\n";
    for (const auto &edge : cutEdges)
    {
        cout << edge.first << " " << edge.second << "\n";
    }

    return 0;
}
