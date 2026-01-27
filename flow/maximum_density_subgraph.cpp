#include <bits/stdc++.h> 
using namespace std;

const double EPS = 1e-7;
const double INF = 1e15;

struct Edge {
    int to;
    double capacity;
    double flow;
    int rev;
};

vector<vector<Edge>> adj;
vector<int> parent;
vector<int> edge_idx;

void add_edge(int u, int v, double cap) {
    adj[u].push_back({v, cap, 0, (int)adj[v].size()});
    adj[v].push_back({u, 0, 0, (int)adj[u].size() - 1});
}

double bfs(int s, int t) {
    fill(parent.begin(), parent.end(), -1);
    queue<pair<int, double>> q;
    q.push({s, INF});
    parent[s] = s;

    while (!q.empty()) {
        int u = q.front().first;
        double flow = q.front().second;
        q.pop();

        for (int i = 0; i < adj[u].size(); i++) {
            Edge &e = adj[u][i];
            if (parent[e.to] == -1 && e.capacity - e.flow > EPS) {
                parent[e.to] = u;
                edge_idx[e.to] = i;
                double new_flow = min(flow, e.capacity - e.flow);
                if (e.to == t) return new_flow;
                q.push({e.to, new_flow});
            }
        }
    }
    return 0;
}

double edmonds_karp(int s, int t) {
    double max_flow = 0;
    double flow;
    while ((flow = bfs(s, t)) > EPS) {
        max_flow += flow;
        int curr = t;
        while (curr != s) {
            int prev = parent[curr];
            int idx = edge_idx[curr];
            adj[prev][idx].flow += flow;
            int rev_idx = adj[prev][idx].rev;
            adj[curr][rev_idx].flow -= flow;
            curr = prev;
        }
    }
    return max_flow;
}

int main() {
    int n, m;
    cin >> n >> m;
    vector<pair<int, int>> edges(m);
    vector<int> degree(n + 1, 0);

    for (int i = 0; i < m; i++) {
        cin >> edges[i].first >> edges[i].second;
        degree[edges[i].first]++;
        degree[edges[i].second]++;
    }

    double low = 0, high = m;
    int source = 0, sink = n + 1;

    // Binary search for density g
    for (int iter = 0; iter < 50; iter++) {
        double g = (low + high) / 2.0;
        adj.assign(n + 2, vector<Edge>());
        parent.resize(n + 2);
        edge_idx.resize(n + 2);

        for (auto &e : edges) {
            add_edge(e.first, e.second, 1.0);
            add_edge(e.second, e.first, 1.0);
        }
        for (int i = 1; i <= n; i++) {
            add_edge(source, i, (double)m);
            add_edge(i, sink, (double)m + 2.0 * g - (double)degree[i]);
        }

        double max_f = edmonds_karp(source, sink);
        // The condition (n*m - max_f) / 2 > 0 determines if g is feasible
        if ((double)n * m - max_f > EPS) low = g;
        else high = g;
    }

    cout << "Maximum Density: " << low << endl;
    return 0;
}