
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

const int maxn = 110;
const lli infinit = 0x003f3f3f3f3f3f3fll;

class Floyd
{
public:
    lli dist[maxn][maxn];
    int n;
    void add_edge(int u, int v, lli len)
    {
        dist[u][v] = len;
        return ;
    }
    void eval(void)
    {
        rep(k, 1, n)
            rep(i, 1, n)
                rep(j, 1, n)
                    if (dist[i][k] + dist[k][j] < dist[i][j])
                        dist[i][j] = dist[i][k] + dist[k][j];
        return ;
    }
    void init(int n)
    {
        this->n = n;
        rep(i, 1, n)
            rep(j, 1, n)
                dist[i][j] = i == j ? 0 : infinit;
        return ;
    }
} graph;

int T, n, m;
lli x[maxn];

int main(int argc, char** argv)
{
    scanf("%d", &T);
    rep(case_, 1, T) {
        lli L[5], C[5];
        scanf("%lld%lld%lld%lld", &L[1], &L[2], &L[3], &L[4]);
        scanf("%lld%lld%lld%lld", &C[1], &C[2], &C[3], &C[4]);
        scanf("%d%d", &n, &m);
        rep(i, 1, n)
            scanf("%lld", &x[i]);
        graph.init(n);
        rep(i, 1, n)
            rep(j, i + 1, n) {
                #define proc(val)                 \
                if (abs(x[i] - x[j]) <= L[val])   \
                    graph.add_edge(i, j, C[val]), \
                    graph.add_edge(j, i, C[val])
                proc(1);
                else proc(2);
                else proc(3);
                else proc(4);
                #undef proc
            }
        graph.eval();
        printf("Case %d:\n", case_);
        rep(i, 1, m) {
            int u, v;
            scanf("%d%d", &u, &v);
            if (graph.dist[u][v] >= infinit)
                printf("Station %d and station %d are not attainable.\n", u, v);
            else
                printf("The minimum cost between station %d and station %d is %lld.\n",
                       u, v, graph.dist[u][v]);
        }
    }
    return 0;
}
