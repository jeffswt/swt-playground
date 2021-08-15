
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

class FloydClosure
{
public:
    bool conn[maxn][maxn];
    int n;
    void add_edge(int u, int v)
    {
        conn[u][v] = true;
        return ;
    }
    void eval(void)
    {
        rep(k, 1, n)
            rep(i, 1, n)
                rep(j, 1, n)
                    conn[i][j] |= conn[i][k] && conn[k][j];
        return ;
    }
    void init(int n)
    {
        this->n = n;
        rep(i, 1, n)
            rep(j, 1, n)
                conn[i][j] = false;
        return ;
    }
} graph;

int n, m;

int main(int argc, char** argv)
{
    scanf("%d%d", &n, &m);
    graph.init(n);
    rep(i, 1, m) {
        int x, y;
        scanf("%d%d", &x, &y);
        graph.add_edge(x, y);
    }
    graph.eval();
    int res = 0;
    rep(i, 1, n) {
        int deg = 0;
        rep(j, 1, n)
            if (graph.conn[i][j] || graph.conn[j][i])
                deg += 1;
        if (deg == n - 1)
            res += 1;
    }
    printf("%d\n", res);
    return 0;
}
