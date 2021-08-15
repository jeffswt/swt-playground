
lli crt_solve(lli a[], lli m[], int n)
{
    lli res = 0, lcm = 1, t, tg, x, y;
    rep(i, 1, n)
        lcm *= m[i];
    rep(i, 1, n) {
        t = lcm / m[i];
        exgcd(t, m[i], x, y);
        x = ((x % m[i]) + m[i]) % m[i];
        res = (res + t * x * a[i]) % lcm;
    }
    return (res + lcm) % lcm;
}

lli excrt_solve(lli a[], lli m[], int n)
{
    lli cm = m[1], res = a[1], x, y;
    rep(i, 2, n) {
        lli A = cm, B = m[i], C = (a[i] - res % B + B) % B,
            gcd = exgcd(A, B, x, y),
            Bg = B / gcd;
        if (C % gcd != 0)
            return -1;
        x = (x * (C / gcd)) % Bg;
        res += x * cm;
        cm *= Bg;
        res = (res % cm + cm) % cm;
    }
    return (res % cm + cm) % cm;
}
