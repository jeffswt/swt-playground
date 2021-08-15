
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

const int maxn = 2010;
const int maxm = 1000100;
#define nullptr 0

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
} graph;

class TwoSAT
{
public:
    int n;
    #define constrain(_p, _vp, _q, _vq)  \
            graph.add_edge(2 * (_p) - (_vp), 2 * (_q) - (_vq))
    void set_true(int p) {
        constrain(p, 0, p, 1); }
    void set_false(int p) {
        constrain(p, 1, p, 0); }
    void require_and(int p, int q) {
        constrain(p, 0, p, 1);
        constrain(q, 0, q, 1); }
    void require_or(int p, int q) {
        constrain(p, 0, q, 1);
        constrain(q, 0, p, 1); }
    void require_nand(int p, int q) {
        constrain(p, 1, q, 0);
        constrain(q, 1, p, 0); }
    void require_nor(int p, int q) {
        constrain(p, 1, p, 0);
        constrain(q, 1, q, 0); }
    void require_xor(int p, int q) {
        constrain(p, 1, q, 0);
        constrain(q, 1, p, 0);
        constrain(p, 0, q, 1);
        constrain(q, 0, p, 1); }
    void require_xnor(int p, int q) {
        constrain(p, 1, q, 1);
        constrain(q, 1, p, 1);
        constrain(p, 0, q, 0);
        constrain(q, 0, p, 0); }
    void require_eq(int p, int q) {
        require_xnor(p, q); }
    void require_neq(int p, int q) {
        require_xor(p, q); }
    #undef constrain
    void dfs(int p, int res[])
    {
        int id = (p + 1) / 2;
        if (res[id] != -1)
            return ;
        res[id] = 2 * id - p;
        for (Tarjan::edge* ep = graph.edges[p]; ep; ep = ep->next)
            dfs(ep->v, res);
        return ;
    }
    bool eval(int res[])
    {
        graph.eval();
        rep(i, 1, n)
            if (graph.belong[2 * i] == graph.belong[2 * i - 1])
                return false;
        rep(i, 1, n)
            res[i] = -1;
        rep(i, 1, n)
            if (res[i] == -1)
                dfs(2 * i, res);
        return true;
    }
    void init(int n)
    {
        this->n = n;
        graph.init(2 * n);
        return ;
    }
} tsat;

int n, m;
char op[10];
int res[maxn];

int main(int argc, char** argv)
{
    scanf("%d%d", &n, &m);
    tsat.init(n);
    rep(i, 1, m) {
        int a, b, c;
        scanf("%d%d%d%s", &a, &b, &c, op);
        a += 1, b += 1;
        if (c == 1 && op[0] == 'A')
            tsat.require_and(a, b);
        else if (c == 1 && op[0] == 'O')
            tsat.require_or(a, b);
        else if (c == 1 && op[0] == 'X')
            tsat.require_xor(a, b);
        else if (c == 0 && op[0] == 'A')
            tsat.require_nand(a, b);
        else if (c == 0 && op[0] == 'O')
            tsat.require_nor(a, b);
        else if (c == 0 && op[0] == 'X')
            tsat.require_xnor(a, b);
    }
    printf("%s\n", tsat.eval(res) ? "YES" : "NO");
    return 0;
}
