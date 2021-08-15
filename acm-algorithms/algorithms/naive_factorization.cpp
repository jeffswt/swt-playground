
vector<lli> factorize(lli n)
{
    lli tmp = n;
    vector<lli> factors;
    rep(i, 1, primes[0]) {
        lli p = primes[i];
        if (p * p > n || tmp <= 1)
            break;
        while (tmp % p == 0) {
            factors.push_back(p);
            tmp /= p;
        }
    }
    if (tmp > 1)
        factors.push_back(tmp);
    return factors;
}
