
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

int main()
{
    int T;
    cin >> T;

    for (int tc = 1; tc <= T; tc++)
    {
        cin >> n >> m;

        // Reset
        memset(capacity, 0, sizeof(capacity));
        for (int i = 0; i <= n; i++)
        {
            adj[i].clear();
        }

        for (int i = 0; i < m; i++)
        {
            int u, v;
            cin >> u >> v;

            if (capacity[u][v] == 0)
            {
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
            capacity[u][v]++; // shobar capacity 1 dicchi;
        }

        int maxFlow = edmondsKarp(1, n);

        cout << "Case " << tc << ": " << maxFlow << endl;

        if (maxFlow == 0)
        {
            cout << "No escape route possible! The Professor needs a new plan." << endl;
        }
        else
        {
            // Reconstruct paths
            for (int i = 0; i < maxFlow; i++)
            {
                vector<int> path;
                vector<bool> visited(n + 1, false);

                dfs(1, n, path, visited);

                for (int j = 0; j < path.size(); j++)
                {
                    if (j > 0)
                        cout << " -> ";
                    cout << path[j];
                }
                cout << endl;
            }
        }
    }

    return 0;
}
