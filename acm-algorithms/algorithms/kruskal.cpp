
class Kruskal
{
public:
    struct edge
    {
        int u, v;
        lli len;
        bool operator < (const edge& b) const
        {
            return this->len < b.len;
        }
    } edges[maxm];
    int n, m, mst_cnt;
    void add_edge(int u, int v, lli len)
    {
        edge *ep = &edges[++m];
        ep->u = u; ep->v = v; ep->len = len;
        return ;
    }
    void join(int u, int v)
    {
        if (!dsu.joined(u, v)) {
            dsu.join(u, v);
            mst_cnt += 1;
        }
        return ;
    }
    lli eval(void)
    {
        lli min_span = 0;
        sort(edges + 1, edges + m + 1);
        rep(i, 1, m) {
            if (mst_cnt >= n - 1)
                break;
            int u = edges[i].u, v = edges[i].v, len = edges[i].len;
            if (dsu.joined(u, v))
                continue;
            dsu.join(u, v);
            // printf("add_edge %d -> %d : %lld\n", u, v, len);
            min_span += len;
            mst_cnt += 1;
        }
        if (mst_cnt < n - 1)
            min_span = -1;
        return min_span;
    }
    void init(int n)
    {
        this->n = n;
        m = 0;
        mst_cnt = 0;
        dsu.init(n);
        return ;
    }
} graph;
