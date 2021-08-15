
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

const int maxn = 400100;

class Trie
{
public:
    int ncnt, root;
    int ch[maxn][26];
    bool flag[maxn];
    int size[maxn], data[maxn];
    int make_node(void)
    {
        int p = ++ncnt;
        memclr(ch[p], 25);
        flag[p] = false;
        size[p] = data[p] = 0;
        return p;
    }
    void insert(char str[], int vdata = 0)
    {
        int p = root;
        for (int i = 0; str[i] != '\0'; i++) {
            int v = str[i] - 'a';
            size[p] += 1;
            if (!ch[p][v])
                ch[p][v] = make_node();
            p = ch[p][v];
        }
        size[p] += 1;
        flag[p] = true;
        data[p] = vdata;
        return ;
    }
    void remove(char str[])
    {
        int p = root;
        for (int i = 0; str[i] != '\0'; i++) {
            int v = str[i] - 'a';
            size[p] -= 1;
            p = ch[p][v];
        }
        size[p] -= 1;
        flag[p] = false;
        return ;
    }
    int find(char str[])
    {
        int p = root;
        for (int i = 0; str[i] != '\0'; i++) {
            int v = str[i] - 'a';
            if (!ch[p][v])
                return 0;
            p = ch[p][v];
        }
        return size[p];
    }
    void init(void)
    {
        ncnt = 0;
        root = make_node();
        return ;
    }
} trie;

char str[maxn];

int main(int argc, char** argv)
{
    while (gets(str)) {
        if (str[0] == '\0')
            break;
        trie.insert(str);
    }
    while (scanf("%s", str) != EOF) {
        printf("%d\n", trie.find(str));
    }
    return 0;
}
