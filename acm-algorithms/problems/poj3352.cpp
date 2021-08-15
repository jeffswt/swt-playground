
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

// #define memclr(_arr)  memset(_arr, 0, sizeof(_arr))
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
#define nullptr 0

const int maxn = 2010;
const int maxm = 4010;

class Tarjan
{
public:
    struct edge
    {
        int u, v;
        bool is_bridge;
        edge *next, *rev;
    };
    edge epool[maxm], *edges[maxn];
    int n, ecnt, dcnt, bcnt;
    int dfn[maxn], low[maxn];
    int belong[maxn], bsize[maxn];
    vector<pair<int, int> > bridges;
    void add_edge(int u, int v)
    {
        edge *p = &epool[++ecnt],
             *q = &epool[++ecnt];
        p->u = u; p->v = v; p->is_bridge = false;
        p->next = edges[u]; edges[u] = p;
        q->u = v; q->v = u; q->is_bridge = false;
        q->next = edges[v]; edges[v] = q;
        p->rev = q; q->rev = p;
        return ;
    }
    void dfs1(int p, int par)
    {
        dfn[p] = low[p] = ++dcnt;
        for (edge *ep = edges[p]; ep; ep = ep->next) {
            int q = ep->v;
            if (!dfn[q]) {
                dfs1(q, p);
                minimize(low[p], low[q]);
                if (low[q] > dfn[p])
                    ep->is_bridge = ep->rev->is_bridge = true;
            } else if (dfn[q] < dfn[p] && q != par) {
                minimize(low[p], dfn[q]);
            }
        }
        return ;
    }
    void dfs2(int p)
    {
        dfn[p] = true;
        belong[p] = bcnt;
        bsize[bcnt] += 1;
        for (edge *ep = edges[p]; ep; ep = ep->next) {
            if (ep->is_bridge) {
                if (ep->u < ep->v)
                    bridges.push_back(make_pair(ep->u, ep->v));
                continue;
            }
            if (!dfn[ep->v])
                dfs2(ep->v);
        }
        return ;
    }
    int eval(void)
    {
        dcnt = bcnt = 0;
        rep(i, 1, n) {
            dfn[i] = low[i] = 0;
            belong[i] = 0;
        }
        rep(i, 1, n)
            if (!dfn[i])
                dfs1(i, 0);
        rep(i, 1, n)
            dfn[i] = false;  // use dfs[] as vis[]
        bridges.clear();
        rep(i, 1, n)
            if (!dfn[i]) {
                bsize[++bcnt] = 0;
                dfs2(i);
            }
        return bcnt;
    }
    void init(int n)
    {
        this->n = n;
        ecnt = 0;
        rep(i, 1, n)
            edges[i] = nullptr;
        return ;
    }
    // external code
    set<int> bdeg[maxn];
    int post_process(void)
    {
        rep(i, 1, n)
            bdeg[i].clear();
        rep(i, 1, n)
            for (edge *ep = edges[i]; ep; ep = ep->next)
                if (belong[ep->v] != belong[i])
                    bdeg[belong[i]].insert(belong[ep->v]);
        int cnt = 0;
        rep(i, 1, bcnt)
            if (bdeg[i].size() == 1)
                cnt += 1;
        return (cnt + 1) / 2;
    }
} graph;

int n, m;

int main(int argc, char** argv)
{
    scanf("%d%d", &n, &m);
    graph.init(n);
    rep(i, 1, m) {
        int u, v;
        scanf("%d%d", &u, &v);
        graph.add_edge(u, v);
    }
    graph.eval();
    int res = graph.post_process();
    printf("%d\n", res);
    return 0;
}
