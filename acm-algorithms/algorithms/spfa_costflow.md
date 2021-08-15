---
title: SPFA 费用流
problems: [luogu3381]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的有向图 $G = (V, E)$，每条边的流量和代价，给定源点 $s$ 和汇点  $t$，求从点 $s$ 到点 $t$ 的最大流量，以及达成最大流量前提下的最小总花费。

# Complexity

* Space:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$
* Time:
  * Worst Case: $O(n^2 m)$
  * Amortized: $O(n^2 m)$
  * Best Case: $O(n + m)$

# Solution

SPFA 费用流每次求一条代价最小的增广路，然后将这条增广路上的最大流量求出，并去掉这条流量。进一步地，可以使用多路增广，用类似 Dinic 的写法加速每次 BFS 增广的流量。我们甚至可以借用 Dijkstra 写法中的优先队列来优化 BFS 速度。注意这里 `set` 去重的效率并不如 `priority_queue` 直接推入。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限。
* `lli infinit`：规定的无限大流量。
* `void add_edge(int u, int v, lli flow, lli cost)`：添加一条 $u \rightarrow v$，流量为 $flow$，代价为 $cost$ 的有向边。
* `<lli, lli> eval()`：计算该有向图的最大流和对应的最小费用。
* `void init(int n, int s, int t)`：初始化点数为 $n$，源点为 $s$，汇点为 $t$ 的图。

