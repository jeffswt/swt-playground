---
title: 中国剩余定理
problems: []
depends: [euclid_gcd]
---

# Target

给定以下方程组，求 $x$ 的最小非负整数解：
$$
\begin{cases}
x \equiv a_1 \mod m_1\\
x \equiv a_2 \mod m_2\\
x \equiv a_3 \mod m_3\\
\ldots\\
x \equiv a_n \mod m_n
\end{cases}
$$
其中 $m_1, m_2, \ldots, m_n$ 两两互质。

进一步地，当 $m_1, m_2, \ldots, m_n$ 两两不一定互质时，求解对应的 $x$。

# Complexity

* Space:
  * Worst Case: $O(n)$
  * Amortized: $O(n)$
  * Best Case: $O(n)$
* Time:
  * Worst Case: $O(n \log m_i)$
  * Amortized: $O(n \log m_i)$
  * Best Case: $O(n)$

# Solution

记 $M$ 为所有 $m_i$ 的最小公倍数，则应有：
$$
M = \prod_{i=1}^{n} m_i
$$
又记 $t_i$ 为同余方程 $\frac{M}{m_i} t_i \equiv 1 \mod m_i$ 的最小非负整数解，则存在所求的解 $x$ 满足：
$$
x = \sum_{i=1}^{n}a_i \frac{M}{m_i}t_i
$$
通解则为 $x^* = x + k M$。

进一步地，推广到当 $m_1, m_2, \ldots, m_n$ 不一定两两互质的条件下，我们假定前 $n-1$ 个同余方程已经被求解，且此时已有 $M = \prod_{i=1}^{n-1} m_i$，$n$ 个同余方程构成的方程组的解应为：
$$
t_n M \equiv a_n - x \mod m_n
$$
我们可以调用扩展欧几里得算法求得此同余方程。本质上，扩展中国剩余定理就是求解 $n$ 次扩展欧几里得。

# Invocation

* `lli crt_solve(lli[] a, lli[] m, int n)`：求解由 $n$ 个方程构成的同余方程组，其中保证 $m$ 内整数两两互质，返回最小的非负整数解 $x$。
* `lli excrt_solve(lli[] a, lli[] m, int n)`：求解由 $n$ 个方程构成的同余方程组，其中 $m$ 内整数不一定两两互质，返回最小的非负整数解 $x$。若该方程组无解，返回 $-1$。

