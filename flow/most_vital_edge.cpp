/*
The Problem: You are a general studying an enemy supply network (Source to Sink). 
You have enough explosives to destroy exactly one road.
Task: Identify the edge that, if removed, causes the largest reduction in the Max Flow.


* SOLUTION FOR: Sabotage (Vital Edge)
 * LOGIC:
 * 1. Run initial Max Flow. 
 * 2. Collect all edges that are "saturated" (flow == capacity).
 * (Optimization: strictly speaking, we only need to test edges in the Min-Cut,
 * but testing all saturated edges is a safe, easier heuristic for small M).
 * 3. Loop through candidates:
 * - Temporarily set cap = 0.
 * - Run Max Flow from scratch (or un-saturate and adjust).
 * - Record flow reduction.
 * - Restore cap.
Choose the edge that gives the minimum result
*/

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <tuple>

using namespace std;

const long long INF = 1e18;

// Structure to represent an edge in the flow network
struct Edge {
    int v;              // Destination vertex
    long long capacity; // Total capacity
    long long flow;     // Current flow
    int rev;            // Index of the reverse edge in adj[v]
};

class EdmondsKarp {
public:
    int n;
    vector<vector<Edge>> adj;

    EdmondsKarp(int nodes) : n(nodes), adj(nodes) {}

    // Function to add a directed edge with a capacity
    void add_edge(int u, int v, long long cap) {
        Edge a = {v, cap, 0, (int)adj[v].size()};
        Edge b = {u, 0, 0, (int)adj[u].size()}; // Backward edge with 0 capacity
        adj[u].push_back(a);
        adj[v].push_back(b);
    }

    // BFS to find the shortest augmenting path in the residual graph
    // Returns the amount of flow that can be pushed (0 if no path)
    long long bfs(int s, int t, vector<int>& parent_edge_index, vector<int>& parent_node) {
        fill(parent_node.begin(), parent_node.end(), -1);
        parent_node[s] = -2; // Mark source as visited
        
        queue<pair<int, long long>> q;
        q.push({s, INF}); // Push source with infinite flow potential

        while (!q.empty()) {
            int u = q.front().first;
            long long flow = q.front().second;
            q.pop();

            for (int i = 0; i < adj[u].size(); i++) {
                Edge &e = adj[u][i];
                // If the edge is not full and the destination is not visited
                if (parent_node[e.v] == -1 && e.capacity - e.flow > 0) {
                    parent_node[e.v] = u;
                    parent_edge_index[e.v] = i;
                    long long new_flow = min(flow, e.capacity - e.flow);
                    
                    if (e.v == t) return new_flow; // Reached sink
                    q.push({e.v, new_flow});
                }
            }
        }
        return 0;
    }

    long long max_flow(int s, int t) {
        long long total_flow = 0;
        long long new_flow;
        vector<int> parent_node(n);       // Stores the node we came from
        vector<int> parent_edge_index(n); // Stores the index of the edge in adj list

        // While there is a path from S to T with available capacity
        while (new_flow = bfs(s, t, parent_edge_index, parent_node)) {
            total_flow += new_flow;
            int cur = t;
            
            // Backtrack to update residual capacities
            while (cur != s) {
                int prev = parent_node[cur];
                int edge_idx = parent_edge_index[cur];
                
                // Add flow to forward edge
                adj[prev][edge_idx].flow += new_flow;
                
                // Subtract flow from reverse edge (add to residual)
                int reverse_edge_idx = adj[prev][edge_idx].rev;
                adj[cur][reverse_edge_idx].flow -= new_flow;
                
                cur = prev;
            }
        }
        return total_flow;
    }
};

void solveSabotage() {
    int N, M;
    cin >> N >> M;
    
    struct Road { int u, v, c, id; };
    vector<Road> roads;
    for(int i=0; i<M; i++){
        int u, v, c;
        cin >> u >> v >> c;
        roads.push_back({u, v, c, i});
    }

    // 1. Calculate Initial Max Flow
    EdmondsKarp initial_solver(N);
    for(const auto& r : roads) initial_solver.add_edge(r.u, r.v, r.c);
    long long initial_flow = initial_solver.max_flow(0, N-1);
    
    long long min_flow = initial_flow;
    int best_edge_id = -1;

    // 2. Try removing each edge one by one
    for(int i=0; i<M; i++) {
        EdmondsKarp temp_solver(N);
        for(int j=0; j<M; j++) {
            if(i == j) continue; // This is the "Sabotaged" edge
            temp_solver.add_edge(roads[j].u, roads[j].v, roads[j].c);
        }
        
        long long current_flow = temp_solver.max_flow(0, N-1);
        
        if(current_flow < min_flow) {
            min_flow = current_flow;
            best_edge_id = roads[i].id;
        }
    }

    if(best_edge_id != -1) cout << best_edge_id << " " << min_flow << endl;
    else cout << "None" << endl;
}
