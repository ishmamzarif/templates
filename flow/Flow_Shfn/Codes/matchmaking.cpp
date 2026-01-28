#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <cmath>
using namespace std;
/*
3. Match Extraction (The unique part!)
After max flow completes, the program reconstructs
which specific pairs were matched by examining the residual graph:

if (matchEdges[i][j] && capacity[womanNode][i] > 0)

This checks:

matchEdges[i][j]: Was there originally an edge from man i to woman j?
capacity[womanNode][i] > 0: Is there reverse flow (from woman back to man)?
Why check reverse capacity?

When flow goes from man → woman in the original graph, the algorithm:
Decreases forward capacity (man → woman) to 0
Increases reverse capacity (woman → man) to 1
So reverse capacity > 0 means flow went through that edge, indicating a match!

*/
const int MAXN = 205;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];

struct Person
{
    int height;
    int age;
    int id;
};

bool canMatch(Person &man, Person &woman)
{
    // Height difference <= 10 inches
    if (abs(man.height - woman.height) > 10)
        return false;

    // Age difference <= 5 years
    if (abs(man.age - woman.age) > 5)
        return false;

    return true;
}

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
    int m, n; // m men, n women
    cin >> m >> n;

    vector<Person> men(m);
    for (int i = 0; i < m; i++)
    {
        cin >> men[i].height >> men[i].age >> men[i].id;
    }

    vector<Person> women(n);
    for (int i = 0; i < n; i++)
    {
        cin >> women[i].height >> women[i].age >> women[i].id;
    }

    // Reset
    memset(capacity, 0, sizeof(capacity));
    for (int i = 0; i < MAXN; i++)
    {
        adj[i].clear();
    }

    int src = 0;
    int sink = m + n + 1;
    int total = sink + 1;

    // Men are nodes 1 to m
    // Women are nodes m+1 to m+n

    // Connect source to all men
    for (int i = 1; i <= m; i++)
    {
        adj[src].push_back(i);
        adj[i].push_back(src);
        capacity[src][i] = 1;
    }

    // Connect men to compatible women
    vector<vector<int>> matchEdges(m + 1, vector<int>(n + 1, 0));
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (canMatch(men[i], women[j]))
            {
                int manNode = i + 1;
                int womanNode = m + j + 1;

                if (capacity[manNode][womanNode] == 0) // dewa mandatory na ig,
                {
                    adj[manNode].push_back(womanNode);
                    adj[womanNode].push_back(manNode);
                }
                capacity[manNode][womanNode] = 1;
                matchEdges[i + 1][j + 1] = 1;
            }
        }
    }

    // Connect all women to sink
    for (int i = 1; i <= n; i++)
    {
        int womanNode = m + i;
        adj[womanNode].push_back(sink);
        adj[sink].push_back(womanNode);
        capacity[womanNode][sink] = 1;
    }

    // Store original capacity for matching extraction
    int originalCap[MAXN][MAXN];
    memcpy(originalCap, capacity, sizeof(capacity));

    int maxFlow = edmondsKarp(src, sink, total);

    cout << maxFlow << endl;

    // Extract matches
    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            // Check if this edge was used in the matching
            int womanNode = m + j;
            if (matchEdges[i][j] && capacity[womanNode][i] > 0)
            {
                // Man i matched with woman j
                cout << "Man " << men[i - 1].id << " - Woman " << women[j - 1].id << endl;
            }
        }
    }

    return 0;
}