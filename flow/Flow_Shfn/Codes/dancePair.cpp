#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAXN = 305;
int capacity[MAXN][MAXN];
int originalCap[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];

bool bfs(int s, int t, int total)
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
            if (v < total && parent[v] == -1 && capacity[u][v] > 0)
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

int edmondsKarp(int s, int t, int total)
{
    int maxFlow = 0;

    while (bfs(s, t, total))
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
    int m, n; // m = max dances per pair, n = max dances per student
    cin >> m >> n;

    int X, Y; // X girls, Y boys
    cin >> X >> Y;

    int p; // number of registered pairs
    cin >> p;

    vector<pair<int, int>> pairs;

    // Reset
    memset(capacity, 0, sizeof(capacity));
    memset(originalCap, 0, sizeof(originalCap));
    for (int i = 0; i < MAXN; i++)
    {
        adj[i].clear();
    }

    int src = 0;
    int sink = X + Y + 1;
    int total = sink + 1;

    // Girls are nodes 1 to X
    // Boys are nodes X+1 to X+Y

    // Connect source to all girls with capacity n
    for (int i = 1; i <= X; i++)
    {
        adj[src].push_back(i);
        adj[i].push_back(src);
        capacity[src][i] = n;
    }

    // Read registered pairs and connect girls to boys with capacity m
    for (int i = 0; i < p; i++)
    {
        int x, y;
        cin >> x >> y;

        pairs.push_back({x, y});

        int girlNode = x + 1;
        int boyNode = X + y + 1;

        if (capacity[girlNode][boyNode] == 0)
        {
            adj[girlNode].push_back(boyNode);
            adj[boyNode].push_back(girlNode);
        }
        capacity[girlNode][boyNode] = m;
        originalCap[girlNode][boyNode] = m;
    }

    // Connect all boys to sink with capacity n
    for (int i = 1; i <= Y; i++)
    {
        int boyNode = X + i;
        adj[boyNode].push_back(sink);
        adj[sink].push_back(boyNode);
        capacity[boyNode][sink] = n;
    }

    int maxFlow = edmondsKarp(src, sink, total);

    // Extract flow for each pair
    for (auto &pr : pairs)
    {
        int x = pr.first;
        int y = pr.second;

        int girlNode = x + 1;
        int boyNode = X + y + 1;

        int flow = originalCap[girlNode][boyNode] - capacity[girlNode][boyNode];
        // int flow = capacity[boyNode][girlNode];  // Direct reverse edge capacity!

        if (flow > 0)
        {
            cout << "(" << x << "," << y << ") -> " << flow;
            if (flow == 1)
            {
                cout << " dance" << endl;
            }
            else
            {
                cout << " dances" << endl;
            }
        }
    }

    return 0;
}