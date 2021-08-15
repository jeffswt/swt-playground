---
title: 线性筛
problems: []
depends: []
---

# Target

计算 $[1, n]$ 内的所有正整数的 $\varphi(i)$，以及筛出该区间内的质数。

# Complexity

* Space:
  * Worst Case: $O(n)$
  * Amortized: $O(n)$
  * Best Case: $O(n)$
* Time:
  * Worst Case: $O(n)$
  * Amortized: $O(n)$
  * Best Case: $O(n)$

# Solution

在朴素的 $O(n^2)$ 筛法上优化成为了 $O(n)$ 的欧拉筛，使每个数不会被筛两次，且保证所有合数被消除。

利用 $\varphi(x)$ 的积性函数性质可以在筛质数的过程中计算 $\varphi$ 函数。

# Invocation

* `int maxn`：筛选的质数的数量上限。

* `int maxp`：$[1, n]$ 内质数的数量上限。

* `void filter(int n)`：筛选出 $[1, n]$ 范围内的质数及其 $\varphi(i)$ 值。

* `bool[] isprime`：长度为 $maxn$ 的数组，代表该位置是否为质数。

* `int[] primes`：长度为 $maxp$ 的数组，$primes[0]$ 代表一共有多少质数，$primes[i]$ 为该范围内第 $i$ 个质数。$[1, n]$ 内质数的个数满足：
  $$
  primes[0] = \begin{cases}
  1229 & n = 10^4\\
  9592 & n = 10^5\\
  78498 & n = 10^6\\
  664579 & n = 10^7\\
  5761455 & n = 10^8
  \end{cases}
  $$

* `int[] phi`：长度为 $maxn$ 的数组，满足 $phi[i] = \varphi(i)$。

