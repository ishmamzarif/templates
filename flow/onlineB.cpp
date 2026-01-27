/* NMS */

/* Online B but easier to understand */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int INF = 1e9;

vector<vector<int>> adj;
vector<vector<int>> capacity;
vector<int> parent;
vector<bool> visited;

/* BFS for Edmonds–Karp */
int bfs(int s, int t, int n)
{
    parent.assign(n + 1, -1);
    parent[s] = -2;

    queue<pair<int, int>> q;
    q.push(make_pair(s, INF));

    while (!q.empty())
    {
        int u = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (auto v : adj[u])
        {
            if (parent[v] == -1 && capacity[u][v] > 0)
            {
                parent[v] = u;
                int newflow = min(flow, capacity[u][v]);

                if (v == t)
                    return newflow;

                q.push(make_pair(v, newflow));
            }
        }
    }
    return 0;
}

/* Edmonds–Karp */
int maxFlow(int s, int t, int n)
{
    int flow, result = 0;

    while ((flow = bfs(s, t, n)) > 0)
    {
        result += flow;
        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            capacity[u][v] -= flow;
            capacity[v][u] += flow;
        }
    }
    return result;
}

/* DFS to find reachable nodes in residual graph */
void dfs(int u)
{
    visited[u] = true;
    for (auto v : adj[u])
    {
        if (!visited[v] && capacity[u][v] > 0)
            dfs(v);
    }
}

int main()
{
    int N, M;
    cin >> N >> M;

    adj.assign(N + 1, vector<int>());
    capacity.assign(N + 1, vector<int>(N + 1, 0));
    parent.assign(N + 1, -1);

    for (int i = 0; i < M; i++)
    {
        int u, v, c;
        cin >> u >> v >> c;
        adj[u].push_back(v);
        adj[v].push_back(u); // residual edge
        capacity[u][v] += c;
    }

    /* Compute original max flow */
    maxFlow(1, N, N);

    /* Find min-cut: reachable nodes from source */
    visited.assign(N + 1, false);
    dfs(1);

    int P;
    cin >> P;

    vector<int> answer;
    for (int i = 1; i <= P; i++)
    {
        int u, v, c;
        cin >> u >> v >> c;

        // Flyover increases flow if it crosses min-cut
        if (visited[u] && !visited[v])
            answer.push_back(i);
    }

    if (answer.empty())
    {
        cout << "None" << endl;
    }
    else
    {
        for (int i = 0; i < answer.size(); i++)
        {
            cout << answer[i] << endl;
        }
    }

    return 0;
}