#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring>
#include <tuple>
using namespace std;

int n = 1000, m;
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
vector<vector<int>> originalCap;
vector<vector<int>> adj;
vector<edge> edges;

bool bfs(int s, int t, vector<int> &parent)
{
    vector<bool> vis(n, false);
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
    vector<int> parent(n);
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

void print(int s, int t)
{
    int maxFlow = edmondsKarp(s, t);

    cout << maxFlow << endl;

    for (auto &edge : edges)
    {
        int u = edge.u;
        int v = edge.v;
        int cap = edge.c;

        int flow;
        if (originalCap[v][u] > 0) // handling antiparallel edges;
        {
            int usedCapacity = originalCap[u][v] - capacity[u][v];
            int reverseFlow = capacity[v][u] - originalCap[v][u];
            flow = usedCapacity - reverseFlow;
            flow = max(0, min(cap, flow));
        }
        else
        {
            flow = originalCap[u][v] - capacity[u][v];
        }

        cout << u << " " << v << " " << flow << "/" << cap << endl;
    }
}

int main()
{
    cin >> n >> m;
    capacity.resize(n, vector<int>(n, 0));
    originalCap.resize(n, vector<int>(n, 0));
    adj.resize(n);

    for (int i = 0; i < m; i++)
    {
        int u, v, c;
        cin >> u >> v >> c;
        edges.push_back(edge(u, v, c));

        originalCap[u][v] += c;

        adj[u].push_back(v);
        adj[v].push_back(u); // eta important for the residual graph, jate travel korte pare

        capacity[u][v] += c;
    }

    int s, t;
    cin >> s >> t;

    print(s, t);

    return 0;
}

// #include <iostream>
// #include <vector>
// #include <queue>
// #include <climits>
// #include <cstring>
// #include <tuple>
// using namespace std;

// int n = 1000, m;
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

//     cin >> n >> m;
//     capacity.resize(n, vector<int>(n, 0));
//     adj.resize(n);

//     for (int i = 0; i < m; i++)
//     {
//         int u, v, c;
//         cin >> u >> v >> c;
//         edges.push_back(edge(u, v, c));

//         // Add edge to adjacency list if not already present
//         if (capacity[u][v] == 0 && c > 0)
//         {
//             adj[u].push_back(v);
//             // adj[v].push_back(u);
//         }

//         capacity[u][v] += c;
//     }

//     int s, t;
//     cin >> s >> t;

//     print(s, t);

//     return 0;
// }
