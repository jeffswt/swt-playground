
lli fastmul_old(lli a, lli b, lli m)
{
    lli res = 0, tmp = a;
    while (b > 0) {
        if (b & 1)
            res = (res + tmp) % m;
        tmp = (tmp + tmp) % m;
        b >>= 1;
    }
    return res;
}

lli fastmul(lli a, lli b, lli m)
{
    a %= m, b %= m;
    return ((a * b - (lli)((long double)a / m * b + 1.0e-8) * m) + m) % m;
}

lli fastpow(lli a, lli b, lli m)
{
    lli res = 1, tmp = a;
    while (b > 0) {
        if (b & 1)
            res = res * tmp % m;
        tmp = tmp * tmp % m;
        b >>= 1;
    }
    return res;
}

lli fastpow_safe(lli a, lli b, lli m)
{
    lli res = 1, tmp = a;
    while (b > 0) {
        if (b & 1)
            res = fastmul(res, tmp, m);
        tmp = fastmul(tmp, tmp, m);
        b >>= 1;
    }
    return res;
}

