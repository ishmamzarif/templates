/* 
https://cses.fi/problemset/task/1695/
*/

/*
cses police chase problem

simple find the min-cut problem
run max flow
then run a dfs for some edge u-v
if u is reachable from sink but v is not
then u-v is in the min-cut

n.b:
for an undirected graph,
push both u-v and v-u to capacity matrix
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
        ll bottleneck = 1;

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

// dfs to find reachable vertices in residual graph
void dfs(int u, vvi& adj, vvll& flow, vvll& capacity, vector<bool>& vis) {
    vis[u] = true;
    for (int v : adj[u]) {
        int rem_capacity = capacity[u][v] - flow[u][v];
        if (!vis[v] && rem_capacity > 0) {
            dfs(v, adj, flow, capacity, vis);
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
        cin >> u >> v;
        --u, --v;
        adj[u].push_back(v);
        adj[v].push_back(u);
        capacity[u][v] = 1;
        capacity[v][u] = 1;

        edges.push_back({u, v});
        edges.push_back({v, u});
    }

    int source = 0;
    int sink = N - 1;
    
    edmond_karp(adj, flow, capacity, source, sink, N);

    // find min-cut
    vector<bool> reachable(N, false);
    dfs(source, adj, flow, capacity, reachable);

    for (vi& edge : edges) {
        int u = edge[0], v = edge[1];
        if (reachable[u] && !reachable[v]) {
            printf("%d %d\n", u + 1, v + 1);
        }
    }
    
    return 0;
}