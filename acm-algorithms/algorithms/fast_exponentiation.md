---
title: 快速幂
problems: [bzoj1008*, bzoj3142*]
depends: []
---

# Target

给定整数 $a, b$，计算 $a^b$。

# Complexity

* Space:
  * Worst Case: $O(1)$
  * Amortized: $O(1)$
  * Best Case: $O(1)$
* Time:
  * Worst Case: $O(\log b)$
  * Amortized: $O(\log b)$
  * Best Case: $O(\log b)$

# Solution

$$
a^b = \begin{cases}
a \cdot a^{b-1} & b \equiv 1 \mod 2\\
(a^{\left\lfloor \frac{b}{2} \right\rfloor})^2 & b \equiv 0 \mod 2
\end{cases}
$$

用递推方式书写保证运算次数恰为 $\log b$ 轮。

快速幂函数 $fastpow(a, b)$ 可以被拓展成以下形式：$f(a, b, \circ_1, \circ_2)$，其中 $a \circ_1 b \equiv a + b$，$a \circ_2 b = a^b$。一般地，运算符 $\circ_1, \circ_2$ 满足 $\langle \mathbb{Z}, \circ_1\rangle$ 和 $\langle \mathbb{Z}, \circ_2\rangle$ 均为半群，且 $\underbrace{a \circ_1 a \circ_1 \ldots \circ_1 a}_{b个a} = a \circ_2 b$。为防止溢出，我们有时使用快速幂的一个变种 “快速乘”。注意使用快速乘保证不溢出的快速幂时间复杂度增长到了 $O((\log b)^2)$。

另一种快速乘使用了 `long double` 保证精度，在确保其精度正确性的前提下可代替非模意义下的快速乘。经过粗略检验该方法的出错率应当小于 $10^{-8}$。

# Invocation

* `lli fastmul_old(lli a, lli b, lli m)`：用快速幂写法的 $O(\log b)$ 快速乘，计算 $a \cdot b \mod m$。
* `lli fastmul(lli a, lli b, lli m)`：利用 `long double` 作为中介运算器的快速乘，计算 $a \cdot b \mod m$。
* `lli fastpow(lli a, lli b, lli m)`：快速幂，计算 $a^b \mod m$。
* `lli fastpow_safe(lli a, lli b, lli m)`：适用于模数 $m \geq 2^{31}-1$ 时，为防止溢出，调用了安全的快速乘的快速幂，同样计算 $a^b \mod m$。

