/*
max flow with vertex capacity

Split each vertex v into two nodes: an "in-node" v_in and an "out-node" v_out.
    Add a directed edge from v_in to v_out with a capacity equal to the vertex capacity.

Redirect original edges: If there is an edge from u to v with capacity w, 
    it now becomes an edge from u_out to v_in with capacity w.

Source and Sink: The source becomes s_in 
(though effectively we start from s_out since s_in to s_out usually has infinite capacity in these problems) 
    and the sink becomes t_in.

see below for the print version    
*/

#include<bits/stdc++.h>
using namespace std;
using ll = long long int;

// Edmonds-Karp BFS to find augmenting paths
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
    // n = number of vertices, m = number of edges
    if (!(cin >> n >> m)) return 0;

    // We split each node i into i (in) and i + n (out)
    int total_nodes = 2 * n;

    vector<vector<int>> residual(total_nodes, vector<int>(total_nodes, 0));
    vector<vector<int>> adj(total_nodes);

    // 1. Handle Vertex Capacities
    for (int i = 0; i < n; ++i) {
        int v_cap;
        cin >> v_cap; // Input capacity for vertex i
        
        int in_node = i;
        int out_node = i + n;
        
        residual[in_node][out_node] = v_cap;
        adj[in_node].push_back(out_node);
        adj[out_node].push_back(in_node);
    }

    // 2. Handle Edge Capacities
    for (int i = 0; i < m; ++i) {
        int u, v, e_cap;
        cin >> u >> v >> e_cap;
        
        // Edge goes from u_out to v_in
        int u_out = u + n;
        int v_in = v;

        residual[u_out][v_in] = e_cap;
        adj[u_out].push_back(v_in);
        adj[v_in].push_back(u_out);
    }

    int s, t;
    cin >> s >> t;

    // The flow starts at s_in and must pass through s_in -> s_out (vertex cap)
    // and ends at t_in (after passing through the capacity of vertex t)
    // Therefore, we find flow from s_in (s) to t_out (t + n)
    cout << "Max Flow: " << maxflow(s, t + n, total_nodes, residual, adj) << endl;

    return 0;
}


/*  ---! same problem but with printing the original edges
int main() {
    int n, m;
    if (!(cin >> n >> m)) return 0;

    int total_nodes = 2 * n;
    vector<vector<int>> residual(total_nodes, vector<int>(total_nodes, 0));
    vector<vector<int>> adj(total_nodes);

    vector<int> v_caps(n);
    vector<vector<int>> original_v_residual(n); // To store initial state
    for (int i = 0; i < n; ++i) {
        cin >> v_caps[i];
        residual[i][i + n] = v_caps[i];
        adj[i].push_back(i + n);
        adj[i + n].push_back(i);
    }

    // Store original edge info for reporting
    struct Edge { int u, v, cap; };
    vector<Edge> edges;
    for (int i = 0; i < m; ++i) {
        int u, v, c;
        cin >> u >> v >> c;
        edges.push_back({u, v, c});
        residual[u + n][v] = c;
        adj[u + n].push_back(v);
        adj[v].push_back(u + n);
    }

    int s, t;
    cin >> s >> t;

    cout << "\n--- Results ---\n";
    cout << "Total Max Flow: " << maxflow(s, t + n, total_nodes, residual, adj) << endl;

    cout << "\nVertex Flow Usage:\n";
    for (int i = 0; i < n; ++i) {
        int flow_used = v_caps[i] - residual[i][i + n];
        cout << "Vertex " << i << ": " << flow_used << "/" << v_caps[i] << endl;
    }

    cout << "\nEdge Flow Usage:\n";
    for (auto& e : edges) {
        int flow_used = e.cap - residual[e.u + n][e.v];
        cout << e.u << " -> " << e.v << ": " << flow_used << "/" << e.cap << endl;
    }

    return 0;
}
*/