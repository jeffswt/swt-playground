
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

const int maxn = 1010;
const int maxm = 30010;

class DisjointSet
{
public:
    int n, par[maxn], size[maxn];
    lli vsm[maxn];
    int find(int p)
    {
        if (par[p] != p)
            par[p] = find(par[p]);
        return par[p];
    }
    bool joined(int p, int q)
    {
        return find(p) == find(q);
    }
    void join(int p, int q)
    {
        int gp = find(p),
            gq = find(q);
        if (gp == gq)
            return ;
        if (size[gq] < size[gp])
            swap(gq, gp);
        par[gp] = gq;
        size[gq] += size[gp];
        vsm[gq] += vsm[gp];
        return ;
    }
    void init(int n, lli w[] = nullptr)
    {
        rep(i, 1, n) {
            par[i] = i;
            size[i] = 1;
            vsm[i] = w ? w[i] : 0;
        }
        return ;
    }
} dsu;

class Kruskal
{
public:
    struct edge
    {
        int u, v;
        lli len;
        bool operator < (const edge& b) const
        {
            return this->len < b.len;
        }
    } edges[maxm];
    int n, m, mst_cnt;
    void add_edge(int u, int v, lli len)
    {
        edge *ep = &edges[++m];
        ep->u = u; ep->v = v; ep->len = len;
        return ;
    }
    void join(int u, int v)
    {
        if (!dsu.joined(u, v)) {
            dsu.join(u, v);
            mst_cnt += 1;
        }
        return ;
    }
    lli eval(void)
    {
        lli min_span = 0;
        sort(edges + 1, edges + m + 1);
        rep(i, 1, m) {
            if (mst_cnt >= n - 1)
                break;
            int u = edges[i].u, v = edges[i].v, len = edges[i].len;
            if (dsu.joined(u, v))
                continue;
            dsu.join(u, v);
            // printf("add_edge %d -> %d : %lld\n", u, v, len);
            min_span += len;
            mst_cnt += 1;
        }
        if (mst_cnt < n - 1)
            min_span = -1;
        return min_span;
    }
    void init(int n)
    {
        this->n = n;
        m = 0;
        mst_cnt = 0;
        dsu.init(n);
        return ;
    }
} graph;

int T, n, m, k;

int main(int argc, char** argv)
{
    scanf("%d", &T);
    rep(case_, 1, T) {
        scanf("%d%d%d", &n, &m, &k);
        graph.init(n);
        rep(i, 1, m) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            graph.add_edge(a, b, c);
        }
        rep(i, 1, k) {
            int t, a, b;
            scanf("%d%d", &t, &a);
            rep(j, 2, t) {
                scanf("%d", &b);
                graph.join(a, b);
            }
        }
        lli res = graph.eval();
        printf("%lld\n", res);
    }
    return 0;
}
