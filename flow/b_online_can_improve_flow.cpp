// b online

// solve ->
// first run edmond-karp
// then (u, v) will increase flow from S to T
// if S->u reachable AND v->T reachable
// bfs/dfs from S and bfs/dfs from T on reverse_graph

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

void edmond_karp(vvi& adj, vvll& flow, vvll& capacity, int source, int sink, int N) {
    // run bfs to find augmenting paths
    ll max_flow = 0;
    
    while (true) {
        vi parent(N, -1);

        queue<int> q;
        q.push(source);

        vector<bool> vis(N, false);
        vis[source] = true;

        // the bfs
        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                ll rem_capacity = capacity[u][v] - flow[u][v];
                if (!vis[v] && rem_capacity > 0) {
                    q.push(v);
                    vis[v] = true;
                    parent[v] = u;

                    // break as i find the sink
                    if (v == sink) {
                        break;
                    }
                }
            }
        }

        // if no path was found, break;
        if (parent[sink] == -1) {
            break;
        }

        // find the bottleneck value 
        ll bottleneck = INF;
        for (int node = sink; parent[node] != -1; node = parent[node]) {
            int u = parent[node], v = node;
            ll rem_capacity = capacity[u][v] - flow[u][v];
            bottleneck = min(bottleneck, rem_capacity);
        }

        // augment edges along the path
        for (int node = sink; parent[node] != -1; node = parent[node]) {
            int u = parent[node], v = node;
            flow[u][v] += bottleneck;
            flow[v][u] -= bottleneck;
        }

        max_flow += bottleneck;
    }


    // print flow
    printf("%lld\n", max_flow);
}

void bfs(vvi adj, vector<bool>& vis, vvll &capacity, vvll &flow, int src, int N, bool rev_graph = false) {
    if (rev_graph) {
        // reverse the graph
        vvi new_adj(N);
        for (int u = 0; u < N; u++) {
            for (int v : adj[u]) {
                new_adj[v].push_back(u);
            }
        }
        adj = new_adj;
    }
    vis[src] = true;
    queue<int> q;
    q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            int rem_capacity = capacity[u][v] - flow[u][v];
            if (rev_graph) {
                rem_capacity = capacity[v][u] - flow[v][u];
            }
            if (!vis[v] && rem_capacity > 0) {
                vis[v] = true;
                q.push(v);
            }
        }
    }
}

int main(void) {
    int N, M;
    cin >> N >> M;

    vvi adj(N);
    vvll flow(N, vll(N, 0));
    vvll capacity(N, vll(N, 0));
    vvi edges;

    for (int i = 0; i < M; i++) {
        int u, v;
        ll cap;
        cin >> u >> v >> cap;
        --u, --v; // 0-based 
        adj[u].push_back(v);
        capacity[u][v] = cap;

        edges.push_back({u, v});

        //add residual edge
        adj[v].push_back(u);
    }

    int source = 0;
    int sink = N - 1; // 0-based
    
    edmond_karp(adj, flow, capacity, source, sink, N);

    vector<bool> reachable_from_source(N, false);
    vector<bool> can_reach_sink(N, false);

    bfs(adj, reachable_from_source, capacity, flow, source, N, false);
    bfs(adj, can_reach_sink, capacity, flow, sink, N, true);

    int P;
    cin >> P;
    for (int i = 1; i <= P; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        --u, --v;

        if (reachable_from_source[u] && can_reach_sink[v]) {
            cout << i << endl;
        }
    }
    
    return 0;
}
