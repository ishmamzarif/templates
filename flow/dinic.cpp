#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = 1e18;
using pll = pair<ll, ll>;
using tll = tuple<ll, ll, ll>;
using vi = vector<int>;
using vvi = vector<vi>;
using vll = vector<ll>;
using vvll = vector<vll>;

// ================== EDMONDS-KARP ==================
void edmond_karp(vvi& adj, vvll& flow, vvll& capacity, int source, int sink, int N) {
    ll max_flow = 0;
    
    while (true) {
        vi parent(N, -1);
        queue<int> q;
        q.push(source);
        vector<bool> vis(N, false);
        vis[source] = true;

        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                ll rem_capacity = capacity[u][v] - flow[u][v];
                if (!vis[v] && rem_capacity > 0) {
                    q.push(v);
                    vis[v] = true;
                    parent[v] = u;
                    if (v == sink) break;
                }
            }
        }

        if (parent[sink] == -1) break;

        ll bottleneck = INF;
        for (int node = sink; parent[node] != -1; node = parent[node]) {
            int u = parent[node], v = node;
            bottleneck = min(bottleneck, capacity[u][v] - flow[u][v]);
        }

        for (int node = sink; parent[node] != -1; node = parent[node]) {
            int u = parent[node], v = node;
            flow[u][v] += bottleneck;
            flow[v][u] -= bottleneck;
        }

        max_flow += bottleneck;
    }

    printf("%lld\n", max_flow);
}

// ================== DINIC ==================
struct Dinic {
    int N;
    vvi adj;
    vvll capacity, flow;
    vi level, ptr;
    int source, sink;

    Dinic(int n, vvi& _adj, vvll& _capacity, int s, int t) {
        N = n; adj = _adj; capacity = _capacity;
        flow.assign(N, vll(N, 0));
        level.assign(N, -1);
        ptr.assign(N, 0);
        source = s; sink = t;
    }

    bool bfs() {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        q.push(source);
        level[source] = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (level[v] == -1 && capacity[u][v] - flow[u][v] > 0) {
                    level[v] = level[u] + 1;
                    q.push(v);
                }
            }
        }
        return level[sink] != -1;
    }

    ll dfs(int u, ll pushed) {
        if (u == sink) return pushed;
        for (int& i = ptr[u]; i < (int)adj[u].size(); i++) {
            int v = adj[u][i];
            if (level[v] == level[u] + 1 && capacity[u][v] - flow[u][v] > 0) {
                ll tr = dfs(v, min(pushed, capacity[u][v] - flow[u][v]));
                if (tr > 0) {
                    flow[u][v] += tr;
                    flow[v][u] -= tr;
                    return tr;
                }
            }
        }
        return 0;
    }

    ll max_flow() {
        ll total = 0;
        while (bfs()) {
            fill(ptr.begin(), ptr.end(), 0);
            while (ll pushed = dfs(source, INF)) {
                total += pushed;
            }
        }
        return total;
    }
};

int main(void) {
    int N, M;
    cin >> N >> M;

    vvi adj(N);
    vvll capacity(N, vll(N, 0));
    vvi edges;

    for (int i = 0; i < M; i++) {
        int u, v;
        ll cap;
        cin >> u >> v >> cap;
        adj[u].push_back(v);
        adj[v].push_back(u); // residual edge
        capacity[u][v] = cap;
        edges.push_back({u, v});
    }

    int source, sink;
    cin >> source >> sink;

    // --- Edmonds-Karp ---
    vvll flow_ek(N, vll(N, 0));
    edmond_karp(adj, flow_ek, capacity, source, sink, N);
    for (vi& edge : edges) {
        int u = edge[0], v = edge[1];
        printf("%d %d %lld/%lld\n", u, v, flow_ek[u][v], capacity[u][v]);
    }

    // --- Dinic ---
    Dinic dinic(N, adj, capacity, source, sink);
    ll maxflow_dinic = dinic.max_flow();
    printf("Dinic: %lld\n", maxflow_dinic);
    for (vi& edge : edges) {
        int u = edge[0], v = edge[1];
        printf("%d %d %lld/%lld\n", u, v, dinic.flow[u][v], capacity[u][v]);
    }

    return 0;
}
