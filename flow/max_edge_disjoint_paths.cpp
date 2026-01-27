/*
same problem of maximum independent paths
but here the vertex constraint is relaxed
-- meaning two edge-disjoint paths can have same vertex passing through
    but not the same edges

remove the vertex splitting
then normal max flow
use findPaths for printing the paths
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

ll maxflow(int s, int t, int n, vector<vector<int>>& residual, vector<vector<int>>& adj) {
    ll flow = 0;
    vector<int> parent(n);
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

// Backtrack to find paths by following edges with used capacity
void findPaths(int curr, int t, vector<vector<int>>& residual, vector<int>& path) {
    if (curr == t) {
        for (int i = 0; i < path.size(); ++i) {
            cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
        }
        cout << endl;
        return;
    }

    // Check all possible next nodes
    for (int next = 0; next < residual.size(); ++next) {
        // If residual[next][curr] > 0, it means flow was sent from curr to next
        // (because the backward edge in residual graph increases when flow is sent)
        if (residual[next][curr] == 1) { 
            // In a simple capacity 1 graph, if backward > 0 and we haven't used it:
            --residual[next][curr]; // remove flow
            path.push_back(next);
            findPaths(next, t, residual, path);
            return;
        }
    }
}

int main() {
    int n, m;
    cout << "Enter number of vertices and edges: ";
    cin >> n >> m;

    // Use original n nodes
    vector<vector<int>> residual(n, vector<int>(n, 0));
    vector<vector<int>> adj(n);

    int s, t;
    cout << "Enter source and sink: ";
    cin >> s >> t;

    cout << "Enter " << m << " edges (u v):" << endl;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        // Capacity 1 for edge-disjoint paths
        residual[u][v] = 1; 
        adj[u].push_back(v);
        adj[v].push_back(u); // Reverse edge for residual graph
    }

    ll max_paths = maxflow(s, t, n, residual, adj);

    cout << "\nMaximum number of edge-disjoint paths: " << max_paths << endl;
    
    cout << "The paths are:" << endl;
    for (int i = 0; i < max_paths; ++i) {
        vector<int> path = {s};
        findPaths(s, t, residual, path);
    }

    return 0;
}