
bool miller_rabin_test(lli n, lli k)
{
    if (fastpow(k, n - 1, n) != 1)
        return false;
    lli t = n - 1, tmp;
    while (t % 2 == 0) {
        t >>= 1;
        tmp = fastpow(k, t, n);
        if (tmp != 1 && tmp != n - 1)
            return false;
        if (tmp == n - 1)
            return true;
    }
    return true;
}

bool is_prime(lli n)
{
    if (n == 1 || (n > 2 && n % 2 == 0))
        return false;
    // lli samples[13] = { 12, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };
    lli samples[4] = { 4, 2, 7, 61 };
    rep(i, 1, samples[0]) {
        if (n == samples[i])
            return true;
        if (n > samples[i] && !miller_rabin_test(n, samples[i]))
            return false;
    }
    return true;
}
