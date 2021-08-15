
class ISAP
{
public:
    struct edge
    {
        int u, v;
        lli flow;
        edge *next, *rev;
    } epool[maxm], *edges[maxn], *cedge[maxn];
    int n, s, t, ecnt, level[maxn], gap[maxn];
    lli maxflow;
    void add_edge(int u, int v, lli flow, lli rflow = 0)
    {
        edge *p = &epool[++ecnt],
             *q = &epool[++ecnt];
        p->u = u; p->v = v; p->flow = flow;
        p->next = edges[u]; edges[u] = p;
        q->u = v; q->v = u; q->flow = rflow;
        q->next = edges[v]; edges[v] = q;
        p->rev = q; q->rev = p;
        return ;
    }
    void make_level(void)
    {
        memclr(level, n);
        memclr(gap, n);
        queue<int> que;
        level[t] = 1;
        gap[level[t]] += 1;
        que.push(t);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (!level[ep->v]) {
                    level[ep->v] = level[p] + 1;
                    gap[level[ep->v]] += 1;
                    que.push(ep->v);
                }
        }
        return ;
    }
    lli find(int p, lli flow)
    {
        if (p == t)
            return flow;
        lli used = 0;
        for (edge *ep = cedge[p]; ep; ep = ep->next)
            if (ep->flow > 0 && level[ep->v] + 1 == level[p]) {
                lli tmp = find(ep->v, min(ep->flow, flow - used));
                if (tmp > 0) {
                    ep->flow -= tmp;
                    ep->rev->flow += tmp;
                    used += tmp;
                    cedge[p] = ep;
                }
                if (used == flow)
                    return used;
            }
        gap[level[p]] -= 1;
        if (gap[level[p]] == 0)
            level[s] = n + 1;
        level[p] += 1;
        gap[level[p]] += 1;
        cedge[p] = edges[p];
        return used;
    }
    lli eval(void)
    {
        lli res = 0;
        make_level();
        rep(i, 1, n)
            cedge[i] = edges[i];
        while (level[s] <= n)
            res += find(s, infinit);
        return res;
    }
    void init(int n, int s, int t)
    {
        this->n = n;
        this->s = s;
        this->t = t;
        ecnt = 0;
        memclr(edges, n);
        return ;
    }
} graph;
