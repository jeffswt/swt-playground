
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
