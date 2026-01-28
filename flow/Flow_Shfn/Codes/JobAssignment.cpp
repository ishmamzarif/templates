#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <sstream>
using namespace std;

const int MAXN = 205;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];

bool bfs(int s, int t, int total)
{
    memset(parent, -1, sizeof(parent));
    queue<int> q;
    q.push(s);
    parent[s] = -2;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : adj[u])
        {
            if (v < total && parent[v] == -1 && capacity[u][v] > 0)
            {
                parent[v] = u;
                if (v == t)
                    return true;
                q.push(v);
            }
        }
    }
    return false;
}

int edmondsKarp(int s, int t, int total)
{
    int maxFlow = 0;

    while (bfs(s, t, total))
    {
        int pathFlow = 1;

        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            capacity[u][v] -= pathFlow;
            capacity[v][u] += pathFlow;
        }

        maxFlow += pathFlow;
    }
    return maxFlow;
}

int main()
{
    int N, M; // N applicants, M jobs
    cin >> N >> M;
    cin.ignore();

    // Reset
    memset(capacity, 0, sizeof(capacity));
    for (int i = 0; i < MAXN; i++)
    {
        adj[i].clear();
    }

    int src = 0;
    int sink = M + N + 1;
    int total = sink + 1;

    // Jobs are nodes 1 to M
    // Applicants are nodes M+1 to M+N

    // Connect source to all jobs
    for (int i = 1; i <= M; i++)
    {
        adj[src].push_back(i);
        adj[i].push_back(src);
        capacity[src][i] = 1;
    }

    // Read job applications
    for (int i = 0; i < M; i++)
    {
        string line;
        getline(cin, line);
        stringstream ss(line); // input newa tai challenge here lol.
                               //  1 line er first ta holo jobId and porer gula applicant id;

        int jobId;
        ss >> jobId;

        int applicantId;
        while (ss >> applicantId)
        {
            int jobNode = jobId;
            int applicantNode = M + applicantId;

            if (capacity[jobNode][applicantNode] == 0)
            {
                adj[jobNode].push_back(applicantNode);
                adj[applicantNode].push_back(jobNode);
            }
            capacity[jobNode][applicantNode] = 1;
        }
    }

    // Connect all applicants to sink
    for (int i = 1; i <= N; i++)
    {
        int applicantNode = M + i;
        adj[applicantNode].push_back(sink);
        adj[sink].push_back(applicantNode);
        capacity[applicantNode][sink] = 1;
    }

    int maxFlow = edmondsKarp(src, sink, total);

    cout << maxFlow << endl;

    return 0;
}
