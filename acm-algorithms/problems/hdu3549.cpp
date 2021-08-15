
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

const int maxn = 10010;
const int maxm = 30010;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class Dinic
{
public:
    struct edge
    {
        int u, v;
        lli flow;
        edge *next, *rev;
    } epool[maxm], *edges[maxn];
    int n, s, t, ecnt, level[maxn];
    void add_edge(int u, int v, lli flow, lli rflow = 0)
    {
        edge *p = &epool[++ecnt],
             *q = &epool[++ecnt];
        p->u = u; p->v = v; p->flow = flow;
        p->next = edges[u]; edges[u] = p;
        q->u = v; q->v = u; q->flow = rflow;
        q->next = edges[v]; edges[v] = q;
        p->rev = q; q->rev = p;
        return ;
    }
    bool make_level(void)
    {
        rep(i, 1, n)
            level[i] = 0;
        queue<int> que;
        level[s] = 1;
        que.push(s);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (ep->flow > 0 && !level[ep->v]) {
                    level[ep->v] = level[p] + 1;
                    que.push(ep->v);
                }
            if (level[t] > 0)
                return true;
        }
        return false;
    }
    lli find(int p, lli mn)
    {
        if (p == t)
            return mn;
        lli tmp = 0, sum = 0;
        for (edge *ep = edges[p]; ep && sum < mn; ep = ep->next)
            if (ep->flow && level[ep->v] == level[p] + 1) {
                tmp = find(ep->v, min(mn, ep->flow));
                if (tmp > 0) {
                    sum += tmp;
                    ep->flow -= tmp;
                    ep->rev->flow += tmp;
                    return tmp;
                }
            }
        if (sum == 0)
            level[p] = 0;
        return 0;
    }
    lli eval(void)
    {
        lli tmp, sum = 0;
        while (make_level()) {
            bool found = false;
            while (tmp = find(s, infinit)) {
                sum += tmp;
                found = true;
            }
            if (!found)
                break;
        }
        return sum;
    }
    void init(int n, int s, int t)
    {
        this->n = n;
        this->s = s;
        this->t = t;
        ecnt = 0;
        rep(i, 1, n)
            edges[i] = nullptr;
        return ;
    }
} graph;

int T, n, m;

int main(int argc, char** argv)
{
    scanf("%d", &T);
    rep(case_, 1, T) {
        scanf("%d%d", &n, &m);
        graph.init(n, 1, n);
        rep(i, 1, m) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            graph.add_edge(a, b, c);
        }
        lli res = graph.eval();
        printf("Case %d: %lld\n", case_, res);
    }
    return 0;
}
