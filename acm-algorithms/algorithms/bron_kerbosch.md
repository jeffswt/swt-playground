---
title: Bron-Kerbosch 最大团算法
problems: [poj1419]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的无向图 $G = (V, E)$，求无向图的最大全连通分量。

最大独立集：取 $V' \subseteq V$ 且 $|V'| = k$，同时 $V'$ 内点两两无连接。

最大团：取 $V' \subseteq V$ 且 $|V'| = k$，同时 $V'$ 内点两两相连，也作全连通分量。

# Complexity

* Space:
  * Worst Case: $O(n^2)$
  * Amortized: $O(n^2)$
  * Best Case: $O(n^2)$
* Time:
  * Worst Case: $O(3^\frac{n}{3})$
  * Amortized: $O(3^\frac{n}{3})$
  * Best Case: $O(n^2)$

# Solution

一个无向图的最大独立集为其补图的最大团。

补图：$G' = (V, U \setminus E)$ 是 $G$ 的补图，即边联通状态完全相反。

暴力搜索所有团，适当剪枝。

一个图的最大团数量不会超过 $3^\frac{n}{3}$ 个。

# Invocation

* `int maxn`：点的数量上限。
* `void add_edge(int u, int v)`：添加一条 $u \leftrightarrow v$ 的有向边。
* `void remove_edge(int u, int v)`：删除原来在图中的一条 $u \leftrightarrow v$ 的无向边。
* `int[] eval()`：计算该无向图的最大团大小，及其点构成。
* `void init(int n, bool state)`：对任意的边 $(u, v)$，初始其状态为 $state$。

