---
title: Tarjan 点双连通分量
problems: [poj1144]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的无向图 $G = (V, E)$，求 $G$ 的点双连通分量。

割点：删掉该点以后无向连通图被分割成两个连通子图。

点双连通图：不存在割点的无向连通图。

点双连通分量：一个无向图的极大点双联通子图。

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

记数组 $dfn_i$ 为点 $i$ 被 dfs 到的次序编号（时间戳），$low_i$ 为 $i$ 和 $i$ 的子树能够追溯到的最早的堆栈中的节点的时间戳。维护一个堆栈用于储存要处理的连通分量。若回溯时目标节点 $low_i$ 不小于当前点 $dfn$ 值，则不断出栈直到目标节点。具体做法见代码。

点双连通图有以下性质：

* 任意两点间至少存在两条点不重复的路径。
* 图中删去任意一个点都不会改变图的连通性。
* 若点双连通分量之间存在公共点，则这个点为原图的割点。
* 无向连通图中割点必定属于至少两个点双连通分量，非割点属于且恰属于一个。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：无向边的数量上限。
* `int[] belong`：点 $i$ 属于第 $belong_i$ 个点双连通分量。
* `int[] bsize`：第 $i$ 个点双连通分量的大小为 $bsize_i$。
* `bool[] is_cut`：标记点 $i$ 是否为割点。
* `void add_edge(int u, int v)`：添加一条 $u \leftrightarrow v$ 的无向边。
* `int eval()`：求图 $G$ 的点双连通分量，并返回点双连通分量的个数。
* `void init(int n)`：初始化点数为 $n$ 的图。

