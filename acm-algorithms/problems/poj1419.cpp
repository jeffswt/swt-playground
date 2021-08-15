
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

const int maxn = 1010;

class BronKerbosch
{
public:
    int n;
    bool edge[maxn][maxn];
    bool vis[maxn];
    void add_edge(int u, int v)
    {
        edge[u][v] = edge[v][u] = true;
        return ;
    }
    void remove_edge(int u, int v)
    {
        edge[u][v] = edge[v][u] = false;
        return ;
    }
    void dfs(int p, int& curn, int& bestn, vector<int>& res)
    {
        if (p > n) {
            res.clear();
            rep(i, 1, n)
                if (vis[i])
                    res.push_back(i);
            bestn = curn;
            return ;
        }
        bool flag = true;
        rep(i, 1, p - 1)
            if (vis[i] && !edge[i][p]) {
                flag = false;
                break;
            }
        if (flag) {
            curn += 1;
            vis[p] = true;
            dfs(p + 1, curn, bestn, res);
            curn -= 1;
            vis[p] = false;
        }
        if (curn + n - p > bestn)
            dfs(p + 1, curn, bestn, res);
        return ;
    }
    vector<int> eval(void)
    {
        int curn = 0, bestn = 0;
        vector<int> res;
        dfs(1, curn, bestn, res);
        return res;
    }
    void init(int n, bool state = false)
    {
        this->n = n;
        rep(i, 1, n)
            rep(j, 1, n)
                edge[i][j] = state;
        memclr(vis, n);
        return ;
    }
} graph;

int T, n, m;

int main(int argc, char** argv)
{
    scanf("%d", &T);
    rep(case_, 1, T) {
        scanf("%d%d", &n, &m);
        graph.init(n, true);
        rep(i, 1, m) {
            int a, b;
            scanf("%d%d", &a, &b);
            graph.remove_edge(a, b);
        }
        vector<int> res = graph.eval();
        printf("%d\n", res.size());
        rep(i, 1, res.size())
            printf("%d ", res[i - 1]);
        printf("\n");
    }
    return 0;
}
