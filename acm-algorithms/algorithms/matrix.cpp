
typedef llf typ;

struct matrix
{
protected:
    int n, m;
    typ arr[maxn][maxn];
public:
    matrix(void)
    {
        n = m = 1;
        arr[0][0] = 0;
    }
    matrix(int n, int m)
    {
        this->n = n, this->m = m;
        rep(i, 0, n - 1)
            rep(j, 0, m - 1)
                arr[i][j] = 0;
    }
    typ& operator () (int i, int j)
    {
        if (i < 1 || i > n || j < 1 || j > m)
            throw std::out_of_range("invalid indices");
        return arr[i - 1][j - 1];
    }
    matrix operator () (int top, int bottom, int left, int right);
    friend matrix eye(int);
    friend matrix zeros(int, int);
    friend matrix ones(int, int);
    friend matrix operator + (const matrix&, const matrix&);
    friend matrix operator - (const matrix&, const matrix&);
    matrix& operator += (const matrix&);
    matrix& operator -= (const matrix&);
    friend matrix prod(const matrix&, const matrix&);
    friend matrix prod(const matrix&, const typ);
    friend matrix operator * (const matrix&, const matrix&);
    friend matrix operator * (const matrix&, const typ);
    friend matrix operator * (const typ, const matrix&);
    matrix& operator *= (const matrix&);
    matrix& operator *= (const typ);
    friend matrix pow(const matrix& a, lli b);
    friend matrix transpose(const matrix&);
    friend typ det_def(const matrix&);
    friend typ det(const matrix&);
    friend matrix inv(const matrix&);
    friend ostream& operator << (ostream&, const matrix&);
};

matrix matrix::operator () (int top, int bottom, int left, int right)
{
    if (top < 1 || top > n || bottom < 1 || bottom > n || top > bottom ||
        left < 1 || left > m || right < 1 || right > m || left > right)
        throw std::out_of_range("invalid indices");
    int h = bottom - top + 1, w = right - left + 1;
    matrix mat(h, w);
    rep(i, 0, h - 1)
        rep(j, 0, w - 1)
            mat.arr[i][j] = arr[top - 1 + i][left - 1 + j];
    return std::move(mat);
}

matrix eye(int n)
{
    matrix mat(n, n);
    rep(i, 0, n - 1)
        mat.arr[i][i] = 1;
    return std::move(mat);
}

matrix zeros(int n, int m = -1)
{
    if (m == -1)
        m = n;
    matrix mat(n, m);
    return std::move(mat);
}

matrix ones(int n, int m = -1)
{
    if (m == -1)
        m = n;
    matrix mat(n, m);
    rep(i, 0, n - 1)
        rep(j, 0, m - 1)
            mat.arr[i][j] = 1;
    return std::move(mat);
}

matrix operator + (const matrix& a, const matrix& b)
{
    matrix c(a.n, a.m);
    if (b.n != c.n || b.m != c.m)
        throw std::domain_error("nonconformant arguments");
    rep(i, 0, c.n - 1)
        rep(j, 0, c.m - 1)
            c.arr[i][j] = a.arr[i][j] + b.arr[i][j];
    return std::move(c);
}

matrix operator - (const matrix& a, const matrix& b)
{
    matrix c(a.n, a.m);
    if (b.n != c.n || b.m != c.m)
        throw std::domain_error("nonconformant arguments");
    rep(i, 0, c.n - 1)
        rep(j, 0, c.m - 1)
            c.arr[i][j] = a.arr[i][j] - b.arr[i][j];
    return std::move(c);
}

matrix& matrix::operator += (const matrix& b)
{
    if (b.n != n || b.m != m)
        throw std::domain_error("nonconformant arguments");
    rep(i, 0, n - 1)
        rep(j, 0, m - 1)
            arr[i][j] = arr[i][j] + b.arr[i][j];
    return *this;
}

matrix& matrix::operator -= (const matrix& b)
{
    if (b.n != n || b.m != m)
        throw std::domain_error("nonconformant arguments");
    rep(i, 0, n - 1)
        rep(j, 0, m - 1)
            arr[i][j] = arr[i][j] - b.arr[i][j];
    return *this;
}

matrix prod(const matrix& a, const matrix& b)
{
    matrix c(a.n, a.m);
    if (a.n == b.n && a.m == b.m) {
        rep(i, 0, a.n - 1)
            rep(j, 0, a.m - 1)
                c.arr[i][j] = a.arr[i][j] * b.arr[i][j];
    } else if (a.n == b.n && b.m == 1) {
        rep(i, 0, a.n - 1)
            rep(j, 0, a.m - 1)
                c.arr[i][j] = a.arr[i][j] * b.arr[i][0];
    } else if (a.m == b.m && b.n == 1) {
        rep(i, 0, a.n - 1)
            rep(j, 0, a.m - 1)
                c.arr[i][j] = a.arr[i][j] * b.arr[0][j];
    } else if (b.n == 1 && b.m == 1) {
        rep(i, 0, a.n - 1)
            rep(j, 0, a.m - 1)
                c.arr[i][j] = a.arr[i][j] * b.arr[0][0];
    } else {
        throw std::domain_error("nonconformant arguments");
    }
    return std::move(c);
}

matrix prod(const matrix& a, typ b)
{
    matrix c(a.n, a.m);
    rep(i, 0, a.n - 1)
        rep(j, 0, a.m - 1)
            c.arr[i][j] = a.arr[i][j] * b;
    return std::move(c);
}

matrix operator * (const matrix& a, const matrix& b)
{
    if (a.m != b.n)
        throw std::domain_error("nonconformant arguments");
    matrix c(a.n, b.m);
    rep(i, 0, c.n - 1)
        rep(j, 0, c.m - 1) {
            c.arr[i][j] = 0;
            rep(k, 0, a.m - 1)
                c.arr[i][j] += a.arr[i][k] * b.arr[k][j];
        }
    return std::move(c);
}

matrix operator * (const matrix& a, const typ b)
{
    return std::move(prod(a, b));
}

matrix operator * (const typ a, const matrix& b)
{
    return std::move(prod(b, a));
}

matrix& matrix::operator *= (const matrix& b)
{
    if (m != b.n)
        throw std::domain_error("nonconformant arguments");
    *this = *this * b;
    return *this;
}

matrix& matrix::operator *= (const typ b)
{
    rep(i, 0, n - 1)
        rep(j, 0, m - 1)
            arr[i][j] = arr[i][j] * b;
    return *this;
}

matrix pow(const matrix& a, lli b)
{
    if (a.n != a.m)
        throw std::domain_error("nonconformant arguments");
    matrix res = eye(a.n), tmp = a;
    while (b > 0) {
        if (b & 1)
            res *= tmp;
        tmp *= tmp;
        b >>= 1;
    }
    return std::move(res);
}

matrix transpose(const matrix& a)
{
    matrix mat(a.m, a.n);
    rep(i, 0, a.n - 1)
        rep(j, 0, a.m - 1)
            mat.arr[j][i] = a.arr[i][j];
    return std::move(mat);
}

typ det_def(const matrix& a)
{
    if (a.n != a.m)
        throw std::domain_error("nonconformant arguments");
    int n = a.n;
    typ res = 0;
    static int vec[maxn];
    static bool vis[maxn];
    rep(i, 0, n - 1)
        vec[i] = vis[i] = 0;
    stack<pair<int, int>> dfs_stk;  // dfs stack
    rep_(i, n - 1, 0)
        dfs_stk.push(make_pair(1, i + 1));
    while (!dfs_stk.empty()) {
        pair<int, int> pr = dfs_stk.top();
        dfs_stk.pop();
        int depth = pr.first,
            p = abs(pr.second) - 1,
            rm = pr.second < 0;
        if (rm) {  // restore previous state
            vis[p] = false;
            vec[depth - 1] = 0;
            continue;
        } else if (depth == n) {  // final state
            vis[p] = true;
            vec[depth - 1] = p + 1;
            // calculate count inverse
            lli cnt_inv = 0;
            rep(i, 0, n - 1)
                rep(j, i + 1, n - 1)
                    if (vec[i] > vec[j])
                        cnt_inv += 1;
            // (-1) ^ cnt_inv(vec) * sum(arr[i][vec[i]])
            typ tmp = 0;
            tmp = cnt_inv % 2 == 1 ? -1 : 1;
            rep(i, 0, n - 1)
                tmp *= a.arr[i][vec[i] - 1];
            // sum(tmp)
            res += tmp;
            // restore previous state
            vis[p] = false;
            vec[depth - 1] = 0;
            continue;
        }
        // mark restoration state
        dfs_stk.push(make_pair(depth, - (p + 1)));
        vis[p] = true;
        vec[depth - 1] = p + 1;
        // children states
        rep_(i, n - 1, 0)
            if (!vis[i])
                dfs_stk.push(make_pair(depth + 1, i + 1));
    }
    return res;
}

typ det(const matrix& a)
{
    if (a.n != a.m)
        throw std::domain_error("nonconformant arguments");
    int n = a.n;
    matrix m(n, n);
    rep(i, 0, n - 1)
        rep(j, 0, n - 1)
            m.arr[i][j] = a.arr[i][j];
    // arrange rows
    typ comp = 1;  // compensate
    rep_(i, n - 1, 0) {
        // find non-zero row to dissipate other values
        int flag = -1;
        rep_(j, i, 0)
            if (m.arr[i][j] != 0) {
                flag = j;
                break;
            }
        if (flag == -1)
            return 0;
        // swap rows
        if (flag != i)
            rep(j, 0, n - 1)
                swap(m.arr[j][flag], m.arr[j][i]);
        // eliminate other rows
        rep(j, 0, i - 1) {
            typ pa = m.arr[i][j],
                pb = m.arr[i][i];
            comp *= pb;
            rep(k, 0, n - 1)
                m.arr[k][j] = m.arr[k][j] * pb - m.arr[k][i] * pa;
        }
    }
    // multiply diagonal elements
    typ res = 1;
    rep(i, 0, n - 1)
        res *= m.arr[i][i];
    // divide by compensate
    res /= comp;
    return res;
}

matrix inv(const matrix& a)
{
    if (a.n != a.m)
        throw std::domain_error("nonconformant arguments");
    int n = a.n;
    matrix b(n, n), tmp(n - 1, n - 1);
    typ det_a = det(a);
    if (det_a == 0)
        throw std::invalid_argument("not inversible");
    rep(i, 0, n - 1)
        rep(j, 0, n - 1) {
            rep(_i, 0, n - 2)
                rep(_j, 0, n - 2)
                    tmp.arr[_i][_j] = a.arr[_i < i ? _i : (_i + 1)]
                                           [_j < j ? _j : (_j + 1)];
            b.arr[j][i] = ((i + j) % 2 == 1 ? -1 : 1) * det(tmp) / det_a;
        }
    return std::move(b);
}

ostream& operator << (ostream& out, const matrix& mat)
{
    rep(i, 0, mat.n - 1) {
        out << "|";
        rep(j, 0, mat.m - 1)
            out << " " << mat.arr[i][j];
        out << " |";
        if (i < mat.n - 1)
            out << "\n";
    }
    return out;
}
