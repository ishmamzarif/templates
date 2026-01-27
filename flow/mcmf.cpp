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
    // Forward edge: rev points to the next available slot in adj[v]
    adj[u].push_back({u, v, cap, 0, cost, (int)adj[v].size()});
    // Backward edge (initial capacity 0, cost is negative)
    // Backward edge: rev points to the slot we just used in adj[u]
    adj[v].push_back({v, u, 0, 0, -cost, (int)adj[u].size() - 1});
}

bool bellman_ford(int s, int t, int n, int &flow, ll &cost) {
    dist.assign(n, INF);
    parent_node.assign(n, -1);
    parent_edge.assign(n, -1);
    dist[s] = 0;

    // Standard Bellman-Ford: Relax all edges |V| - 1 times
    for (int i = 0; i < n - 1; ++i) {
        for (int u = 0; u < n; ++u) {
            for (int j = 0; j < adj[u].size(); ++j) {
                Edge &e = adj[u][j];
                if (e.capacity - e.flow > 0 && dist[e.u] != INF && dist[e.v] > dist[e.u] + e.cost) {
                    dist[e.v] = dist[e.u] + e.cost;
                    parent_node[e.v] = u;
                    parent_edge[e.v] = j;
                }
            }
        }
    }

    // If sink is unreachable, no more flow can be pushed
    if (dist[t] == INF) return false;

    // Determine the bottleneck capacity along the path found
    int push = INT_MAX;
    int curr = t;
    while (curr != s) {
        int prev = parent_node[curr];
        int idx = parent_edge[curr];
        push = min(push, adj[prev][idx].capacity - adj[prev][idx].flow);
        curr = prev;
    }

    // Update flow and total cost
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

    // Repeatedly find the cheapest path using Bellman-Ford
    while (bellman_ford(s, t, n, total_flow, min_cost));

    cout << "Max Flow: " << total_flow << endl;
    cout << "Min Cost: " << min_cost << endl;

    return 0;
}