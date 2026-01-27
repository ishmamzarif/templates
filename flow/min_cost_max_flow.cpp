/*
    min cost max flow(mcmf) 
    finding the cheapest possible way of sending certain amount of flow(usually the max flow)
    through a flow network. 
    In this problem, every edge has two attributes: flow capacity and the cost to use the edge

    use bellman-ford instead of bfs
    bfs finds a path with minimum number of edges
    bellman-ford finds a path with minimum cost(with negative cost--- here for residual graph it is needed)
*/

#include <bits/stdc++.h>
using namespace std;

using ll = long long int;
const ll INF = 1e18;

struct Edge {
    int u, v, capacity, flow, cost, rev;
};

vector<vector<Edge>> adj;
vector<ll> dist;
vector<int> parent_node;
vector<int> parent_edge;

void add_edge(int u, int v, int cap, int cost) {
    // Forward edge
    adj[u].push_back({u, v, cap, 0, cost, (int)adj[v].size()});
    // Backward edge
    adj[v].push_back({v, u, 0, 0, -cost, (int)adj[u].size() - 1});
}

bool bellman_ford(int s, int t, int n, int &flow, ll &cost) {
    dist.assign(n, INF);
    parent_node.assign(n, -1);
    parent_edge.assign(n, -1);
    dist[s] = 0;

    // Relax edges |V|-1 times
    for (int i = 0; i < n - 1; ++i) {
        for (int u = 0; u < n; ++u) {
            if (dist[u] == INF) continue;
            for (int j = 0; j < adj[u].size(); ++j) {
                Edge &e = adj[u][j];
                if (e.capacity - e.flow > 0 && dist[e.v] > dist[u] + e.cost) {
                    dist[e.v] = dist[u] + e.cost;
                    parent_node[e.v] = u;
                    parent_edge[e.v] = j;
                }
            }
        }
    }

    if (dist[t] == INF) return false;

    // Bottleneck
    int push = INT_MAX;
    int curr = t;
    while (curr != s) {
        int prev = parent_node[curr];
        int idx = parent_edge[curr];
        push = min(push, adj[prev][idx].capacity - adj[prev][idx].flow);
        curr = prev;
    }

    // Update flow and cost
    flow += push;
    cost += (ll)push * dist[t];

    // Update residual graph
    curr = t;
    while (curr != s) {
        int prev = parent_node[curr];
        int idx = parent_edge[curr];
        int rev_idx = adj[prev][idx].rev;

        adj[prev][idx].flow += push;
        adj[curr][rev_idx].flow -= push;
        curr = prev;
    }

    return true;
}

/* ---------------- MIN CUT ---------------- */

vector<int> mincut_side; // 1 = reachable from source, 0 = not reachable

void dfs_residual(int u) {
    mincut_side[u] = 1;
    for (auto &e : adj[u]) {
        // residual capacity > 0
        if (!mincut_side[e.v] && (e.capacity - e.flow > 0)) {
            dfs_residual(e.v);
        }
    }
}

void find_min_cut(int s, int n) {
    mincut_side.assign(n, 0);

    // Step 1: DFS/BFS in residual graph
    dfs_residual(s);

    cout << "\n--- MIN CUT ---\n";
    cout << "Set S (reachable from source): ";
    for (int i = 0; i < n; i++)
        if (mincut_side[i]) cout << i << " ";
    cout << "\n";

    cout << "Set T (not reachable): ";
    for (int i = 0; i < n; i++)
        if (!mincut_side[i]) cout << i << " ";
    cout << "\n\n";

    cout << "Min-Cut Edges (u -> v):\n";

    // Step 2: edges from S to T in original graph
    for (int u = 0; u < n; u++) {
        if (!mincut_side[u]) continue;
        for (auto &e : adj[u]) {
            if (!mincut_side[e.v] && e.capacity > 0) {
                cout << u << " -> " << e.v 
                     << " | cap = " << e.capacity 
                     << " | flow = " << e.flow << "\n";
            }
        }
    }
}

/* ---------------- MAIN ---------------- */

int main() {
    int n, m;
    if (!(cin >> n >> m)) return 0;

    adj.assign(n, vector<Edge>());

    for (int i = 0; i < m; ++i) {
        int u, v, cap, cost;
        cin >> u >> v >> cap >> cost;
        add_edge(u, v, cap, cost);
    }

    int s, t;
    cin >> s >> t;

    int total_flow = 0;
    ll min_cost = 0;

    // MCMF
    while (bellman_ford(s, t, n, total_flow, min_cost));

    cout << "Max Flow: " << total_flow << endl;
    cout << "Min Cost: " << min_cost << endl;

    // MIN CUT
    find_min_cut(s, n);

    return 0;
}
