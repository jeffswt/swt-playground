
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

const int maxn = 1010, maxm = 10010;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class SPFA
{
public:
    struct edge
    {
        int u, v;
        lli len;
        edge *next;
    };
    edge epool[maxm], *edges[maxn], *from[maxn];
    int n, ecnt;
    lli dist[maxn];
    int qcnt[maxn], inque[maxn];
    void add_edge(int u, int v, lli len)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v; p->len = len;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    bool eval(int s)
    {
        #define USE_SLF
        #ifdef USE_SLF
        deque<int> que;
        #else
        queue<int> que;
        #endif
        rep(i, 0, n) {
            qcnt[i] = 0;
            inque[i] = false;
            dist[i] = infinit;
            from[i] = 0;
        }
        dist[s] = 0;
        qcnt[s] += 1;
        inque[s] = true;
        #ifdef USE_SLF
        que.push_back(s);
        #else
        que.push(s);
        #endif
        while (!que.empty()) {
            int p = que.front();
            #ifdef USE_SLF
            que.pop_front();
            #else
            que.pop();
            #endif
            inque[p] = false;
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (dist[p] + ep->len < dist[ep->v]) {
                    dist[ep->v] = dist[p] + ep->len;
                    from[ep->v] = ep;
                    if (!inque[ep->v]) {
                        inque[ep->v] = true;
                        qcnt[ep->v] += 1;
                        if (qcnt[ep->v] >= n)
                            return false;
                        #ifdef USE_SLF
                        if (que.empty() || dist[ep->v] > dist[que.front()])
                            que.push_back(ep->v);
                        else
                            que.push_front(ep->v);
                        #else
                        que.push(ep->v);
                        #endif
                    }
                }
        }
        return true;
    }
    void init(int n)
    {
        this->n = n;
        ecnt = 0;
        rep(i, 1, n)
            edges[i] = 0;
        return ;
    }
} graph;

int T;
int n, m, mM, mW;

int main(int argc, char** argv)
{
    scanf("%d", &T);
    rep(case_, 1, T) {
        scanf("%d%d%d", &n, &mM, &mW);
        graph.init(n + 1);
        m = mM + mW;
        rep(i, 1, mM) {
            int u, v, w;
            scanf("%d%d%d", &u, &v, &w);
            graph.add_edge(u, v, w);
            graph.add_edge(v, u, w);
        }
        rep(i, 1, mW) {
            int u, v, w;
            scanf("%d%d%d", &u, &v, &w);
            graph.add_edge(u, v, - w);
        }
        rep(i, 1, n)
            graph.add_edge(n + 1, i, 0);
        bool res = !graph.eval(n + 1);
        printf("%s\n", res ? "YES" : "NO");
    }
    return 0;
}
