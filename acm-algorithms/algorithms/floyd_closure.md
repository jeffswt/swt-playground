---
title: Floyd-Warshall 传递闭包
problems: [poj3660]
depends: []
---

# Target

给定 $n$ 个点的有向图 $G = (V, E)$，求 $V$ 上任意两点之间的连通性。

# Complexity

* Space:
  * Worst Case: $O(n^3)$
  * Amortized: $O(n^3)$
  * Best Case: $O(n^3)$
* Time:
  * Worst Case: $O(n^2)$
  * Amortized: $O(n^2)$
  * Best Case: $O(n^2)$

# Solution

松弛操作：对于边 $e(i, j) \in E$，$conn_{s,j} := conn_{s,j} \lor (conn_{s,i} \land conn_{i,j})$

用边 $(i, k)$, $(k, j)$ 松弛边 $(i, j)$，循环方式从外到内分别为 $k, i, j$。

# Invocation

* `int maxn`：点的数量上限。
* `bool[][] conn`：点 $i$ 到点 $j$ 是否存在路径。
* `void add_edge(int u, int v)`：添加一条 $u \rightarrow v$ 的有向边。
* `void eval()`：计算 $n$ 个点中任意两个点间的连通性。
* `void init(int n)`：初始化点数为 $n$ 的图。

