
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

template <typename typ>
void memclr(typ p) {
    memset(p, 0, sizeof(p)); }
template <typename typ>
void memclr(typ arr[], int n) {
    memset(arr, 0, sizeof(arr[0]) * (n + 1)); }
template <typename typ, int dim>
void memclr(typ arr[][dim], int n, int m) {
    rep(i, 0, n) memset(arr[i], 0, sizeof(arr[i][0]) * (m + 1)); }

lli read(void)
{
    lli res = 0, sgn = 1;
    char ch = getchar();
    while(ch < '0' || ch > '9')
        sgn = ch == '-' ? -1 : 1, ch = getchar();
    while(ch >= '0' && ch <= '9')
        res = res * 10 + ch - '0', ch = getchar();
    return res * sgn;
}

const int maxn = 5010;
const int maxm = 100100;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class SPFACostFlow
{
public:
    typedef pair<lli, int> pli;
    struct edge
    {
        int u, v;
        lli flow, cost;
        edge *next, *rev;
    } epool[maxm], *edges[maxn], *from[maxn];
    int n, s, t, ecnt;
    int vis[maxn];
    lli dist[maxn], height[maxn];
    void add_edge(int u, int v, lli flow, lli cost)
    {
        edge *p = &epool[++ecnt],
             *q = &epool[++ecnt];
        p->u = u; p->v = v; p->flow = flow; p->cost = cost;
        p->next = edges[u]; edges[u] = p;
        q->u = v; q->v = u; q->flow = 0; q->cost = - cost;
        q->next = edges[v]; edges[v] = q;
        p->rev = q; q->rev = p;
        return ;
    }
    bool spfa(void)
    {
        rep(i, 1, n)
            dist[i] = infinit;
        priority_queue<pli, vector<pli>, greater<pli>> pq;
        dist[s] = 0;
        pq.push(make_pair(dist[s], s));
        while (!pq.empty()) {
            pli pr = pq.top();
            int p = pr.second;
            pq.pop();
            if (dist[p] < pr.first)
                continue;
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (ep->flow > 0 && dist[p] + ep->cost +
                        height[p] - height[ep->v] < dist[ep->v]) {
                    dist[ep->v] = dist[p] + ep->cost + height[p] -
                                  height[ep->v];
                    from[ep->v] = ep;
                    pq.push(make_pair(dist[ep->v], ep->v));
                }
        }
        return dist[t] < infinit;
    }
    lli dfs(int p, lli flow, lli& rcost)
    {
        if (p == t || flow == 0)
            return flow;
        vis[p] = true;
        lli used = 0;
        for (edge *ep = edges[p]; ep; ep = ep->next)
            if (!vis[ep->v] && ep->flow > 0 && height[ep->v] ==
                    height[p] + ep->cost) {
                lli tmp = dfs(ep->v, min(flow - used, ep->flow), rcost);
                used += tmp;
                ep->flow -= tmp;
                ep->rev->flow += tmp;
                rcost += tmp * ep->cost;
                if (used == flow)
                    break;
            }
        vis[p] = false;
        return used;
    }
    pair<lli, lli> eval(void)
    {
        memclr(height, n);
        lli rflow = 0, rcost = 0;
        while (spfa()) {
            rep(i, 1, n)
                height[i] = min(infinit, height[i] + dist[i]);
            lli tmp = dfs(s, infinit, rcost);
            if (tmp == 0)
                break;
            rflow += tmp;
        }
        return make_pair(rflow, rcost);
    }
    void init(int n, int s, int t)
    {
        this->n = n;
        this->s = s;
        this->t = t;
        ecnt = 0;
        memclr(edges, n);
        return ;
    }
} graph;

int T, n, m;

int main(int argc, char** argv)
{
    int T, n, m, s, t;
    T = 1;
    rep(case_, 1, T) {
        // scanf("%d%d%d%d", &n, &m, &s, &t);
        n = read();
        m = read();
        s = read();
        t = read();
        graph.init(n, s, t);
        rep(i, 1, m) {
            int a, b, c, d;
            // scanf("%d%d%d%d", &a, &b, &c, &d);
            a = read();
            b = read();
            c = read();
            d = read();
            graph.add_edge(a, b, c, d);
        }
        auto res = graph.eval();
        printf("%lld %lld\n", res.first, res.second);
    }
    return 0;
}
