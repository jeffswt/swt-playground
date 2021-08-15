---
title: 朴素质因数分解
problems: []
depends: [linear_sieve]
---

# Target

给定整数 $n$，求 $n$ 的所有质因子。

# Complexity

* Space:
  * Worst Case: $O(1)$
  * Amortized: $O(1)$
  * Best Case: $O(1)$
* Time:
  * Worst Case: $O(\frac{\sqrt{n}}{\log \sqrt{n}})$
  * Amortized: $O(\frac{\sqrt{n}}{\log \sqrt{n}})$
  * Best Case: $O(\frac{\sqrt{n}}{\log \sqrt{n}})$

# Solution

整数 $n$ 最多含有一个大于 $\sqrt{n}$ 的质因子，所以仅需暴力判别小于等于 $\sqrt{n}$ 的所有质数即可。

调用该方法前需先用线性筛求出一定范围内的所有质数。

# Invocation

* `lli[] factorize(lli n)`：质因数分解 $n$，将结果（可能重复地）按顺序放入结果中。例如若 $n = 2^3 \cdot 3$，则 $factors = [2, 2, 2, 3]$。

