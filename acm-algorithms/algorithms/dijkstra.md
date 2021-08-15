---
title: Dijkstra 最短路
problems: [hdu2544]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的有向图 $G = (V, E)$，$s \in V$ 以及每条边的长度（无负权回路），求点 $s$ 到 $V$ 中任意一点的最短距离及满足距离最短的任意一条最短路径。

# Complexity

* Space:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$
* Time:
  * Worst Case: $O((n + m) \log n)$
  * Amortized: $O((n + m) \log n)$
  * Best Case: $O((n + m) \log n)$

# Solution

松弛操作：对于边 $e(i, j) \in E$，$dist_j := min(dist_j, dist_i + len_e)$

朴素 Dijkstra 算法：从起点开始，松弛每一个可达的点，并依次递归处理这些被松弛的点。

堆优化，将距离源点近的点优先弹出队列，保证最大程度减小无用松弛的次数。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限。
* `lli infinit`：规定的无限远距离。
* `lli[] dist`：对于每个点 $i \in [1, n]$，点 $i$ 到源点 $s$ 的距离。
* `edge[] from`：从源点到点 $i$ 的最短路径上指向 $i$ 的边的地址。
* `void add_edge(int u, int v, lli len)`：添加一条 $u \rightarrow v$，长度为 $len$ 的有向边。
* `void eval(int s)`：以 $s$ 为源点，计算到 $[1, n]$ 所有点的最短路。
* `int[] get_path(int p)`：获取一个节点编号构成的列表，构成一条 $s$ 到 $p$ 的最短路径。
* `void init(int n)`：初始化点数为 $n$ 的图。

