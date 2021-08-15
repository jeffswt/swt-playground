
#include <iostream>  // C++ I/O
#include <fstream>  // File I/O
#include <sstream>  // String stream I/O
#include <iomanip>  // C++ I/O manipulator

#include <cstdlib>  // C library
#include <cstdio>  // C I/O
#include <ctime>  // C time
#include <cmath>  // Math library
#include <cstring>  // C strings

#include <vector>  // Vector
#include <queue>  // Queue
#include <stack>  // Stack
#include <map>  // Map
#include <set>  // Set
#include <algorithm>  // Algorithms

using namespace std;

#define memclr(_arr)  memset(_arr, 0, sizeof(_arr))
#define reps(_var,_begin,_end,_step)  for (int _var = (_begin);  \
    _var <= (_end); _var += (_step))
#define reps_(_var,_end,_begin,_step)  for (int _var = (_end);  \
    _var >= (_begin); _var -= (_step))
#define rep(_var,_begin,_end)  reps(_var, _begin, _end, 1)
#define rep_(_var,_end,_begin)  reps_(_var, _end, _begin, 1)
#define minimize(_var,_targ)  _var = min(_var, _targ)
#define maximize(_var,_targ)  _var = max(_var, _targ)

typedef unsigned long long ull;
typedef long long lli, ll;
typedef double llf;

const int maxn = 1010, maxm = 20010;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class Dijkstra
{
public:
    struct edge
    {
        int u, v;
        lli len;
        edge *next;
    };
    edge epool[maxm], *edges[maxn];
    int n, ecnt;
    lli dist[maxn];
    bool vis[maxn];
    typedef pair<lli, int> pli;
    void add_edge(int u, int v, lli len)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v; p->len = len;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    void eval(int s)
    {
        priority_queue<pli, vector<pli>, greater<pli> > pq;
        rep(i, 0, n) {
            dist[i] = infinit;
            vis[i] = false;
        }
        dist[s] = 0;
        pq.push(make_pair(dist[s], s));
        while (!pq.empty()) {
            pli pr = pq.top();
            pq.pop();
            int p = pr.second;
            if (vis[p])
                continue;
            vis[p] = true;
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (!vis[ep->v] && dist[p] + ep->len < dist[ep->v]) {
                    dist[ep->v] = dist[p] + ep->len;
                    pq.push(make_pair(dist[ep->v], ep->v));
                }
        }
        return ;
    }
    void init(int n)
    {
        this->n = n;
        ecnt = 0;
        memclr(edges);
        return ;
    }
} graph;

int main(int argc, char** argv)
{
    int n, m;
    while (true) {
        scanf("%d%d", &n, &m);
        if (n == 0 && m == 0)
            break;
        graph.init(n);
        rep(i, 1, m) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            graph.add_edge(a, b, c);
            graph.add_edge(b, a, c);
        }
        graph.eval(1);
        printf("%lld\n", graph.dist[n]);
    }
    return 0;
}