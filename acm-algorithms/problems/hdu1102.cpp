
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

const int maxn = 110;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class Prim
{
public:
    lli dist[maxn][maxn];
    int n, vis[maxn], min_cost[maxn];
    void add_edge(int u, int v, lli len)
    {
        dist[u][v] = dist[v][u] = len;
        return ;
    }
    void join(int u, int v)
    {
        add_edge(u, v, 0);
        return ;
    }
    lli eval(void)
    {
        lli min_span = 0;
        rep(i, 1, n) {
            vis[i] = false;
            min_cost[i] = 1;
        }
        vis[1] = true;
        rep(i, 1, n) {
            int p = 0;
            rep(j, 1, n)
                if (!vis[j] && dist[min_cost[j]][j] < dist[min_cost[p]][p])
                    p = j;
            if (p == 0)
                break;
            min_span += dist[min_cost[p]][p];
            // printf("add_edge %d -> %d : %lld\n", p, min_cost[p],
            //        dist[p][min_cost[p]]);
            vis[p] = true;
            rep(j, 1, n)
                if (dist[p][j] < dist[min_cost[j]][j])
                    min_cost[j] = p;
        }
        return min_span;
    }
    void init(int n)
    {
        this->n = n;
        rep(i, 0, n)
            rep(j, 0, n)
                dist[i][j] = infinit;
        return ;
    }
} graph;

int n, Q;

int main(int argc, char** argv)
{
    while (scanf("%d", &n) != EOF) {
        graph.init(n);
        rep(i, 1, n)
            rep(j, 1, n) {
                int l;
                scanf("%d", &l);
                graph.add_edge(i, j, l);
            }
        scanf("%d", &Q);
        rep(i, 1, Q) {
            int a, b;
            scanf("%d%d", &a, &b);
            graph.join(a, b);
        }
        lli res = graph.eval();
        printf("%lld\n", res);
    }
    return 0;
}