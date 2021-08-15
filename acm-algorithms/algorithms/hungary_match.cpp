
class HungaryMatch
{
public:
    struct edge
    {
        int u, v;
        edge *next;
    };
    edge epool[maxm], *edges[maxn];
    int n, ecnt, from[maxn], vis[maxn];
    void add_edge(int u, int v)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    bool find(int p)
    {
        for (edge *ep = edges[p]; ep; ep = ep->next)
            if (!vis[ep->v]) {
                vis[ep->v] = true;
                if (!from[ep->v] || find(from[ep->v])) {
                    from[ep->v] = p;
                    return true;
                }
            }
        return false;
    }
    int eval(void)
    {
        int res = 0;
        rep(i, 1, n) {
            memclr(vis, n);
            if (find(i))
                res += 1;
        }
        return res;
    }
    void init(int n)
    {
        this->n = n;
        ecnt = 0;
        memclr(edges, n);
        return ;
    }
} graph;
