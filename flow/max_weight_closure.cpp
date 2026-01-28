/*
The Problem: You are a manager with a set of possible projects and a set of required machines.
Projects: Each project P_i yields a profit (positive value).
Machines: Each machine M_j has a cost to buy (negative value).
Dependencies: Project P_i requires specific machines (e.g., P_1 needs M_2 and M_5).
Task: Select a subset of projects and machines to maximize Net Profit (Total Profit - Total Cost). You cannot pick a project without buying its required machines.


* SOLUTION FOR: Profitable Projects (Max Weight Closure)
 * LOGIC:
 * 1. Create Source (S) and Sink (T).
 * 2. Calculate sum of all positive profits (TotalPotential).
 * 3. Build Graph:
 * - For each Project P: Add Edge S -> P with cap = Profit.
 * - For each Machine M: Add Edge M -> T with cap = Cost.
 * - If P needs M: Add Edge P -> M with cap = INFINITY.
 * 4. Result = TotalPotential - MaxFlow.
 
Note:
Maximum independent set = Total Nodes - Minimum Vertex Cover = the largest set of nodes (Independent Set) such that NO two nodes in the set are connected by an edge.
Minium Vertex Cover = Max Flow = the smallest number of nodes (Vertex Cover) needed to "touch" or "cover" every single edge in the graph.
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

void solveProjectSelection() {
    int num_projects, num_machines;
    cin >> num_projects >> num_machines;
    
    int S = 0;
    int T = num_projects + num_machines + 1;
    EdmondsKarp ek(T + 1);
    
    long long total_potential_profit = 0;

    // 1. Setup Projects (Nodes 1 to num_projects)
    for(int i=1; i<=num_projects; i++) {
        int profit, k;
        cin >> profit >> k; // Profit and count of required machines
        total_potential_profit += profit;
        
        ek.add_edge(S, i, profit); // Edge S -> Project
        
        while(k--) {
            int machine_id;
            cin >> machine_id;
            // Edge Project -> Machine (Infinite because we can't "cut" this dependency)
            // Machine IDs are shifted by num_projects
            ek.add_edge(i, num_projects + machine_id, INF);
        }
    }

    // 2. Setup Machines (Nodes num_projects+1 ...)
    for(int j=1; j<=num_machines; j++) {
        int cost;
        cin >> cost;
        // Edge Machine -> T (Capacity is the cost)
        ek.add_edge(num_projects + j, T, cost);
    }

    long long min_loss = ek.max_flow(S, T);
    cout << total_potential_profit - min_loss << endl;
}

