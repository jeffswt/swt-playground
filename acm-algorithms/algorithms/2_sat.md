---
title: 2-SAT 问题
problems: [poj3678]
depends: []
---

# Target

给定 $n$ 个布尔变量 $a_1, a_2, \ldots, a_n$，$m$ 个限定条件 $limit(f, i, j, v)$ 限制 $f(a_i, a_j) = v$。求该组限制条件是否可以达成，并给出一组解。

# Complexity

* Space:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$
* Time:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$

# Solution

将每个布尔变量拆成两个点 $a_i^p, a_i^n$，分别代表点 $i$ 的正值和负值。每一个布尔操作的断言 $f(a_i, a_j) = v$ 都可以转化成某两个拆开的点后的连接。在程序内表现为 `constrain(p, 0, q, 1)`，代表若变量 $a_p = 0$ 则 $a_q = 1$。跑一遍强连通分量，若一个点拆开的两个点在一个强连通分量中则出现矛盾，否则 dfs 所有点即可获得一组解。所有可能解的总数为 $2^{bcnt}$，其中 $bcnt$ 为强连通分量的个数。

# Invocation

* `int maxn`：点的数量上限。
* `void set_true(int p)`：$a_p = 1$
* `void set_false(int p)`：$a_p = 0$
* `void require_and(int p, int q)`：$a_p \land a_q = 1$
* `void require_or(int p, int q)`：$a_p \lor a_q = 1$
* `void require_nand(int p, int q)`：$a_p \land a_q = 0$
* `void require_nor(int p, int q)`：$a_p \lor a_q = 0$
* `void require_xor(int p, int q)`：$a_p \oplus a_q = 1$
* `void require_xnor(int p, int q)`：$a_p \oplus a_q = 0$
* `bool eval(int[] res)`：求解限制组，结果存入 $res$，若无解返回 `false`。
* `void init(int n)`：初始化点数为 $n$ 的限制组。
