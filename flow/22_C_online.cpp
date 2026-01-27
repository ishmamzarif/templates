#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using vi = vector<int>;
using vvi = vector<vi>;
using vll = vector<ll>;
using vvll = vector<vll>;
const ll INF = 1e18;

struct Person {
    int exp, age, record, origin, weapon, trust, lang;
};

// check if a gang member and partner can form a partnership
bool canPartner(const Person &a, const Person &b) {
    if (abs(a.exp - b.exp) > 12) return false;        // experience gap
    if (abs(a.age - b.age) > 5) return false;         // age compatibility
    if (a.record != b.record) return false;           // criminal record matching
    if (a.origin != b.origin) return false;           // territory familiarity
    if (a.weapon + b.weapon == 0) return false;      // at least one weapon proficient
    if (a.trust + b.trust < 10) return false;        // trust level
    if ((a.lang & b.lang) == 0) return false;        // language compatibility
    return true;
}

ll edmond_karp(vvi &adj, vvll &flow, vvll &capacity, int source, int sink, int N) {
    ll max_flow = 0;

    while (true) {
        vi parent(N, -1);
        queue<int> q;
        q.push(source);
        parent[source] = -2;
        vector<bool> vis(N, false);
        vis[source] = true;

        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (!vis[v] && capacity[u][v] - flow[u][v] > 0) {
                    parent[v] = u;
                    q.push(v);
                    vis[v] = true;
                    if (v == sink) break;
                }
            }
        }

        if (parent[sink] == -1) break; // no more augmenting path

        ll bottleneck = INF;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            bottleneck = min(bottleneck, capacity[u][v] - flow[u][v]);
        }

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            flow[u][v] += bottleneck;
            flow[v][u] -= bottleneck;
        }

        max_flow += bottleneck;
    }

    return max_flow;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;

    for (int caseNo = 1; caseNo <= T; caseNo++) {
        int m, n;
        cin >> m >> n;

        vector<Person> members(m), partners(n);

        for (int i = 0; i < m; i++)
            cin >> members[i].exp >> members[i].age >> members[i].record >> members[i].origin 
                >> members[i].weapon >> members[i].trust >> members[i].lang;

        for (int i = 0; i < n; i++)
            cin >> partners[i].exp >> partners[i].age >> partners[i].record >> partners[i].origin 
                >> partners[i].weapon >> partners[i].trust >> partners[i].lang;

        int N = m + n + 2;
        int source = 0, sink = N - 1;

        vvi adj(N);
        vvll capacity(N, vll(N, 0));
        vvll flow(N, vll(N, 0));

        // connect source to gang members
        for (int i = 0; i < m; i++) {
            int u = i + 1;
            adj[source].push_back(u);
            adj[u].push_back(source);
            capacity[source][u] = 1;
        }

        // connect partners to sink
        for (int i = 0; i < n; i++) {
            int v = m + 1 + i;
            adj[v].push_back(sink);
            adj[sink].push_back(v);
            capacity[v][sink] = 1;
        }

        // connect members to partners if compatible
        for (int i = 0; i < m; i++) {
            int u = i + 1;
            for (int j = 0; j < n; j++) {
                int v = m + 1 + j;
                if (canPartner(members[i], partners[j])) {
                    adj[u].push_back(v);
                    adj[v].push_back(u);
                    capacity[u][v] = 1;
                }
            }
        }

        ll maxPartnerships = edmond_karp(adj, flow, capacity, source, sink, N);

        cout << "Case " << caseNo << ": " << maxPartnerships << "\n";
    }

    return 0;
}
