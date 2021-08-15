---
title: Floyd-Warshall 最短路
problems: [hdu1690]
depends: []
---

# Target

给定 $n$ 个点的有向图 $G = (V, E)$ 以及每条边的长度，求 $V$ 中任意两点之间的最短距离。

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

松弛操作：对于边 $e(i, j) \in E$，$dist_{s,j} := min(dist_{s,j}, dist_{s,i} + len_e)$

用边 $(i, k)$, $(k, j)$ 松弛边 $(i, j)$，循环方式从外到内分别为 $k, i, j$。

事实上无论何种循环方式，只要执行三次 Floyd 即可保证结果正确。

# Invocation

* `int maxn`：点的数量上限。
* `lli infinit`：规定的无限远距离。
* `lli[][] dist`：点 $i$ 到点 $j$ 的最短路径长度。
* `void add_edge(int u, int v, lli len)`：添加一条 $u \rightarrow v$，长度为 $len$ 的有向边。
* `void eval()`：计算 $n$ 个点中任意两个点间的最短路径长度。
* `void init(int n)`：初始化点数为 $n$ 的图。

