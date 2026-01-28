#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
using namespace std;

// Connect workers to tasks they can complete
// A worker can complete a task if worker's strength >= task's required strength

const int MAXN = 205;
int capacity[MAXN][MAXN];
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
    int N, M; // N workers, M tasks
    cin >> N >> M;

    vector<int> workerStrength(N);
    for (int i = 0; i < N; i++)
    {
        cin >> workerStrength[i];
    }

    vector<int> taskStrength(M);
    for (int i = 0; i < M; i++)
    {
        cin >> taskStrength[i];
    }

    // Reset
    memset(capacity, 0, sizeof(capacity));
    for (int i = 0; i < MAXN; i++)
    {
        adj[i].clear();
    }

    int src = 0;
    int sink = N + M + 1;
    int total = sink + 1;

    // Workers are nodes 1 to N
    // Tasks are nodes N+1 to N+M

    // Connect source to all workers
    for (int i = 1; i <= N; i++)
    {
        adj[src].push_back(i);
        adj[i].push_back(src);
        capacity[src][i] = 1;
    }

    // Connect workers to tasks they can complete
    // A worker can complete a task if worker's strength >= task's required strength
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (workerStrength[i] >= taskStrength[j]) // check one condition and we are done;
            {
                int workerNode = i + 1;
                int taskNode = N + j + 1;

                if (capacity[workerNode][taskNode] == 0)
                {
                    adj[workerNode].push_back(taskNode);
                    adj[taskNode].push_back(workerNode);
                }
                capacity[workerNode][taskNode] = 1;
            }
        }
    }

    // Connect all tasks to sink
    for (int i = 1; i <= M; i++)
    {
        int taskNode = N + i;
        adj[taskNode].push_back(sink);
        adj[sink].push_back(taskNode);
        capacity[taskNode][sink] = 1;
    }

    int maxFlow = edmondsKarp(src, sink, total);

    cout << maxFlow << endl;

    return 0;
}