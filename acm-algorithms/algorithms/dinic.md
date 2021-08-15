---
title: Dinic 最大流
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

在最大流的题目中，图被称为网络，而每条边的边权被称作流量。我们可以把最大流问题想象成这样：源点有一个水库有无限吨水，汇点也有一个水库，希望得到最多的水。有一些水管，单位时间内可以输水 $e(i, j)$ 吨。

正式地讲，最大流是指网络中满足弧流量限制条件和平衡条件且具有最大流量的可行流。

定义前向弧为和有向边方向相同的弧，反之称之为后向弧。

定义增广路为一条链，其中链上的前向弧都不饱和，后向弧都非零，从源点开始汇点结束。

增广过程为寻找一条增广路的过程。Dinic 算法的本质就是不断寻找增广路直到找不到为止。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限。
* `lli infinit`：规定的无限大流量。
* `void add_edge(int u, int v, lli flow)`：添加一条 $u \rightarrow v$，流量为 $flow$ 的有向边。
* `lli eval()`：计算该有向图的最大流。
* `void init(int n, int s, int t)`：初始化点数为 $n$，源点为 $s$，汇点为 $t$ 的图。

