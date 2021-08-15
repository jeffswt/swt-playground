
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

const int maxn = 1250;
const int maxm = 250000;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class HLPP
{
public:
    struct edge
    {
        int v, rev;
        lli flow;
        edge(int _1, int _2, lli _3) {
            v = _1, rev = _2, flow = _3; }
    };
    vector<edge> edges[maxn];
    int n, s, t, ecnt;
    int hlevel, level[maxn], cntl[maxn], wcounter;
    deque<int> lst[maxn];
    vector<int> gap[maxn];
    lli dist[maxn];
    void add_edge(int u, int v, lli flow, lli rflow = 0)
    {
        edges[u].push_back(edge(v, edges[v].size(), flow));
        edges[v].push_back(edge(u, edges[u].size() - 1, rflow));
        return ;
    }
    void update_height(int p, int nh)
    {
        wcounter += 1;
        if (level[p] != n + 2)
            cntl[level[p]] -= 1;
        level[p] = nh;
        if (nh == n + 2)
            return ;
        cntl[nh] += 1;
        hlevel = nh;
        gap[nh].push_back(p);
        if (dist[p] > 0)
            lst[nh].push_back(p);
        return ;
    }
    void relabel(void)
    {
        memclr(cntl, n);
        rep(i, 0, n)
            level[i] = n + 2;
        rep(i, 0, hlevel) {
            lst[i].clear();
            gap[i].clear();
        }
        wcounter = 0;
        queue<int> que;
        level[t] = 0;
        que.push(t);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            for (edge &ep : edges[p])
                if (level[ep.v] == n + 2 && edges[ep.v][ep.rev].flow > 0) {
                    que.push(ep.v);
                    update_height(ep.v, level[p] + 1);
                }
            hlevel = level[p];
        }
        return ;
    }
    void push(int p, edge& ep)
    {
        if (dist[ep.v] == 0)
            lst[level[ep.v]].push_back(ep.v);
        lli flow = min(dist[p], ep.flow);
        ep.flow -= flow;
        edges[ep.v][ep.rev].flow += flow;
        dist[p] -= flow;
        dist[ep.v] += flow;
        return ;
    }
    void discharge(int p)
    {
        int nh = n + 2;
        for (edge& ep : edges[p])
            if (ep.flow > 0) {
                if (level[p] == level[ep.v] + 1) {
                    push(p, ep);
                    if (dist[p] <= 0)
                        return ;
                } else {
                    nh = min(nh, level[ep.v] + 1);
                }
            }
        if (cntl[level[p]] > 1) {
            update_height(p, nh);
        } else {
            rep(i, level[p], n + 2 - 1) {
                for (auto j : gap[i])
                    update_height(j, n + 2);
                gap[i].clear();
            }
        }
        return ;
    }
    lli eval(void)
    {
        memclr(dist, n);
        dist[s] = infinit;
        dist[t] = - infinit;
        hlevel = 0;
        relabel();
        for (edge& ep : edges[s])
            push(s, ep);
        for (; hlevel >= 0; hlevel--)
            while (!lst[hlevel].empty()) {
                int p = lst[hlevel].back();
                lst[hlevel].pop_back();
                discharge(p);
                if (wcounter > 4 * n)
                    relabel();
            }
        return dist[t] + infinit;
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
            // scanf("%d%d%d", &a, &b, &c);
            a = read();
            b = read();
            c = read();
            graph.add_edge(a, b, c);
        }
        lli res = graph.eval();
        printf("%lld\n", res);
    }
    return 0;
}
