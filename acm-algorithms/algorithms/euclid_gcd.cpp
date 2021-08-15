
lli gcd_iterative(lli a, lli b)
{
    if (a < b)
        swap(a, b);
    while (b != 0) {
        lli c = a % b;
        a = b;
        b = c;
    }
    return a;
}

lli gcd(lli a, lli b)
{
    return b == 0 ? a : gcd(b, a % b);
}

lli lcm(lli a, lli b)
{
    return a / gcd(a, b) * b;
}

lli exgcd(lli a, lli b, lli& x, lli& y)
{
    if (b == 0) {
        x = 1, y = 0;
        return a;
    }
    int q = exgcd(b, a % b, y, x);
    y -= lli(a / b) * x;
    return q;
}
