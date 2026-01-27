/*
two paths are independent paths if they do not share any vertex apart from source and sink 
    and also they don't share any edges
find the max number of independent paths and print them

similar with max_flow with vertex capacity
here each edge has capacity 1 and each vertex has capacity 1

run max_flow from source (s_in) to sink (t_out)
use findPaths function to print them
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

// Function to backtrack through the residual graph and find the actual paths
void findPaths(int curr, int t_out, int n, vector<vector<int>>& residual, vector<int>& path, vector<bool>& visited) {
    if (curr == t_out) {
        // Print the path: only print "in-nodes" (0 to n-1)
        for (int i = 0; i < path.size(); ++i) {
            if (path[i] < n) cout << path[i] << (i == path.size() - 2 ? "" : " -> ");
        }
        cout << endl;
        return;
    }

    // Nodes are split: 0..n-1 are IN, n..2n-1 are OUT
    for (int next = 0; next < 2 * n; ++next) {
        // If there was an original capacity and now there is flow (residual is 0)
        // Note: For capacity 1, if residual is 0, it means flow was sent.
        // We check if the reverse edge has flow.
        if (residual[next][curr] > 0) { 
            residual[next][curr]--; // Consume this flow so we don't reuse the path
            path.push_back(next);
            findPaths(next, t_out, n, residual, path, visited);
            return;
        }
    }
}

int main() {
    int n, m;
    cout << "Enter number of vertices and edges: ";
    cin >> n >> m;

    int total_nodes = 2 * n;
    vector<vector<int>> residual(total_nodes, vector<int>(total_nodes, 0));
    vector<vector<int>> adj(total_nodes);

    int s, t;
    cout << "Enter source and sink: ";
    cin >> s >> t;

    // 1. Vertex Capacities: Set to 1 for independent paths
    // Source and Sink are given large capacity as they can be reused
    for (int i = 0; i < n; ++i) {
        int v_cap = (i == s || i == t) ? m : 1; 
        int in_node = i;
        int out_node = i + n;
        residual[in_node][out_node] = v_cap;
        adj[in_node].push_back(out_node);
        adj[out_node].push_back(in_node);
    }

    // 2. Edge Capacities: Set to 1
    cout << "Enter " << m << " edges (u v):" << endl;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        int u_out = u + n;
        int v_in = v;
        residual[u_out][v_in] = 1;
        adj[u_out].push_back(v_in);
        adj[v_in].push_back(u_out);
    }

    int start_node = s;      // s_in
    int end_node = t + n;    // t_out

    ll max_paths = maxflow(start_node, end_node, total_nodes, residual, adj);

    cout << "\nMaximum number of independent paths: " << max_paths << endl;
    
    cout << "The paths are:" << endl;
    for (int i = 0; i < max_paths; ++i) {
        vector<int> path = {s};
        vector<bool> visited(total_nodes, false);
        findPaths(s + n, end_node, n, residual, path, visited);
    }

    return 0;
}
