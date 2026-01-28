#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring>
#include <tuple>
using namespace std;

int n = 1000, m, k;
int src, snk;
class edge
{
public:
    int u, v, c;

    edge(int u, int v, int c)
    {
        this->u = u;
        this->v = v;
        this->c = c;
    }
};

vector<vector<int>> capacity;
vector<vector<int>> adj;
vector<edge> edges;

bool bfs(int s, int t, vector<int> &parent)
{
    int totalNodes = n + 5;
    vector<bool> vis(totalNodes, false);
    queue<int> q;
    q.push(s);
    vis[s] = true;
    parent[s] = -1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : adj[u])
        {
            if (!vis[v] && capacity[u][v] > 0)
            {
                vis[v] = true;
                parent[v] = u;
                if (v == t)
                {
                    return true;
                }
                q.push(v);
            }
        }
    }

    return false;
}

int edmondsKarp(int s, int t)
{
    int totalNodes = n + 5;
    vector<int> parent(totalNodes);
    int maxFlow = 0;

    while (bfs(s, t, parent))
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

int main()
{
    cin >> n >> k >> m;
    capacity.resize(n + 5, vector<int>(n + 5, 0));
    adj.resize(n + 5);
    src = n;
    snk = n + 1;

    for (int i = 0; i < k; i++)
    {
        if (capacity[src][i] == 0)
        {
            adj[src].push_back(i);
            adj[i].push_back(src);
        }
        capacity[src][i] += 1;
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
        capacity[u][v] += 1;
    }

    for (int i = k; i < n; i++)
    {
        if (capacity[i][snk] == 0)
        {
            adj[i].push_back(snk);
            adj[snk].push_back(i);
        }
        capacity[i][snk] += 1;
    }

    int maxFlow = edmondsKarp(src, snk);
    cout << maxFlow << endl;

    vector<pair<int, int>> matchings;
    for (int i = 0; i < k; i++)
    {
        for (int j = k; j < n; j++)
        {
            if (capacity[j][i] > 0)
            {
                matchings.push_back({i, j});
            }
        }
    }
    for (pair<int, int> p : matchings)
    {
        cout << p.first << " " << p.second << endl;
    }

    return 0;
}

// #include <iostream>
// #include <vector>
// #include <queue>
// #include <climits>
// #include <cstring>
// #include <tuple>
// using namespace std;

// int n = 1000, m, k;
// int src, snk;
// class edge
// {
// public:
//     int u, v, c;

//     edge(int u, int v, int c)
//     {
//         this->u = u;
//         this->v = v;
//         this->c = c;
//     }
// };

// vector<vector<int>> capacity;
// vector<vector<int>> adj;
// vector<edge> edges; // original edges (u, v, cap)

// bool bfs(int s, int t, vector<int> &parent)
// {
//     vector<bool> vis(n, false); // n dewa uchit maxflow theke copy kore ene;
//     queue<int> q;
//     q.push(s);
//     vis[s] = true;
//     parent[s] = -1;

//     while (!q.empty())
//     {
//         int u = q.front();
//         q.pop();

//         for (int v : adj[u])
//         {
//             if (!vis[v] && capacity[u][v] > 0)
//             {
//                 vis[v] = true;
//                 parent[v] = u;
//                 if (v == t)
//                 {
//                     return true;
//                 }
//                 q.push(v);
//             }
//         }
//     }

//     return false;
// }
// int edmondsKarp(int s, int t)
// {
//     vector<int> parent(n);
//     int maxFlow = 0;

//     // BFS diye augmenting path er existence check korchi
//     while (bfs(s, t, parent))
//     {

//         // path er bottleneck khujtesi
//         int pathFlow = INT_MAX;
//         for (int v = t; v != s; v = parent[v])
//         {
//             int u = parent[v];
//             pathFlow = min(pathFlow, capacity[u][v]);
//         }

//         // residual capacity update korchi
//         for (int v = t; v != s; v = parent[v])
//         {
//             int u = parent[v];
//             capacity[u][v] -= pathFlow;
//             capacity[v][u] += pathFlow;
//         }

//         maxFlow += pathFlow;
//     }

//     return maxFlow;
// }
// void print(int s, int t)
// {

//     vector<vector<int>> originalCapacity = capacity;
//     int maxFlow = edmondsKarp(s, t);

//     cout << maxFlow << endl;

//     for (auto &edge : edges)
//     {
//         int u = edge.u;
//         int v = edge.v;
//         int cap = edge.c;

//         // Flow = original capacity - residual capacity
//         int flow = cap - capacity[u][v];

//         cout << u << " " << v << " " << flow << "/" << cap << endl;
//     }
// }

// int main()
// {

//     cin >> n >> k >> m;
//     capacity.resize(n + 5, vector<int>(n + 5, 0));
//     adj.resize(n + 5);
//     src = n + 1;
//     snk = n + 2;
//     for (int i = 0; i < k; i++)
//     {

//         edges.push_back(edge(src, i, 1));

//         // Add edge to adjacency list if not already present
//         if (capacity[src][i] == 0)
//         {
//             adj[src].push_back(i);
//             // adj[v].push_back(u);
//         }

//         capacity[src][i] += 1;
//     }

//     for (int i = 0; i < m; i++)
//     {
//         int u, v;
//         cin >> u >> v;
//         edges.push_back(edge(u, v, 1));

//         // Add edge to adjacency list if not already present
//         if (capacity[u][v] == 0)
//         {
//             adj[u].push_back(v);
//             // adj[v].push_back(u);
//         }

//         capacity[u][v] += 1;
//     }

//     for (int i = k; i < n; i++)
//     {

//         edges.push_back(edge(i, snk, 1));

//         // Add edge to adjacency list if not already present
//         if (capacity[i][snk] == 0)
//         {
//             adj[i].push_back(snk);
//             // adj[v].push_back(u);
//         }

//         capacity[i][snk] += 1;
//     }

//     int maxFlow = edmondsKarp(src, snk);
//     cout << maxFlow << endl;

//     vector<pair<int, int>> matchings;
//     for (int i = 0; i < k; i++)
//     {
//         for (int j = k; j < n; j++)
//         {
//             if (capacity[j][i] > 0)
//             {
//                 matchings.push_back({i, j});
//             }
//         }
//     }
//     for (pair<int, int> p : matchings)
//     {
//         cout << p.first << " " << p.second << endl;
//     }

//     return 0;
// }
