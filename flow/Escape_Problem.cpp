#include <bits/stdc++.h>
using namespace std;

int bfs(int s, int t, int n, vector<int> &parent, vector<vector<int>> &capacity, vector<vector<int>> &adj)
{
    parent.assign(n, -1);
    parent[s] = -2;
    queue<pair<int, int>> q;
    q.push({s, INT_MAX});

    while (!q.empty())
    {
        int u = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int v : adj[u])
        {
            if (parent[v] == -1 && capacity[u][v] != 0)
            {
                parent[v] = u;
                int new_flow = min(flow, capacity[u][v]);
                if (v == t)
                    return new_flow;
                q.push({v, new_flow});
            }
        }
    }
    return 0;
}

int maxflow(int s, int t, int n, vector<vector<int>> &capacity, vector<vector<int>> &adj, vector<vector<int>> &flows)
{
    int flow = 0;
    vector<int> parent(n);
    int new_flow;
    while (new_flow = bfs(s, t, n, parent, capacity, adj))
    {
        flow += new_flow;
        int cur = t;
        while (cur != s)
        {
            int prev = parent[cur];
            capacity[prev][cur] -= new_flow;
            capacity[cur][prev] += new_flow;

            flows[prev][cur] += new_flow;
            flows[cur][prev] -= new_flow;
            cur = prev;
        }
    }
    return flow;
}

int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

int main()
{
    // R = Rows, C = Cols
    int R, C;
    if (!(cin >> R >> C))
        return 0;

    int startR, startC;
    cin >> startR >> startC;

    vector<vector<int>> grid(R, vector<int>(C));
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            cin >> grid[r][c]; // 1 = Obstacle, 0 = Empty
        }
    }

    // Node Calculation:
    // Cells 0 to (R*C - 1) are grid nodes.
    // Node (R*C) is the Super Sink.
    int num_nodes = R * C + 1;
    int t = R * C;
    int s = startR * C + startC;

    // Initialize Structures
    vector<vector<int>> capacity(num_nodes, vector<int>(num_nodes, 0));
    vector<vector<int>> adj(num_nodes);
    vector<vector<int>> flows(num_nodes, vector<int>(num_nodes, 0)); // Dummy for template

    // Build Graph
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            if (grid[r][c] == 1)
                continue; // Skip obstacles

            int u = r * C + c;

            // 1. Connect to valid neighbors
            for (int i = 0; i < 4; i++)
            {
                int nr = r + dr[i];
                int nc = c + dc[i];

                // Check bounds
                if (nr >= 0 && nr < R && nc >= 0 && nc < C)
                {
                    if (grid[nr][nc] == 0)
                    { // Only connect if neighbor is empty
                        int v = nr * C + nc;
                        // Add undirected edge capacity 1 (or directed if one-way movement)
                        // Assuming movement is allowed u -> v
                        capacity[u][v] = 1;
                        adj[u].push_back(v);
                        adj[v].push_back(u); // Reverse edge reference
                    }
                }
            }

            // 2. Connect Boundary to Super Sink
            // Any cell on the edge of the grid can "escape"
            if (r == 0 || r == R - 1 || c == 0 || c == C - 1)
            {
                capacity[u][t] = 1000000; // INF capacity to sink
                adj[u].push_back(t);
                adj[t].push_back(u);
            }
        }
    }

    if (grid[startR][startC] == 1)
    {
        cout << "Max Escape Paths: 0" << endl;
    }
    else
    {
        cout << "Max Escape Paths: " << maxflow(s, t, num_nodes, capacity, adj, flows) << endl;
    }

    return 0;
}