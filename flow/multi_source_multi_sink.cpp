/*
multiple source multiple sink problem
use super source and super sink with infinite capacity
*/

#include<bits/stdc++.h>
using namespace std;
using ll = long long int;

int bfs(int s, int t, vector<int>& parent, vector<vector<int>>& adj, vector<vector<int>>& residual) {
    fill(parent.begin(), parent.end(), -1);
    parent[s] = -2;
    queue<pair<int, int>> q;
    q.push({s, INT_MAX});

    while (!q.empty()) {
        int curr = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int next : adj[curr]) {
            if (parent[next] == -1 && residual[curr][next] > 0) {
                parent[next] = curr;
                int new_flow = min(flow, residual[curr][next]);
                if (next == t) return new_flow;
                q.push({next, new_flow});
            }
        }
    }
    return 0;
}

ll maxflow(int s, int t, int total_nodes, vector<vector<int>>& residual, vector<vector<int>>& adj) {
    ll flow = 0;
    vector<int> parent(total_nodes);
    int new_flow = 0;

    while ((new_flow = bfs(s, t, parent, adj, residual)) > 0) {
        flow += new_flow;
        int curr = t;
        while (curr != s) {
            int prev = parent[curr];
            residual[prev][curr] -= new_flow;
            residual[curr][prev] += new_flow;
            curr = prev;
        }
    }
    return flow;
}

int main() {
    int n, m;
    if(!(cin >> n >> m)) return 0;

    // total_nodes = n original + 1 super-source + 1 super-sink
    int S = n, T = n + 1;
    int total_nodes = n + 2;

    vector<vector<int>> residual(total_nodes, vector<int>(total_nodes, 0));
    vector<vector<int>> adj(total_nodes);

    // 1. Read original edges
    for (int i = 0; i < m; ++i) {
        int u, v, c;
        cin >> u >> v >> c;
        residual[u][v] += c;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // 2. Add Super-Source connections
    int num_sources;
    cout << "Enter number of sources: ";
    cin >> num_sources;
    for(int i = 0; i < num_sources; i++){
        int src_node;
        cin >> src_node;
        residual[S][src_node] = INT_MAX; // Capacity from Super-Source
        adj[S].push_back(src_node);
        adj[src_node].push_back(S);
    }

    // 3. Add Super-Sink connections
    int num_sinks;
    cout << "Enter number of sinks: ";
    cin >> num_sinks;
    for(int i = 0; i < num_sinks; i++){
        int sink_node;
        cin >> sink_node;
        residual[sink_node][T] = INT_MAX; // Capacity to Super-Sink
        adj[sink_node].push_back(T);
        adj[T].push_back(sink_node);
    }

    cout << "\nMax Flow: " << maxflow(S, T, total_nodes, residual, adj) << endl;

    return 0;
}