/*Problem:  Find the maximum number of people that can reach 
the Safe Room (Node N) from the Start (Node 1) within exactly T minutes

 * SOLUTION FOR: Time-Critical Evacuation
 * LOGIC:
 * 1. Create (N * (T + 1)) nodes + 1 Super Sink.
 * 2. Mapping: Node u at time t is index: (t * N + u).
 * 3. Add edges u_t -> v_{t+1} for all physical roads.
 * 4. Add edges u_t -> u_{t+1} (infinite cap) for waiting in a room.
 * 5. Connect ExitNode_t -> SuperSink (infinite cap) for all t.
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


void solveTimeExpandedEvacuation() {
    int N, M, T; 
    cin >> N >> M >> T;
    
    vector<tuple<int, int, int>> roads;
    for(int i=0; i<M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        roads.emplace_back(u, v, c);
    }
    
    // Nodes 0 to N-1 are layer 0. Nodes N to 2N-1 are layer 1...
    int total_nodes = N * (T + 1) + 2; 
    int super_source = 0; // Assume Node 0 is start
    int super_sink = total_nodes - 1;
    int exit_node = N - 1; // Assume Node N-1 is exit
    
    EdmondsKarp ek(total_nodes);

    for (int t = 0; t < T; t++) {
        // 1. Movement: u_t -> v_{t+1}
        for (auto [u, v, c] : roads) {
            int u_curr = t * N + u;
            int v_next = (t + 1) * N + v;
            ek.add_edge(u_curr, v_next, c);
        }

        // 2. Waiting: u_t -> u_{t+1}
        for (int i = 0; i < N; i++) {
            int u_curr = t * N + i;
            int u_next = (t + 1) * N + i;
            ek.add_edge(u_curr, u_next, INF);
        }
    }
    
    // 3. Connect Exit at all times to Super Sink
    for (int t = 1; t <= T; t++) {
        int exit_at_t = t * N + exit_node;
        ek.add_edge(exit_at_t, super_sink, INF);
    }
    
    cout << ek.max_flow(super_source, super_sink) << endl;
}