
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
