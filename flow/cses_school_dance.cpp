/* 
There are n boys and m girls in a school. 
Next week a school dance will be organized. 
A dance pair consists of a boy and a girl, and there are k potential pairs.
Your task is to find out the maximum number of dance pairs 
    and show how this number can be achieved.

The boys are numbered 1,2,...,n, and the girls are numbered 1,2,...,m.

maximum bipartite matching
extra edges will be N + M edges
0..N-1 boys
N..M-1 girls
N + M source -> connect to all boys
M + M + 1 sink -> connect to all girls
run max flow
*/

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


int main(void) {
    int N, M, K;
    cin >> N >> M >> K;

    int total = N + M + 2;

    vvi adj(total);
    vvll flow(total, vll(total, 0));
    vvll capacity(total, vll(total, 0));
    set<pll> edges;

    for (int i = 0; i < K; i++) {
        int u, v;
        cin >> u >> v;
        --u, --v; // converting to 0-based
        v = v + N; // girls start from N .. N + M
        if (capacity[u][v] != 1) {
            adj[u].push_back(v);
            capacity[u][v] = 1;
        }

        edges.insert({u, v});

        //add residual edge
        adj[v].push_back(u);
    }

    int source = total - 2;
    int sink = total - 1;

    for (int i = 0; i < N; i++) {
        adj[source].push_back(i);
        capacity[source][i] = 1;

        // add residual edge
        adj[i].push_back(source);
    }
    
    for (int i = N; i < N + M; i++) {
        adj[i].push_back(sink);
        capacity[i][sink] = 1;
    
        // add residual edge
        adj[sink].push_back(i);

    }
    
    edmond_karp(adj, flow, capacity, source, sink, total);

    for (pll edge : edges) {
        int u = edge.first, v = edge.second;
        if (flow[u][v] > 0) {
            printf("%d %d\n", u + 1, v - N + 1);
        }
    }
    
    return 0;
}