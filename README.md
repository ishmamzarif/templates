## templates
topic-wise templates for cse208 algorithms

## use the following template before adding problems
// problem statement  
<br>
// problem link (if any)    
<br>
// solution  
<br>

example:
```cpp
/* 
ishmam
*/

/* 
Problem:
Find the second shortest path between two nodes in a graph

Solution:
Use dijsktra but with two distance arrays 
*/

#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = 1e18;
using pll = pair<ll, ll>;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m, src;
    cin >> n >> m >> src; // n = nodes, m = edges, src = source node (0-indexed)
    
    vector<vector<pll>> adj(n); // adjacency list: {neighbor, weight}
    for (int i = 0; i < m; ++i) {
        int u, v;
        ll w;
        cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w}); // remove if directed
    }

    vector<ll> dist1(n, INF);
    vector<ll> dist2(n, INF);
    dist1[src] = 0;

    priority_queue<pll, vector<pll>, greater<pll>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        pll p = pq.top(); pq.pop();
        ll d = p.first;
        ll u = p.second;

        for (pll& edge : adj[u]) {
            ll v = edge.first;
            ll w = edge.second;

            ll nd = d + w;

            if (nd < dist1[v]) {
                dist2[v] = dist1[v];
                dist1[v] = nd;
                pq.push({dist1[v], v});
                if (dist2[v] != INF) {
                    pq.push({dist2[v], v});
                }
            } 
            else if (nd > dist1[v] && nd < dist2[v]) {
                dist2[v] = nd;
                pq.push({dist2[v], v});
            }
        }
    }

    cout << "Node : Shortest Dist : Second Shortest Dist\n";
    for (int i = 0; i < n; ++i) {
        cout << i << " : ";
        if (dist1[i] < INF) cout << dist1[i]; else cout << "INF";
        cout << " : ";
        if (dist2[i] < INF) cout << dist2[i]; else cout << "INF";
        cout << "\n";
    }

    return 0;
}

```

