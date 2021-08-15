
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
