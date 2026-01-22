/* 
https://cses.fi/problemset/task/1694
*/

/* 
direct max flow easy

there might be multiple paralles edges
just increment the capacity
but dont add duplicate to the adj_list -> check before with find
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
        while (!q.empty() && !vis[sink]) {
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
    cout << max_flow << endl;
}


int main(void) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;

    vvi adj(N);
    vvll flow(N, vll(N, 0));
    vvll capacity(N, vll(N, 0));

    for (int i = 0; i < M; i++) {
        int u, v;
        ll cap;
        cin >> u >> v >> cap;
        --u, --v; // converting to 0-based
        
        // in case of multiple edges
        if (find(begin(adj[u]), end(adj[u]), v) == adj[u].end())
            adj[u].push_back(v);

        if (find(begin(adj[v]), end(adj[v]), u) == adj[v].end())
            adj[v].push_back(u);
        

        // in case of multiple edges
        capacity[u][v] += cap;
    }

    // 0-based source and sink
    int source = 0;
    int sink = N - 1;
    
    edmond_karp(adj, flow, capacity, source, sink, N);
    
    return 0;
}

