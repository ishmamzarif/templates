#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <cmath>
using namespace std;

/*

Source (0)
    ↓ (capacity = 1)
Gang Members (1 to m)
    ↓ (capacity = 1, if compatible)
Partners (m+1 to m+n)
    ↓ (capacity = 1)
Sink (m+n+1)
*/

const int MAXN = 205;
int capacity[MAXN][MAXN];
int parent[MAXN];
vector<int> adj[MAXN];

struct Person
{
    int experience;
    int age;
    int criminal;
    int origin;
    int weapon;
    int trust;
    int language;
};

bool canPartner(Person &a, Person &b)
{
    // 1. Experience gap <= 12
    if (abs(a.experience - b.experience) > 12)
        return false;

    // 2. Age gap <= 5
    if (abs(a.age - b.age) > 5)
        return false;

    // 3. Criminal record matching
    if (a.criminal != b.criminal)
        return false;

    // 4. Territory familiarity
    if (a.origin != b.origin)
        return false;

    // 5. Weapon proficiency - at least one
    if (a.weapon == 0 && b.weapon == 0)
        return false;

    // 6. Trust level sum >= 10
    if (a.trust + b.trust < 10)
        return false;

    // 7. Language compatibility - share at least one
    if ((a.language & b.language) == 0)
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
    int T;
    cin >> T;

    for (int tc = 1; tc <= T; tc++)
    {
        int m, n;
        cin >> m >> n;

        vector<Person> gangMembers(m);
        for (int i = 0; i < m; i++)
        {
            cin >> gangMembers[i].experience >> gangMembers[i].age >> gangMembers[i].criminal >> gangMembers[i].origin >> gangMembers[i].weapon >> gangMembers[i].trust >> gangMembers[i].language;
        }

        vector<Person> partners(n);
        for (int i = 0; i < n; i++)
        {
            cin >> partners[i].experience >> partners[i].age >> partners[i].criminal >> partners[i].origin >> partners[i].weapon >> partners[i].trust >> partners[i].language;
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

        // Connect source to gang members
        for (int i = 0; i < m; i++)
        {
            adj[src].push_back(i + 1);
            adj[i + 1].push_back(src);
            capacity[src][i + 1] = 1;
        }

        // Connect gang members to compatible partners
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (canPartner(gangMembers[i], partners[j]))
                {
                    int gangNode = i + 1;
                    int partnerNode = m + j + 1;
                    adj[gangNode].push_back(partnerNode);
                    adj[partnerNode].push_back(gangNode);
                    capacity[gangNode][partnerNode] = 1;
                }
            }
        }

        // Connect partners to sink
        for (int i = 0; i < n; i++)
        {
            int partnerNode = m + i + 1;
            adj[partnerNode].push_back(sink);
            adj[sink].push_back(partnerNode);
            capacity[partnerNode][sink] = 1;
        }

        int maxFlow = edmondsKarp(src, sink, total);

        cout << "Case " << tc << ": " << maxFlow << endl;
    }

    return 0;
}
