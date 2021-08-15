---
title: ISAP 最大流
problems: [hdu3549]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的有向图 $G = (V, E)$，每条边的权值（即流量），给定源点 $s$ 和汇点  $t$，求从点 $s$ 到点 $t$ 的最大流量。

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

ISAP，Improved Shortest Augmenting Path，从汇点开始反向 bfs 建立层次图，但是每次 dfs 的时候每个点的层次随着算法进行不断提高，这样就不用多次 bfs 重建层次图了。当 $s$ 的深度超过 $n$，则增广完毕，结束算法。

一般地，优化 ISAP 速度能比朴素 Dinic 快 $1$ 倍左右。此外，虽然 HLPP 拥有较优的渐进时间复杂度 $O(n^2 \sqrt{m})$，但是常数较大，在普通随机数据上表现也不如 ISAP。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限。
* `lli infinit`：规定的无限大流量。
* `void add_edge(int u, int v, lli flow)`：添加一条 $u \rightarrow v$，流量为 $flow$ 的有向边。
* `lli eval()`：计算该有向图的最大流。
* `void init(int n, int s, int t)`：初始化点数为 $n$，源点为 $s$，汇点为 $t$ 的图。

