
class SPFACostFlow
{
public:
    typedef pair<lli, int> pli;
    struct edge
    {
        int u, v;
        lli flow, cost;
        edge *next, *rev;
    } epool[maxm], *edges[maxn], *from[maxn];
    int n, s, t, ecnt;
    int vis[maxn];
    lli dist[maxn], height[maxn];
    void add_edge(int u, int v, lli flow, lli cost)
    {
        edge *p = &epool[++ecnt],
             *q = &epool[++ecnt];
        p->u = u; p->v = v; p->flow = flow; p->cost = cost;
        p->next = edges[u]; edges[u] = p;
        q->u = v; q->v = u; q->flow = 0; q->cost = - cost;
        q->next = edges[v]; edges[v] = q;
        p->rev = q; q->rev = p;
        return ;
    }
    bool spfa(void)
    {
        rep(i, 1, n)
            dist[i] = infinit;
        priority_queue<pli, vector<pli>, greater<pli>> pq;
        dist[s] = 0;
        pq.push(make_pair(dist[s], s));
        while (!pq.empty()) {
            pli pr = pq.top();
            int p = pr.second;
            pq.pop();
            if (dist[p] < pr.first)
                continue;
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (ep->flow > 0 && dist[p] + ep->cost +
                        height[p] - height[ep->v] < dist[ep->v]) {
                    dist[ep->v] = dist[p] + ep->cost + height[p] -
                                  height[ep->v];
                    from[ep->v] = ep;
                    pq.push(make_pair(dist[ep->v], ep->v));
                }
        }
        return dist[t] < infinit;
    }
    lli dfs(int p, lli flow, lli& rcost)
    {
        if (p == t || flow == 0)
            return flow;
        vis[p] = true;
        lli used = 0;
        for (edge *ep = edges[p]; ep; ep = ep->next)
            if (!vis[ep->v] && ep->flow > 0 && height[ep->v] ==
                    height[p] + ep->cost) {
                lli tmp = dfs(ep->v, min(flow - used, ep->flow), rcost);
                used += tmp;
                ep->flow -= tmp;
                ep->rev->flow += tmp;
                rcost += tmp * ep->cost;
                if (used == flow)
                    break;
            }
        vis[p] = false;
        return used;
    }
    pair<lli, lli> eval(void)
    {
        memclr(height, n);
        lli rflow = 0, rcost = 0;
        while (spfa()) {
            rep(i, 1, n)
                height[i] = min(infinit, height[i] + dist[i]);
            lli tmp = dfs(s, infinit, rcost);
            if (tmp == 0)
                break;
            rflow += tmp;
        }
        return make_pair(rflow, rcost);
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
