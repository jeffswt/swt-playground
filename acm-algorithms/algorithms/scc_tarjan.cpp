
class Tarjan
{
public:
    struct edge
    {
        int u, v;
        edge *next;
    } epool[maxm], *edges[maxn];
    int n, ecnt, dcnt, bcnt;
    stack<int> stk;
    int instk[maxn], dfn[maxn], low[maxn];
    int belong[maxn], bsize[maxn];
    void add_edge(int u, int v)
    {
        edge *p = &epool[++ecnt];
        p->u = u; p->v = v;
        p->next = edges[u]; edges[u] = p;
        return ;
    }
    void dfs(int p)
    {
        low[p] = dfn[p] = ++dcnt;
        stk.push(p);
        instk[p] = true;
        for (edge *ep = edges[p]; ep; ep = ep->next) {
            int q = ep->v;
            if (!dfn[q]) {
                dfs(q);
                if (low[q] < low[p])
                    low[p] = low[q];
            } else if (instk[q] && dfn[q] < low[p]) {
                low[p] = dfn[q];
            }
        }
        if (dfn[p] == low[p]) {
            bsize[++bcnt] = 0;
            int q = 0;
            do {
                q = stk.top();
                stk.pop();
                instk[q] = false;
                belong[q] = bcnt;
                bsize[bcnt] += 1;
            } while (q != p);
        }
        return ;
    }
    int eval(void)
    {
        while (!stk.empty())
            stk.pop();
        dcnt = bcnt = 0;  // dfs counter, component counter
        rep(i, 1, n) {
            dfn[i] = low[i] = 0;
            instk[i] = false;
            belong[i] = bsize[i] = 0;
        }
        rep(i, 1, n)
            if (!dfn[i])
                dfs(i);
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
