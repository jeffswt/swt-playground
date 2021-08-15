---
title: SPFA 最短路
problems: [poj3259]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的有向图 $G = (V, E)$，$s \in V$ 以及每条边的长度，判定图是否存在负权回路，若无则求点 $s$ 到 $V$ 中任意一点的最短距离及满足距离最短的任意一条最短路径。

# Complexity

* Space:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$
* Time:
  * Worst Case: $O(n m)$
  * Amortized: $O(k m)$
  * Best Case: $O(k m)$

# Solution

松弛操作：对于边 $e(i, j) \in E$，$dist_j := min(dist_j, dist_i + len_e)$

写法类似 Dijkstra 算法，不使用堆优化，并保存一个 $qcnt$ 数组代表每个点进入队列的次数。另记一个数组 $inque$ 代表一个数是否在队列里。如果一个点重复入队超过 $n - 1$ 次，则原图必存在负权回路。

通过比较当前松弛点的距离和队首的距离选择是推入队首还是队末的方法被称为前向星优化。注意在某些情况下该优化方法会被出题人针对并被卡掉，这时需去除前向星优化使用朴素方法。

获得最短路径的方法已在 Dijkstra 模板中记录，此处不予冗述。

一般地，前向星优化能够加速 $1$ 到 $10$ 倍不等，取决于数据强度。在无负权回路的图下，考虑到玄学常数 $k$，强烈建议使用堆优化 Dijkstra 而不是 SPFA。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限。
* `lli infinit`：规定的无限远距离。
* `lli[] dist`：对于每个点 $i \in [1, n]$，点 $i$ 到源点 $s$ 的距离。
* `edge[] from`：从源点到点 $i$ 的最短路径上指向 $i$ 的边的地址。
* `void add_edge(int u, int v, lli len)`：添加一条 $u \rightarrow v$，长度为 $len$ 的有向边。
* `bool eval(int s)`：以 $s$ 为源点，计算到 $[1, n]$ 所有点的最短路。同时若函数返回 $false$ 则该图包含负权回路，反之则不存在负权回路。
* `void init(int n)`：初始化点数为 $n$ 的图。
