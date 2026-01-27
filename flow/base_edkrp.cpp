/*
this is just the base edm-krp ---tnm
*/

#include<bits/stdc++.h>
using namespace std;
using ll = long long int;

int bfs(int s, int t, vector<int>&parent, vector<vector<int>> &adj, vector<vector<int>>&residual){
    for(int i=0; i<parent.size(); ++i){
        parent[i] = -1;
    }
    parent[s] = -2;
    queue<pair<int,int>> q;
    q.push({s, INT_MAX});

    while(!q.empty()){
        int curr = q.front().first;
        int flow = q.front().second;
        q.pop();

        for(int next : adj[curr]){
            if(parent[next] == -1 && residual[curr][next] > 0){
                parent[next] = curr;

                int new_flow = min(flow, residual[curr][next]);

                if(next == t){
                    return new_flow;
                }

                q.push({next, new_flow});
            }
        }
    }
    return 0;
}

ll maxflow(int s, int t, int n, vector<vector<int>> &residual, vector<vector<int>>& adj){

    ll flow = 0;
    vector<int> parent(n);
    int new_flow = 0;

    while((new_flow = bfs(s, t, parent, adj, residual)) > 0){
        flow = flow + new_flow;

        int curr = t;

        while(curr != s){
            int prev = parent[curr];

            residual[prev][curr] = residual[prev][curr] - new_flow;
            residual[curr][prev] = residual[curr][prev] + new_flow;

            curr = prev;
        }
    }
    return flow;
}

int main(){
    int n, m;
    cin >> n >> m;

    vector<vector<int>> residual(n, vector<int>(n,0));
    vector<vector<int>> adj(n);

    vector<pair<pair<int,int>,int>> edge(m);

    for(int i=0; i<m; ++i){
        int u, v, c;
        cin >> u >> v >> c;

        edge[i] = {{u,v},c};

        residual[u][v] = c;

        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    int s, t;
    cin >> s >> t;

    cout << maxflow(s, t, n, residual, adj) << endl;

    for(int i=0; i<m; ++i){
        int u, v,c;
        u = edge[i].first.first;
        v = edge[i].first.second;

        c= edge[i].second;

        cout << u << " " << v << " " << c - residual[u][v] << "/" << c << endl;
    }

    return 0;
}