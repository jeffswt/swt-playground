
class Tarjan
{
public:
    struct edge
    {
        int u, v;
        edge *next;
    } epool[maxm], *edges[maxn];
    int n, ecnt, dcnt, bcnt;
    stack<edge*> stk;
    int instk[maxn], dfn[maxn], low[maxn];
    int belong[maxn], bsize[maxn];
    bool is_cut[maxn];
    void add_edge(int u, int v)
    {
        edge *p = &epool[++ecnt],
             *q = &epool[++ecnt];
        p->u = u; p->v = v;
        p->next = edges[u]; edges[u] = p;
        q->u = v; q->v = u;
        q->next = edges[v]; edges[v] = q;
        return ;
    }
    void dfs(int p, int par)
    {
        int child = 0;
        dfn[p] = low[p] = ++dcnt;
        for (edge *ep = edges[p]; ep; ep = ep->next) {
            int q = ep->v;
            if (!dfn[q]) {
                stk.push(ep);
                child += 1;
                dfs(ep->v, p);
                minimize(low[p], low[q]);
                if (dfn[p] <= low[q]) {
                    is_cut[p] = true;
                    bcnt += 1;
                    edge *eq = nullptr;
                    do {
                        eq = stk.top();
                        stk.pop();
                        if (belong[eq->u] != bcnt) {
                            belong[eq->u] = bcnt;
                            bsize[bcnt] += 1;
                        }
                        if (belong[eq->v] != bcnt) {
                            belong[eq->v] = bcnt;
                            bsize[bcnt] += 1;
                        }
                    } while (eq->u != p || eq->v != q);
                }
            } else if (dfn[q] < dfn[p] && q != par) {
                stk.push(ep);
                minimize(low[p], dfn[q]);
            }
        }
        if (par == 0 && child == 1)
            is_cut[p] = false;
        return ;
    }
    int eval(void)
    {
        while (!stk.empty())
            stk.pop();
        dcnt = bcnt = 0;
        rep(i, 1, n) {
            dfn[i] = low[i] = 0;
            instk[i] = iscut[i] = false;
            belong[i] = 0;
        }
        rep(i, 1, n)
            if (!dfn[i])
                dfs(i, 0);
        return bcnt;
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
