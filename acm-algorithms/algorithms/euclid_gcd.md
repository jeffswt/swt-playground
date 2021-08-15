---
title: Euclid 算法
problems: []
depends: []
---

# Target

给定整数 $a, b$，计算 $\gcd(a, b)$。

给定整数 $a, b$，计算 $\DeclareMathOperator{lcm}{lcm}\lcm(a, b)$。

求解同余方程 $a x + b y = \gcd(a, b)$。

# Complexity

* Space:
  * Worst Case: $O(1)$
  * Amortized: $O(1)$
  * Best Case: $O(1)$
* Time:
  * Worst Case: $O(\log (a + b))$
  * Amortized: $O(\log (a + b))$
  * Best Case: $O(1)$

# Solution

若有 $r_i = r_{i+1} q_{i+1} + r_{i+2}$，且 $r_n = 0$，则有 $r_{i+2} = r_i \mod r_{i+1}$。由共 $n-2$ 个式子逆推可知任意的 $r_i$ 可整除 $r_n$，正确性易得证。

# Invocation

* `lli gcd_iterative(lli a, lli b)`：递推式写法的最大公因数，计算 $\gcd(a, b)$。
* `lli gcd(lli a, lli b)`：递归式写法的最大公因数，计算 $\gcd(a, b)$。
* `lli lcm(lli a, lli b)`：计算 $a, b$ 的最小公倍数。
* `lli exgcd(lli a, lli b, lli& x, lli &y)`：求解同余方程 $a x + b y = gcd(a, b)$，将方程结果存在 `x` 和 `y` 内，调用时传入引用。

