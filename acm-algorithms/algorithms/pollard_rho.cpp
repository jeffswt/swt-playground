
void pollard_rho(lli n, vector<lli>& factors)
{
    if (is_prime(n)) {
        factors.push_back(n);
        return ;
    }
    lli a, b, c, d;
    while (true) {
        c = rand() % n;
        a = b = rand() % n;
        b = (fastmul(b, b, n) + c) % n;
        while (a != b) {
            d = a - b;
            d = gcd(abs(d), n);
            if (d > 1 && d < n) {
                pollard_rho(d, factors);
                pollard_rho(n / d, factors);
                return ;
            }
            a = (fastmul(a, a, n) + c) % n;
            b = (fastmul(b, b, n) + c) % n;
            b = (fastmul(b, b, n) + c) % n;
        }
    }
    return ;
}

vector<lli> pollard_rho(lli n)
{
    lli tmp = n;
    vector<lli> factors;
    pollard_rho(n, factors);
    sort(factors.begin(), factors.end());
    return factors;
}
