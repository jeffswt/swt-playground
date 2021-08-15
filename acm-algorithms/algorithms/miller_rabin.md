---
title: Miller-Rabin 质数判别法
problems: []
depends: [fast_exponentiation]
---

# Target

给定质数 $n$，判定 $n$ 是否为质数。

# Complexity

* Space:
  * Worst Case: $O(k)$
  * Amortized: $O(k)$
  * Best Case: $O(k)$
* Time:
  * Worst Case: $O(k \log^2 n)$
  * Amortized: $O(k \log^2 n)$
  * Best Case: $O(k \log^2 n)$

# Solution

Fermat 小定理：若 $p$ 为质数，则必有 $a^{p-1} \equiv 1 \mod p$。

反之，若有 $a^{p-1} \equiv 1 \mod p$，则 $p$ 大概率是质数，若 $p$ 为合数定义为 Carmichael 数。

二次探测：如果 $p$ 是一个质数，且 $0 \lt x \lt p$，则方程 $x^2 \equiv 1 \mod p$ 的解为 $x = 1$ 或 $x = p-1$。

采用多个质数多次对某数 $n$ 进行上述检测，若次数足够多可以确定 $n$ 是否为质数。经过古今中外无数勇士的贡献与检验，得到一组最少的质数表如下：
$$
p_i = \begin{cases}
2 & n \leq 2.04 \cdot 10^{3}\\
31, 73 & n \leq 9.08 \cdot 10^{6}\\
2, 7, 61 & n \leq 4.75 \cdot 10^{9}\\
2, 13, 23, 1662803 & n \leq 1.12 \cdot 10^{12}\\
2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 & n \leq 3.18 \cdot 10^{23}
\end{cases}
$$
在 `is_prime.samples` 可修改该质数表，其中 `samples[0]` 代表质数表的大小（上述复杂度分析中的常数 $k$ 也指代质数表的大小。需要注意的是，由于复杂度太高，Miller-Rabin 算法不应用于筛选质数。

# Invocation

* `bool miller_rabin_test(lli n, lli p)`：利用质数 $p$ 以一定概率检验 $n$ 是否是质数。
* `bool is_prime(lli n)`：利用事先确定的质数表确定地检验 $n$ 是否为质数。

