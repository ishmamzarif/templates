/* 
 https://cses.fi/problemset/task/1711
*/

/*
 A game consists of n rooms and m teleporters. 
 At the beginning of each day, you start in room 1 and you have to reach room n.
 You can use each teleporter at most once during the game.
 How many days can you play if you choose your routes optimally?
*/

/* 
 use max-flow
 once you find an augmenting path, DO NOT PRINT IT INSIDE AUGMENTING PATH
 because augmenting paths may contain residual edges -> they ARE NOT paths from S->T
 
 finish max-flow
 after that, run dfs/bfs and print paths and flow[node]-- along that path
 repeat until no more paths can be found
*/

#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = 1e18;
using vi = vector<int>;
using vvi = vector<vi>;
using vll = vector<ll>;
using vvll = vector<vll>;

ll edmond_karp(vector<set<int>>& adj, vvll& flow, vvll& capacity, int source, int sink, int N) {
    ll max_flow = 0;
    
    while (true) {
        vi parent(N, -1);
        queue<int> q;
        q.push(source);
        vector<bool> vis(N, false);
        vis[source] = true;

        // BFS
        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (auto v : adj[u]) {
                ll rem_capacity = capacity[u][v] - flow[u][v];
                if (!vis[v] && rem_capacity > 0) {
                    q.push(v);
                    vis[v] = true;
                    parent[v] = u;
                    if (v == sink) {
                        break;
                    }
                }
            }
        }

        if (parent[sink] == -1) {
            break;
        }

        ll bottleneck = 1;

        for (int node = sink; parent[node] != -1; node = parent[node]) {
            int u = parent[node], v = node;
            flow[u][v] += bottleneck;
            flow[v][u] -= bottleneck;
        }

        max_flow += bottleneck;
    }

    return max_flow;
}

int main(void) {
    int N, M;
    cin >> N >> M;

    vector<set<int>> adj(N);
    vvll flow(N, vll(N, 0));
    vvll capacity(N, vll(N, 0));

    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v;
        --u, --v;
        adj[u].insert(v);
        capacity[u][v] += 1;  

        // add residual edge
        adj[v].insert(u);
    }

    int source = 0;
    int sink = N - 1;
    
    ll total_paths = edmond_karp(adj, flow, capacity, source, sink, N);

    cout << total_paths << endl;

    for (int i = 0; i < total_paths; i++) {
        vi path;
        int curr = source;
        path.push_back(curr);

        while (curr != sink) {
            for (auto next : adj[curr]) {
                // Only follow edges that have flow and exist in original graph
                if (capacity[curr][next] > 0 && flow[curr][next] > 0) {
                    flow[curr][next]--; // Consume flow
                    curr = next;
                    path.push_back(curr);
                    break;
                }
            }
        }

        cout << path.size() << "\n";
        for (int j = 0; j < path.size(); j++) {
            cout << path[j] + 1 << (j == (int)path.size() - 1 ? "" : " ");
        }
        cout << "\n";
    }

    return 0;
}
