
class TwoSAT
{
public:
    int n;
    #define constrain(_p, _vp, _q, _vq)  \
            graph.add_edge(2 * (_p) - (_vp), 2 * (_q) - (_vq))
    void set_true(int p) {
        constrain(p, 0, p, 1); }
    void set_false(int p) {
        constrain(p, 1, p, 0); }
    void require_and(int p, int q) {
        constrain(p, 0, p, 1);
        constrain(q, 0, q, 1); }
    void require_or(int p, int q) {
        constrain(p, 0, q, 1);
        constrain(q, 0, p, 1); }
    void require_nand(int p, int q) {
        constrain(p, 1, q, 0);
        constrain(q, 1, p, 0); }
    void require_nor(int p, int q) {
        constrain(p, 1, p, 0);
        constrain(q, 1, q, 0); }
    void require_xor(int p, int q) {
        constrain(p, 1, q, 0);
        constrain(q, 1, p, 0);
        constrain(p, 0, q, 1);
        constrain(q, 0, p, 1); }
    void require_xnor(int p, int q) {
        constrain(p, 1, q, 1);
        constrain(q, 1, p, 1);
        constrain(p, 0, q, 0);
        constrain(q, 0, p, 0); }
    void require_eq(int p, int q) {
        require_xnor(p, q); }
    void require_neq(int p, int q) {
        require_xor(p, q); }
    #undef constrain
    void dfs(int p, int res[])
    {
        int id = (p + 1) / 2;
        if (res[id] != -1)
            return ;
        res[id] = 2 * id - p;
        for (auto* ep = graph.edges[p]; ep; ep = ep->next)
            dfs(ep->v, res);
        return ;
    }
    bool eval(int res[])
    {
        graph.eval();
        rep(i, 1, n)
            if (graph.belong[2 * i] == graph.belong[2 * i - 1])
                return false;
        rep(i, 1, n)
            res[i] = -1;
        rep(i, 1, n)
            if (res[i] == -1)
                dfs(2 * i, res);
        return true;
    }
    void init(int n)
    {
        this->n = n;
        graph.init(2 * n);
        return ;
    }
} tsat;
