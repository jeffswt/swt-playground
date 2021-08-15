
class HLPPPointer
{
public:
    struct edge
    {
        int u, v;
        lli flow;
        edge *next, *rev;
    } epool[maxm], *edges[maxn];
    int n, s, t, ecnt;
    int hlevel, level[maxn], cntl[maxn], wcounter;
    deque<int> lst[maxn];
    vector<int> gap[maxn];
    lli dist[maxn];
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
    void update_height(int p, int nh)
    {
        wcounter += 1;
        if (level[p] != n + 2)
            cntl[level[p]] -= 1;
        level[p] = nh;
        if (nh == n + 2)
            return ;
        cntl[nh] += 1;
        hlevel = nh;
        gap[nh].push_back(p);
        if (dist[p] > 0)
            lst[nh].push_back(p);
        return ;
    }
    void relabel(void)
    {
        memclr(cntl, n);
        rep(i, 0, n)
            level[i] = n + 2;
        rep(i, 0, hlevel) {
            lst[i].clear();
            gap[i].clear();
        }
        wcounter = 0;
        queue<int> que;
        level[t] = 0;
        que.push(t);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (level[ep->v] == n + 2 && ep->rev->flow > 0) {
                    que.push(ep->v);
                    update_height(ep->v, level[p] + 1);
                }
            hlevel = level[p];
        }
        return ;
    }
    void push(int p, edge *ep)
    {
        if (dist[ep->v] == 0)
            lst[level[ep->v]].push_back(ep->v);
        lli flow = min(dist[p], ep->flow);
        ep->flow -= flow;
        ep->rev->flow += flow;
        dist[p] -= flow;
        dist[ep->v] += flow;
        return ;
    }
    void discharge(int p)
    {
        int nh = n + 2;
        for (edge *ep = edges[p]; ep; ep = ep->next)
            if (ep->flow > 0) {
                if (level[p] == level[ep->v] + 1) {
                    push(p, ep);
                    if (dist[p] <= 0)
                        return ;
                } else {
                    nh = min(nh, level[ep->v] + 1);
                }
            }
        if (cntl[level[p]] > 1) {
            update_height(p, nh);
        } else {
            rep(i, level[p], n + 2 - 1) {
                for (auto j : gap[i])
                    update_height(j, n + 2);
                gap[i].clear();
            }
        }
        return ;
    }
    lli eval(void)
    {
        memclr(dist, n);
        dist[s] = infinit;
        dist[t] = - infinit;
        hlevel = 0;
        relabel();
        for (edge *ep = edges[s]; ep; ep = ep->next)
            push(s, ep);
        for (; hlevel >= 0; hlevel--)
            while (!lst[hlevel].empty()) {
                int p = lst[hlevel].back();
                lst[hlevel].pop_back();
                discharge(p);
                if (wcounter > 4 * n)
                    relabel();
            }
        return dist[t] + infinit;
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

class HLPPVeryFast
{
public:
    struct edge
    {
        int v, rev;
        lli flow;
        edge(int _1, int _2, lli _3) {
            v = _1, rev = _2, flow = _3; }
    };
    vector<edge> edges[maxn];
    int n, s, t, ecnt;
    int hlevel, level[maxn], cntl[maxn], wcounter;
    deque<int> lst[maxn];
    vector<int> gap[maxn];
    lli dist[maxn];
    void add_edge(int u, int v, lli flow, lli rflow = 0)
    {
        edges[u].push_back(edge(v, edges[v].size(), flow));
        edges[v].push_back(edge(u, edges[u].size() - 1, rflow));
        return ;
    }
    void update_height(int p, int nh)
    {
        wcounter += 1;
        if (level[p] != n + 2)
            cntl[level[p]] -= 1;
        level[p] = nh;
        if (nh == n + 2)
            return ;
        cntl[nh] += 1;
        hlevel = nh;
        gap[nh].push_back(p);
        if (dist[p] > 0)
            lst[nh].push_back(p);
        return ;
    }
    void relabel(void)
    {
        memclr(cntl, n);
        rep(i, 0, n)
            level[i] = n + 2;
        rep(i, 0, hlevel) {
            lst[i].clear();
            gap[i].clear();
        }
        wcounter = 0;
        queue<int> que;
        level[t] = 0;
        que.push(t);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            for (edge &ep : edges[p])
                if (level[ep.v] == n + 2 && edges[ep.v][ep.rev].flow > 0) {
                    que.push(ep.v);
                    update_height(ep.v, level[p] + 1);
                }
            hlevel = level[p];
        }
        return ;
    }
    void push(int p, edge& ep)
    {
        if (dist[ep.v] == 0)
            lst[level[ep.v]].push_back(ep.v);
        lli flow = min(dist[p], ep.flow);
        ep.flow -= flow;
        edges[ep.v][ep.rev].flow += flow;
        dist[p] -= flow;
        dist[ep.v] += flow;
        return ;
    }
    void discharge(int p)
    {
        int nh = n + 2;
        for (edge& ep : edges[p])
            if (ep.flow > 0) {
                if (level[p] == level[ep.v] + 1) {
                    push(p, ep);
                    if (dist[p] <= 0)
                        return ;
                } else {
                    nh = min(nh, level[ep.v] + 1);
                }
            }
        if (cntl[level[p]] > 1) {
            update_height(p, nh);
        } else {
            rep(i, level[p], n + 2 - 1) {
                for (auto j : gap[i])
                    update_height(j, n + 2);
                gap[i].clear();
            }
        }
        return ;
    }
    lli eval(void)
    {
        memclr(dist, n);
        dist[s] = infinit;
        dist[t] = - infinit;
        hlevel = 0;
        relabel();
        for (edge& ep : edges[s])
            push(s, ep);
        for (; hlevel >= 0; hlevel--)
            while (!lst[hlevel].empty()) {
                int p = lst[hlevel].back();
                lst[hlevel].pop_back();
                discharge(p);
                if (wcounter > 4 * n)
                    relabel();
            }
        return dist[t] + infinit;
    }
    void init(int n, int s, int t)
    {
        this->n = n;
        this->s = s;
        this->t = t;
        ecnt = 0;
        rep(i, 1, n)
            edges[i].clear();
        return ;
    }
} graph;
