---
title: Tarjan 边双连通分量
problems: [poj3352]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的无向图 $G = (V, E)$，求 $G$ 的边双连通分量。

割边：删掉该边以后无向连通图被分割成两个连通子图，也称作桥。

边双连通图：不存在割边的无向连通图，保证任意两个点间至少存在两条不含共同边的路径。

边双连通分量：一个无向图的极大边双联通子图。

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

记数组 $dfn_i$ 为点 $i$ 被 dfs 到的次序编号（时间戳），$low_i$ 为 $i$ 和 $i$ 的子树能够追溯到的最早的堆栈中的节点的时间戳。维护一个堆栈用于储存要处理的连通分量。两遍 dfs，第一次标记所有桥，第二次通过桥的标记求出分量。具体做法见代码。

边双连通图有以下性质：

* 在分量内的任意两个点总可以找到两条边不相同的路径互相到达。
* 若一个边被删除后形成两个边连通图，则该边为原图的桥。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：无向边的数量上限。
* `int[] belong`：点 $i$ 属于第 $belong_i$ 个边双连通分量。
* `int[] bsize`：第 $i$ 个边双连通分量的大小为 $bsize_i$。
* `edge[] bridges`：所有割边的点对，保证 $u < v$。
* `bool edge.is_bridge`：标记该边是否为桥。
* `void add_edge(int u, int v)`：添加一条 $u \leftrightarrow v$ 的无向边。
* `int eval()`：求图 $G$ 的边双连通分量，并返回边双连通分量的个数。
* `void init(int n)`：初始化点数为 $n$ 的图。

