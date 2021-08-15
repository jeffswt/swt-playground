
class SPFA
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
    int qcnt[maxn], inque[maxn];
    void add_edge(int u, int v, lli len)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v; p->len = len;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    bool eval(int s)
    {
        #define USE_SLF
        #ifdef USE_SLF
        deque<int> que;
        #else
        queue<int> que;
        #endif
        rep(i, 0, n) {
            qcnt[i] = 0;
            inque[i] = false;
            dist[i] = infinit;
            from[i] = nullptr;
        }
        dist[s] = 0;
        qcnt[s] += 1;
        inque[s] = true;
        #ifdef USE_SLF
        que.push_back(s);
        #else
        que.push(s);
        #endif
        while (!que.empty()) {
            int p = que.front();
            #ifdef USE_SLF
            que.pop_front();
            #else
            que.pop();
            #endif
            inque[p] = false;
            for (edge *ep = edges[p]; ep; ep = ep->next)
                if (dist[p] + ep->len < dist[ep->v]) {
                    dist[ep->v] = dist[p] + ep->len;
                    from[ep->v] = ep;
                    if (!inque[ep->v]) {
                        inque[ep->v] = true;
                        qcnt[ep->v] += 1;
                        if (qcnt[ep->v] >= n)
                            return false;
                        #ifdef USE_SLF
                        if (que.empty() || dist[ep->v] > dist[que.front()])
                            que.push_back(ep->v);
                        else
                            que.push_front(ep->v);
                        #else
                        que.push(ep->v);
                        #endif
                    }
                }
        }
        return true;
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
