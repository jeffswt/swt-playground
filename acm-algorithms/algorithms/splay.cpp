
class SplayTree
{
public:
    int n, root, ncnt;
    int ch[maxn][2], parent[maxn], size[maxn];
    lli val[maxn], vsm[maxn], vmn[maxn], vmx[maxn];
    lli lazyadd[maxn];
    bool lazyswp[maxn];
    #define lc(x) ch[x][0]
    #define rc(x) ch[x][1]
    #define par(x) parent[x]
    void push_down(int p)
    {
        rep(_, 0, 1)
            if (ch[p][_]) {
                lazyadd[ch[p][_]] += lazyadd[p];
                val[ch[p][_]] += lazyadd[p];
                vsm[ch[p][_]] += size[ch[p][_]] * lazyadd[p];
                vmn[ch[p][_]] += lazyadd[p];
                vmx[ch[p][_]] += lazyadd[p];
                lazyswp[ch[p][_]] ^= lazyswp[p];
            }
        if (lazyswp[p])
            swap(lc(p), rc(p));
        lazyadd[p] = 0;
        lazyswp[p] = false;
        return ;
    }
    void pull_up(int p)
    {
        size[p] = size[lc(p)] + 1 + size[rc(p)];
        vsm[p] = vmn[p] = vmx[p] = val[p];
        rep(_, 0, 1)
            if (ch[p][_]) {
                vsm[p] += vsm[ch[p][_]];
                vmn[p] = min(vmn[p], vmn[ch[p][_]]);
                vmx[p] = max(vmx[p], vmx[ch[p][_]]);
            }
        return ;
    }
    int makenode(int q, lli v)
    {
        int p = ++ncnt; n += 1;
        lc(p) = rc(p) = 0;
        par(p) = q;
        size[p] = 1;
        val[p] = vsm[p] = vmn[p] = vmx[p] = v;
        lazyadd[p] = 0;
        lazyswp[p] = false;
        return p;
    }
    void rotate(int p)
    {
        int q = par(p), g = par(q);
        push_down(q);
        push_down(p);
        int x = p == rc(q);
        // relink connections
        ch[q][x] = ch[p][!x];
        if (ch[q][x]) par(ch[q][x]) = q;
        ch[p][!x] = q; par(q) = p;
        par(p) = g;
        if (g) ch[g][q == rc(g)] = p;
        pull_up(q);
        pull_up(p);
        return ;
    }
    int pre(int p)
    {
        if (!lc(p)) {
            while (p == lc(par(p)))
                p = par(p);
            p = par(p);
        } else {
            p = lc(p);
            while (rc(p))
                p = rc(p);
        }
        return p;
    }
    int suc(int p)
    {
        if (!rc(p)) {
            while (p == rc(par(p)))
                p = par(p);
            p = par(p);
        } else {
            p = rc(p);
            while (lc(p))
                p = lc(p);
        }
        return p;
    }
    void splay(int p, int t)
    {
        for (int q = 0; (q = par(p)) && q != t; rotate(p))
            if (par(q) && par(q) != t)
                rotate((p == lc(q)) == (q == lc(par(q))) ? q : p);
        if (t == 0) root = p;
        return ;
    }
    int find(int x)
    {
        int p = root;
        while (x > 0 && p) {
            push_down(p);
            if (x <= size[lc(p)]) {
                p = lc(p);
                continue;
            } x -= size[lc(p)];
            if (x <= 1) {
                return p;
            } x -= 1;
            p = rc(p);
        }
        return 0;
    }
    int find_bin_geq(lli v)
    {
        // first p s.t. val[p] >= v
        int p = root;
        int q = 2;
        while (p) {
            push_down(p);
            if (val[p] == v) {
                return p;
            } else if (val[p] < v) {
                p = rc(p);
            } else if (val[p] > v) {
                q = val[p] < val[q] ? p : q;
                p = lc(p);
            }
        }
        return q;
    }
    int find_bin_leq(lli v)
    {
        // last p. s.t. val[p] <= v
        int p = root;
        int q = 1;
        while (p) {
            push_down(p);
            if (val[p] == v) {
                return p;
            } else if (val[p] > v) {
                p = lc(p);
            } else if (val[p] < v) {
                q = val[p] > val[q] ? p : q;
                p = rc(p);
            }
        }
        return q;
    }
    void insert(int x, lli v)
    {
        int lp = find(x + 1), rp = suc(lp);
        splay(rp, 0);
        splay(lp, root);
        int c = makenode(lp, v);
        rc(lp) = c;
        pull_up(lp);
        pull_up(rp);
        return ;
    }
    void insert_bin(lli v)
    {
        int p = root;
        int q = 0;
        while (p) {
            push_down(p);
            if (v <= val[p]) {
                if (!lc(p)) {
                    lc(p) = makenode(p, v);
                    q = lc(p);
                    break;
                }
                p = lc(p);
            } else {
                if (!rc(p)) {
                    rc(p) = makenode(p, v);
                    q = rc(p);
                    break;
                }
                p = rc(p);
            }
        }
        splay(q, 0);
        return ;
    }
    void remove(int l, int r)
    {
        int lp = find(l - 1 + 1), rp = find(r + 1 + 1);
        splay(rp, 0);
        splay(lp, root);
        int c = rc(lp);
        par(c) = 0;
        rc(lp) = 0;
        pull_up(lp);
        pull_up(rp);
        n -= r - l + 1;
        return ;
    }
    lli query(int l, int r, int mode)
    {
        // mode = 1: count, 2: sum, 3: min, 4: max
        if (l > r) {
            if (mode == 1)
                return 0;
            if (mode == 2)
                return 0;
            if (mode == 3)
                return infinit;
            if (mode == 4)
                return - infinit;
        }
        int lp = find(l - 1 + 1), rp = find(r + 1 + 1);
        splay(rp, 0);
        splay(lp, root);
        int p = rc(lp);
        if (mode == 1)
            return size[p];
        else if (mode == 2)
            return vsm[p];
        else if (mode == 3)
            return vmn[p];
        else if (mode == 4)
            return vmx[p];
        return 0;
    }
    lli query_bin(lli lb, lli rb, int mode)
    {
        // mode = 1: count, 2: sum, 3: min, 4: max
        if (lb > rb) {
            if (mode == 1)
                return 0;
            if (mode == 2)
                return 0;
            if (mode == 3)
                return infinit;
            if (mode == 4)
                return - infinit;
        }
        int lp = find_bin_leq(lb - 1), rp = find_bin_geq(rb + 1);
        printf("found %d %d\n", lp, rp);
        splay(rp, 0);
        splay(lp, root);
        int p = rc(lp);
        if (mode == 1)
            return size[p];
        else if (mode == 2)
            return vsm[p];
        else if (mode == 3)
            return vmn[p];
        else if (mode == 4)
            return vmx[p];
        return 0;
    }
    void modify_add(int l, int r, lli v)
    {
        int lp = find(l - 1 + 1), rp = find(r + 1 + 1);
        splay(rp, 0);
        splay(lp, root);
        int p = rc(lp);
        lazyadd[p] += v;
        val[p] += v;
        vsm[p] += size[p] * v;
        vmn[p] += v;
        vmx[p] += v;
        pull_up(lp);
        pull_up(rp);
        return ;
    }
    void modify_swp(int l, int r)
    {
        int lp = find(l - 1 + 1), rp = find(r + 1 + 1);
        splay(rp, 0);
        splay(lp, root);
        int p = rc(lp);
        lazyswp[p] ^= 1;
        return ;
    }
    void init(void)
    {
        n = ncnt = 0;
        root = makenode(0, - infinit);
        rc(root) = makenode(root, infinit);
        pull_up(rc(root));
        pull_up(root);
        return ;
    }
} splay;
