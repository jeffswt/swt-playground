---
title: Tarjan 强连通分量
problems: [poj1236, poj2186]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的有向图 $G = (V, E)$，求 $G$ 的强连通分量。

强连通分量：该子图内任意两点间总存在一条仅由子图内边构成的路径。

# Complexity

* Space:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$
* Time:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$

# Solution

记数组 $dfn_i$ 为点 $i$ 被 dfs 到的次序编号（时间戳），$low_i$ 为 $i$ 和 $i$ 的子树能够追溯到的最早的堆栈中的节点的时间戳。维护一个堆栈用于储存要处理的强连通分量。具体做法见代码。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：无向边的数量上限。
* `int[] belong`：点 $i$ 属于第 $belong_i$ 个强连通分量。
* `int[] bsize`：第 $i$ 个强连通分量的大小为 $bsize_i$。
* `void add_edge(int u, int v)`：添加一条 $u \rightarrow v$ 的有向边。
* `int eval()`：求图 $G$ 的强连通分量，并返回强连通分量的个数。
* `void init(int n)`：初始化点数为 $n$ 的图。

