
bool isprime[maxn];
int primes[maxp];
int phi[maxn];

void filter(int n)
{
    rep(i, 2, n)
        isprime[i] = true;
    isprime[1] = false;
    primes[0] = 0;
    phi[1] = 1;
    for (int i = 2; i <= n; i++) {
        if (isprime[i]) {
            primes[++primes[0]] = i;
            phi[i] = i - 1;
        }
        for (int j = 1; j <= primes[0] && i * primes[j] <= n; j++) {
            isprime[i * primes[j]] = false;
            if (i % primes[j] == 0) {
                phi[i * primes[j]] = phi[i] * primes[j];
                break;
            }
            phi[i * primes[j]] = phi[i] * (primes[j] - 1);
        }
    }
    return ;
}
