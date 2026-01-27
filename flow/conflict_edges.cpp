/*
PROBLEM 3: Bipartite Matching with Forbidden Pairs

or maximum vertex cover problem

If edge e1 is chosen, edge e2 cannot be chosen.

MODELING:
Each edge becomes a node.
Conflict edges => capacity 1 cut.
This reduces to max independent set in bipartite conflict graph.
*/

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
    // no. of edges and no. of conflict edges
    int E, F;
    cin >> E >> F;

    int S = 0, T = E + 1;
    int N = T + 1;

    vector<vector<int>> adj(N);
    vector<vector<ll>> cap(N, vector<ll>(N, 0));
    vector<vector<ll>> flow(N, vector<ll>(N, 0));

    for (int i = 1; i <= E; i++) {
        adj[S].push_back(i);
        adj[i].push_back(S);
        cap[S][i] = 1;

        adj[i].push_back(T);
        adj[T].push_back(i);
        cap[i][T] = 1;
    }

    while (F--) {
        int a, b;
        cin >> a >> b;
        cap[a][b] = cap[b][a] = 1;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    // max flow = minimum vertex cover
    // answer = E - maxflow

    // standard EK
    ll maxflow = 0;
    while (true) {
        vector<int> par(N, -1);
        queue<int> q;
        q.push(S);
        par[S] = S;

        while (!q.empty() && par[T] == -1) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (par[v] == -1 && cap[u][v] - flow[u][v] > 0) {
                    par[v] = u;
                    q.push(v);
                }
            }
        }

        if (par[T] == -1) break;

        ll f = 1;
        for (int v = T; v != S; v = par[v]) {
            flow[par[v]][v] += f;
            flow[v][par[v]] -= f;
        }
        maxflow += f;
    }

    cout << E - maxflow << "\n";
}
