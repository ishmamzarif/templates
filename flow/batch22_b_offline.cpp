/*
    TL;DR: a lot of mouse in danger, they run towards holes to be safe
    Each mouse can only run a certain maximum distance r in any direction
    Each hole can only hide a limited number of mice — its capacity
    Not every mouse can reach every hole in time.--- calculate the number of safe mouse

    normal max-flow problem
    create the graph, run max-flow
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


struct mouse{
    int cx, cy;
};

struct hole{
    int cx, cy, cap;
};

int main(){

    int t;
    cin >> t;

    int i = 1;
    while(t--){
        int m,h;
        double r;
        cin >> m >> h >> r;

        vector<mouse> mice(m);
        vector<hole> holes(h);

        for(int i=0; i<m; ++i){
            int x, y;
            cin >> x >> y;

            mice[i].cx = x;
            mice[i].cy = y;
        }

        for(int i=0; i<h; ++i){
            int x,y,c;
            cin >> x >> y >> c;

            holes[i].cx = x;
            holes[i].cy = y;
            holes[i].cap = c;
        }

        // 0 + 1...m + (m+1 ... m+h) + m+h+1 
        int s = 0, t= m+h+1, n= m+h+2;

        vector<vector<int>> residual(n, vector<int>(n,0));
        vector<vector<int>> adj(n);

        for(int i=1; i<=m; ++i){
            residual[s][i] = 1;

            adj[s].push_back(i);
            adj[i].push_back(s);
        }

        for(int i=m+1; i<=m+h; ++i){
            residual[i][t] = holes[i-m-1].cap;

            adj[i].push_back(t);
            adj[t].push_back(i);
        }

        for(int i=0; i<m; ++i){
            for(int j=0; j<h; ++j){
                double temp = sqrt(pow(mice[i].cx - holes[j].cx, 2) + pow(mice[i].cy - holes[j].cy, 2));

                if(temp <= r){
                    residual[i+1][m+1+j] = 1;
                    adj[i+1].push_back(m+1+j);
                    adj[m+1+j].push_back(i+1);
                }
            }
        }

        cout << "Case: " << i << " " <<   maxflow(s, t, n, residual, adj) << endl;

        ++i;
    }

    return 0;
}