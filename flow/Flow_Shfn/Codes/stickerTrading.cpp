#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAXN = 305;
int capacity[MAXN][MAXN];
int originalCap[MAXN][MAXN];
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
        int pathFlow = INT_MAX;

        for (int v = t; v != s; v = parent[v])
        {
            int u = parent[v];
            pathFlow = min(pathFlow, capacity[u][v]);
        }

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
    int n, m; // n friends, m different stickers
    cin >> n >> m;

    int myCount;
    cin >> myCount;

    map<int, int> myStickers;
    for (int i = 0; i < myCount; i++)
    {
        int sticker;
        cin >> sticker;
        myStickers[sticker]++;
    }

    vector<map<int, int>> friendStickers(n);
    for (int i = 0; i < n; i++)
    {
        int count;
        cin >> count;
        for (int j = 0; j < count; j++)
        {
            int sticker;
            cin >> sticker;
            friendStickers[i][sticker]++;
        }
    }

    // Reset
    memset(capacity, 0, sizeof(capacity));
    memset(originalCap, 0, sizeof(originalCap));
    for (int i = 0; i < MAXN; i++)
    {
        adj[i].clear();
    }

    int src = 0;              // You
    int sink = 1 + m + n + 1; // Dummy sink
    int total = sink + 1;

    // Sticker nodes: 1 to m
    // Friend nodes: m+1 to m+n

    // Edge from source to sticker with capacity = number of that sticker you have
    for (auto &p : myStickers)
    {
        int sticker = p.first;
        int count = p.second;

        int stickerNode = sticker;

        if (capacity[src][stickerNode] == 0)
        {
            adj[src].push_back(stickerNode);
            adj[stickerNode].push_back(src);
        }
        capacity[src][stickerNode] = count;
    }

    // Edge from sticker to friend if friend doesn't have that sticker
    for (int sticker = 1; sticker <= m; sticker++)
    {
        for (int friend_idx = 0; friend_idx < n; friend_idx++)
        {
            if (friendStickers[friend_idx].find(sticker) == friendStickers[friend_idx].end())
            {
                int stickerNode = sticker;
                int friendNode = m + friend_idx + 1;

                if (capacity[stickerNode][friendNode] == 0)
                {
                    adj[stickerNode].push_back(friendNode);
                    adj[friendNode].push_back(stickerNode);
                }
                capacity[stickerNode][friendNode] = 1;
            }
        }
    }

    // Edge from friend to sticker if friend has duplicates (count - 1)
    for (int friend_idx = 0; friend_idx < n; friend_idx++)
    {
        for (auto &p : friendStickers[friend_idx])
        {
            int sticker = p.first;
            int count = p.second;

            if (count > 1)
            { // Has duplicates
                int friendNode = m + friend_idx + 1;
                int stickerNode = sticker;

                if (capacity[friendNode][stickerNode] == 0)
                {
                    adj[friendNode].push_back(stickerNode);
                    adj[stickerNode].push_back(friendNode);
                }
                capacity[friendNode][stickerNode] = count - 1;
            }
        }
    }

    // Edge from each sticker to sink with capacity 1
    for (int sticker = 1; sticker <= m; sticker++)
    {
        int stickerNode = sticker;

        adj[stickerNode].push_back(sink);
        adj[sink].push_back(stickerNode);
        capacity[stickerNode][sink] = 1;
    }

    // Store original capacities for extraction
    memcpy(originalCap, capacity, sizeof(capacity));

    int maxFlow = edmondsKarp(src, sink, total);

    cout << maxFlow << endl;

    // Extract exchanges
    // Flow from friend to sticker means you gave that sticker to friend and got it back
    bool hasExchange = false;
    for (int friend_idx = 0; friend_idx < n; friend_idx++)
    {
        int friendNode = m + friend_idx + 1;

        for (auto &p : friendStickers[friend_idx])
        {
            int sticker = p.first;
            int stickerNode = sticker;

            // Check if there's flow from friend to sticker
            if (originalCap[friendNode][stickerNode] > 0)
            {
                int flow = originalCap[friendNode][stickerNode] - capacity[friendNode][stickerNode];

                for (int f = 0; f < flow; f++)
                {
                    // Find which sticker you gave to this friend
                    // Check reverse: which sticker flowed to this friend
                    for (int st = 1; st <= m; st++)
                    {
                        int stNode = st;
                        if (originalCap[stNode][friendNode] > 0 && capacity[stNode][friendNode] < originalCap[stNode][friendNode])
                        {
                            cout << "Exchange a sticker with number " << st
                                 << " against a sticker with number " << sticker
                                 << " of friend " << (friend_idx + 1) << endl;
                            hasExchange = true;
                            capacity[stNode][friendNode]++; // Mark as printed
                            break;
                        }
                    }
                }
            }
        }
    }

    if (!hasExchange)
    {
        cout << "No exchange is possible" << endl;
    }

    return 0;
}
