
class Dijkstra
{
public:
    struct edge
    {
        int u, v;
        lli len;
        edge *next;
    } epool[maxm], *edges[maxn], *from[maxn];
    int n, ecnt;
    lli dist[maxn];
    bool vis[maxn];
    typedef pair<lli, int> pli;
    void add_edge(int u, int v, lli len)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v; p->len = len;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    void eval(int s)
    {
        priority_queue<pli, vector<pli>, greater<pli>> pq;
        rep(i, 0, n) {
            vis[i] = false;
            dist[i] = infinit;
            from[i] = nullptr;
        }
        dist[s] = 0;
        pq.push(make_pair(dist[s], s));
        while (!pq.empty()) {
            pli pr = pq.top();
            pq.pop();
            int p = pr.second;
            if (vis[p])
                continue;
            vis[p] = true;
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (!vis[ep->v] && dist[p] + ep->len < dist[ep->v]) {
                    dist[ep->v] = dist[p] + ep->len;
                    from[ep->v] = ep;
                    pq.push(make_pair(dist[ep->v], ep->v));
                }
        }
        return ;
    }
    vector<int> get_path(int p)
    {
        stack<int> stk;
        stk.push(p);
        edge *ep = from[p];
        while (ep) {
            stk.push(ep->u);
            ep = from[ep->u];
        }
        vector<int> res;
        while (!stk.empty()) {
            res.push_back(stk.top());
            stk.pop();
        }
        return res;
    }
    void init(int n)
    {
        this->n = n;
        ecnt = 0;
        rep(i, 1, n)
            edges[i] = nullptr;
        return ;
    }
} graph;
