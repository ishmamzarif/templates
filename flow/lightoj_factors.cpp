#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = 1e18;

using vi = vector<int>;
using vvi = vector<vi>;
using vll = vector<ll>;
using vvll = vector<vll>;

void add_edge(int u, int v, ll cap,
              vvi &adj, vvll &capacity) {
    adj[u].push_back(v);
    adj[v].push_back(u);
    capacity[u][v] = cap;
}

ll edmond_karp(vvi& adj, vvll& capacity, int source, int sink, int N) {
    vvll flow(N, vll(N, 0));
    ll max_flow = 0;

    while (true) {
        vector<int> parent(N, -1);
        parent[source] = source;
        queue<int> q;
        q.push(source);

        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (parent[v] == -1 && capacity[u][v] - flow[u][v] > 0) {
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        if (parent[sink] == -1) break;

        ll aug = INF;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            aug = min(aug, capacity[u][v] - flow[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            flow[u][v] += aug;
            flow[v][u] -= aug;
        }

        max_flow += aug;
    }

    return max_flow;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;

    while (T--) {
        int n, m;
        cin >> n;
        vector<ll> A(n);
        for (int i = 0; i < n; i++) cin >> A[i];

        cin >> m;
        vector<ll> B(m);
        for (int i = 0; i < m; i++) cin >> B[i];

        int S = n + m;
        int T = n + m + 1;
        int N = n + m + 2;

        vvi adj(N);
        vvll capacity(N, vll(N, 0));

        // Source -> A nodes
        for (int i = 0; i < n; i++) {
            add_edge(S, i, 1, adj, capacity);
        }

        // B nodes -> Sink
        for (int j = 0; j < m; j++) {
            add_edge(n + j, T, 1, adj, capacity);
        }

        // Conflict edges
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (B[j] % A[i] == 0) {
                    add_edge(i, n + j, INF, adj, capacity);
                }
            }
        }

        ll answer = edmond_karp(adj, capacity, S, T, N);
        cout << answer << "\n";
    }

    return 0;
}
