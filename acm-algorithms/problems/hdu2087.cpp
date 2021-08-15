
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

const int maxn = 1000100;

class KMP
{
public:
    int m, patt[maxn], nxt[maxn];
    int n, str[maxn];
    void get_next(void)
    {
        int i = 0, j = -1;
        nxt[0] = -1;
        for (int i = 0, j = -1; i < m; ) {
            if (j == -1 || patt[i] == patt[j])
                i += 1, j += 1, nxt[i] = j;
            else
                j = nxt[j];
        }
        return ;
    }
    vector<int> match(bool overlap = true)
    {
        vector<int> res;
        for (int i = 0, j = 0; i < n; ) {
            if (j == -1 || str[i] == patt[j])
                i += 1, j += 1;
            else
                j = nxt[j];
            if (j == m)
                res.push_back(i), j = overlap ? nxt[j] : 0;
        }
        return res;
    }
    void init_patt(int m, char patt[])
    {
        this->m = m;
        rep(i, 0, m - 1)
            this->patt[i] = patt[i];
        memclr(nxt, m);
        get_next();
        return ;
    }
    void init_str(int n, char str[])
    {
        this->n = n;
        rep(i, 0, n - 1)
            this->str[i] = str[i];
        return ;
    }
} kmp;

int T, n, m;
char str[maxn], patt[maxn];

int main(int argc, char** argv)
{
    while (true) {
        scanf("%s", str);
        if (str[0] == '#')
            break;
        scanf("%s", patt);
        m = strlen(patt);
        n = strlen(str);
        kmp.init_patt(m, patt);
        kmp.init_str(n, str);
        int res = kmp.match(false).size();
        printf("%d\n", res);
    }
    return 0;
}
