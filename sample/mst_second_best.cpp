/*
Find second best MST for a graph (Binary Lifting)
*/


#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> p, sz;
    DSU(int n) : p(n+1), sz(n+1,1) { iota(p.begin(), p.end(), 0); }
    int find(int x){ return p[x]==x?x:p[x]=find(p[x]); }
    bool unite(int a, int b){
        a=find(a); b=find(b);
        if(a==b) return false;
        if(sz[a] < sz[b]) swap(a,b);
        p[b]=a; sz[a]+=sz[b];
        return true;
    }
};

static const int LOG = 20;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    struct Edge { int u, v; long long w; };
    vector<Edge> edges(m);

    for(auto &e : edges)
        cin >> e.u >> e.v >> e.w;

    // 1. Build MST using Kruskal
    sort(edges.begin(), edges.end(),
         [](auto &a, auto &b){ return a.w < b.w; });

    DSU dsu(n);
    long long mstCost = 0;
    vector<int> inMST(m, 0);

    vector<vector<pair<int,long long>>> adj(n+1);

    for(int i = 0; i < m; i++){
        auto &e = edges[i];
        if(dsu.unite(e.u, e.v)){
            inMST[i] = 1;
            mstCost += e.w;

            adj[e.u].push_back({e.v, e.w});
            adj[e.v].push_back({e.u, e.w});
        }
    }

    // 2. Prepare LCA (binary lifting)
    vector<vector<int>> parent(LOG, vector<int>(n+1));
    vector<vector<long long>> maxEdge(LOG, vector<long long>(n+1));
    vector<int> depth(n+1, 0);

    function<void(int,int)> dfs = [&](int v, int p){
        for(auto &nx : adj[v]){
            int u = nx.first;
            long long w = nx.second;
            if(u == p) continue;

            depth[u] = depth[v] + 1;
            parent[0][u] = v;
            maxEdge[0][u] = w;
            dfs(u, v);
        }
    };

    dfs(1, -1);

    // binary lifting dp
    for(int k=1; k<LOG; k++){
        for(int v=1; v<=n; v++){
            parent[k][v] = parent[k-1][ parent[k-1][v] ];
            maxEdge[k][v] = max(maxEdge[k-1][v], maxEdge[k-1][ parent[k-1][v] ]);
        }
    }

    // Function to get max edge on path u-v
    auto queryMax = [&](int u, int v){
        long long mx = 0;

        if(depth[u] < depth[v]) swap(u,v);

        int diff = depth[u] - depth[v];
        for(int k=0; k<LOG; k++)
            if(diff & (1 << k)){
                mx = max(mx, maxEdge[k][u]);
                u = parent[k][u];
            }

        if(u == v) return mx;

        for(int k=LOG-1; k>=0; k--){
            if(parent[k][u] != parent[k][v]){
                mx = max({mx, maxEdge[k][u], maxEdge[k][v]});
                u = parent[k][u];
                v = parent[k][v];
            }
        }

        mx = max({mx, maxEdge[0][u], maxEdge[0][v]});
        return mx;
    };

    // 3. Evaluate all edges not in MST
    long long secondBest = LLONG_MAX;

    for(int i=0; i<m; i++){
        if(inMST[i]) continue;

        auto &e = edges[i];
        long long mx = queryMax(e.u, e.v);

        if(mx != e.w){
            long long cost = mstCost + e.w - mx;
            if(cost > mstCost)
                secondBest = min(secondBest, cost);
        }
    }

    cout << secondBest << "\n";
    return 0;
}
