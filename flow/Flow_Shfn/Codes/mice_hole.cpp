#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <cstring>
using namespace std;

// src=0 , mice ->1 to n , holes->m+1 to m+h, sink->m+h+1;

// So src theke proti mouse and proti mouse theke reachable holes ke 1 kore dilam,
// hole er capacity ache and shetake edge weight dhore connect kore dilam with sink.
// Maxmimum bipartite matching i holo;

const int MAXN = 305;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];

struct Point
{
    double x, y;
};

double distance(Point a, Point b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

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
        int pathFlow = 1;

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
    int T;
    cin >> T;

    for (int tc = 1; tc <= T; tc++)
    {
        int M, H;
        double R;
        cin >> M >> H >> R;

        vector<Point> mice(M);
        for (int i = 0; i < M; i++)
        {
            cin >> mice[i].x >> mice[i].y;
        }

        vector<Point> holes(H);
        vector<int> capacities(H);
        for (int i = 0; i < H; i++)
        {
            cin >> holes[i].x >> holes[i].y >> capacities[i];
        }

        // Reset
        memset(capacity, 0, sizeof(capacity));
        for (int i = 0; i < MAXN; i++)
        {
            adj[i].clear();
        }

        int src = 0;
        int sink = M + H + 1;
        int total = sink + 1;

        // Connect source to mice
        for (int i = 0; i < M; i++)
        {
            adj[src].push_back(i + 1);
            adj[i + 1].push_back(src);
            capacity[src][i + 1] = 1;
        }

        // Connect mice to reachable holes
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < H; j++)
            {
                if (distance(mice[i], holes[j]) <= R)
                {
                    int mouseNode = i + 1;
                    int holeNode = M + j + 1;
                    adj[mouseNode].push_back(holeNode);
                    adj[holeNode].push_back(mouseNode);
                    capacity[mouseNode][holeNode] = 1;
                }
            }
        }

        // Connect holes to sink
        for (int i = 0; i < H; i++)
        {
            int holeNode = M + i + 1;
            adj[holeNode].push_back(sink);
            adj[sink].push_back(holeNode);
            capacity[holeNode][sink] = capacities[i];
        }

        int maxFlow = edmondsKarp(src, sink, total);

        cout << "Case " << tc << ": " << maxFlow << endl;
    }

    return 0;
}
