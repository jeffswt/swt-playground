---
title: 并查集
problems: []
depends: []
---

# Target

给定 $n$ 个集合 $\{1\}, \{2\}, \ldots, \{n\}$，在线完成以下操作：

* $Query(i, j)$：查询两个数 $i, j$ 是否在一个集合内
* $Merge(i, j)$：合并两个数 $i, j$ 所在集合

# Complexity

* Space:
  * Worst Case: $O(n)$
  * Amortized: $O(n)$
  * Best Case: $O(n)$
* $Query$ Time:
  * Worst Case: $O(\log(n))$
  * Amortized: $O(\alpha(n))$
  * Best Case: $O(1)$
* $Merge$ Time:
  * Worst Case: $O(\log(n))$
  * Amortized: $O(\alpha(n))$
  * Best Case: $O(1)$

# Solution

复杂度分析中的 $\alpha(n)$ 函数为反 Ackermann 函数，在一般数据范围内视作常数复杂度。

每个数记一个 $id[i]$ 标记，为该数所在的集合的标志。合并两个集合 $p, q$ 时，仅需设 $id[p] := q$ 即可。这样形成了一个树结构，但为了避免路径长度变成 $O(n)$ 复杂度，我们需要执行路径压缩。具体地就是在查询某个点 $p$ 的祖先 $q$ 时，将该条路径上所有点的标记 $id[i]$ 修改成 $q$。这样我们可以保证每个非祖先点被访问的次数与查询的次数之比不超过某个常数。进一步地，为防止不完整的路径压缩，我们将大小更小的集合合并到更大的集合上，即若 $size[p] \lt size[q]$，则有 $id[p] := q$，反之 $id[q] := p$。

在并查集上可维护多种值，例如集合的和、异或和等等。

**TODO：各种花式并查集技巧待填坑**

# Invocation

* `int find(int p)`：查询某点 $p$ 的祖先。
* `bool joined(int p, int q)`：询问两个点 $p, q$ 是否处在一个集合内，即 $Query$ 操作。
* `void join(int p, int q)`：将两个点 $p, q$ 所在集合合并，即 $Merge$ 操作。
