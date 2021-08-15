
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

const int maxn = 10010;
const int maxm = 50010;

class Tarjan
{
public:
    struct edge
    {
        int u, v;
        edge *next;
    };
    edge epool[maxm], *edges[maxn];
    int n, ecnt, dcnt, bcnt;
    stack<int> stk;
    int instk[maxn], dfn[maxn], low[maxn];
    int belong[maxn], bsize[maxn];
    void add_edge(int u, int v)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    void dfs(int p)
    {
        low[p] = dfn[p] = ++dcnt;
        stk.push(p);
        instk[p] = true;
        for (edge *ep = edges[p]; ep; ep = ep->next) {
            int q = ep->v;
            if (!dfn[q]) {
                dfs(q);
                if (low[q] < low[p])
                    low[p] = low[q];
            } else if (instk[q] && dfn[q] < low[p]) {
                low[p] = dfn[q];
            }
        }
        if (dfn[p] == low[p]) {
            bsize[++bcnt] = 0;
            int q = 0;
            do {
                q = stk.top();
                stk.pop();
                instk[q] = false;
                belong[q] = bcnt;
                bsize[bcnt] += 1;
            } while (q != p);
        }
        return ;
    }
    int eval(void)
    {
        while (!stk.empty())
            stk.pop();
        dcnt = bcnt = 0;  // dfs counter, component counter
        rep(i, 1, n) {
            dfn[i] = low[i] = 0;
            instk[i] = false;
            belong[i] = bsize[i] = 0;
        }
        rep(i, 1, n)
            if (!dfn[i])
                dfs(i);
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
    /* external code */
    int in_deg[maxn];
    int out_deg[maxn];
    pair<int, int> post_process(void)
    {
        rep(i, 1, n)
            in_deg[i] = out_deg[i] = 0;
        rep(i, 1, n)
            for (edge *ep = edges[i]; ep; ep = ep->next)
                if (belong[ep->v] != belong[i]) {
                    out_deg[belong[i]] += 1;
                    in_deg[belong[ep->v]] += 1;
                }
        int res_in = 0, res_out = 0;
        rep(i, 1, bcnt) {
            if (in_deg[i] == 0)
                res_in += 1;
            if (out_deg[i] == 0)
                res_out += 1;
        }
        if (bcnt == 1)
            return make_pair(res_in, 0);
        return make_pair(res_in, max(res_in, res_out));
    }
} graph;

int n, m;

int main(int argc, char** argv)
{
    scanf("%d", &n);
    graph.init(n);
    rep(i, 1, n) {
        int to;
        while (true) {
            scanf("%d", &to);
            if (to == 0)
                break;
            graph.add_edge(i, to);
        }
    }
    graph.eval();
    pair<int, int> res = graph.post_process();
    printf("%d\n%d\n", res.first, res.second);
    return 0;
}
