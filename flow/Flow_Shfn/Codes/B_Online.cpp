
// To find the maximum number of routes where no two routes share an edge

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring>
using namespace std;

const int MAXN = 105;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];
int n, m;
bool visited[MAXN];
struct Edge
{
    int u, v, cap;
};

vector<Edge> allEdges;
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

        maxFlow += pathFlow;
    }
    return maxFlow;
}
// can be reused;
bool dfs(int u, int t, vector<int> &path, vector<bool> &visited) // important for printing path.
{
    path.push_back(u);
    visited[u] = true;

    if (u == t)
        return true;

    for (int v : adj[u])
    {
        if (!visited[v] && capacity[u][v] > 0)
        {
            if (dfs(v, t, path, visited))
            {
                capacity[u][v]--;
                capacity[v][u]++;
                return true;
            }
        }
    }

    path.pop_back();
    return false;
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

    cin >> n >> m;

    // Reset
    memset(capacity, 0, sizeof(capacity));

    for (int i = 0; i < m; i++)
    {
        int u, v, c;
        cin >> u >> v >> c;

        if (capacity[u][v] == 0)
        {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        capacity[u][v] = c;
        ; // shobar capacity 1 dicchi;
    }

    int maxFlow = edmondsKarp(1, n);

    int p;
    cin >> p;

    memset(visited, false, sizeof(visited));
    dfsReachable(1, n);
    int ans = maxFlow;
    // Edge is in min-cut if u is reachable but v is not
    //  if (visited[u] && !visited[v])
    vector<int> index;
    for (int i = 0; i < p; i++)
    {
        int a, b, c;
        cin >> a >> b >> c;
        // Find reachable nodes from source in residual graph

        if ((visited[a] && !visited[b]) || (!visited[a] && visited[b]))
        {

            index.push_back(i + 1);
        }
    }
    if (index.size() == 0)
        cout << "None";
    else
        for (int x : index)
        {
            cout << x << " ";
        }

    return 0;
}
