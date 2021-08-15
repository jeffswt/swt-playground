---
title: Prim 最小生成树
problems: [hdu1102]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的无向图 $G = (V, E)$，求图 $G$ 的最小生成树 $G' = (V, E' \subseteq E)$，且 $\sum_{e \in E'} |e|$ 最小。

# Complexity

* Space:
  * Worst Case: $O(n^2)$
  * Amortized: $O(n^2)$
  * Best Case: $O(n^2)$
* Time:
  * Worst Case: $O(n^2)$
  * Amortized: $O(n^2)$
  * Best Case: $O(n^2)$

# Solution

以任意点为起点，维护一个点集 $S$，初始为 $S = \{ 1 \}$。选择一个点 $p \notin S$ 使得在所有的满足 $a \in S, b \notin S$ 的点对中 $p$ 对应到某一个 $dist_{a, b}$ 最小的点对。随后将 $p$ 加入 $S$。显然这样的做法是对的，证法类同 Kruskal 算法。

注意 Fibonacci 堆的常数较大，所以邻接表写法的 Fibonacci 堆优化 Prim 的复杂度虽然是 $O(m + n \log n)$ 的，其运行速度将大大不如 $O((n + m) \log n)$ 的二叉堆优化 Prim。进一步地，堆优化 Prim 在稠密图上表现并不明显好于朴素的 Prim，且常数较大。故若非稠密图，尽量应采用 Kruskal 算法。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：无向边的数量上限。
* `lli infinit`：规定的无限远距离。
* `void add_edge(int u, int v, lli len)`：添加一条 $u \leftrightarrow v$，长度为 $len$ 的无向边。
* `void join(int u, int v)`：预先连接点 $u$ 和点 $v$。
* `lli eval()`：计算图 $G$ 的最小生成树的边权之和，在代码中对应位置修改可求得该生成树的构造。
* `void init(int n)`：初始化点数为 $n$ 的图。

