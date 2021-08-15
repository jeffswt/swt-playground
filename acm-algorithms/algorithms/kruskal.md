---
title: Kruskal 最小生成树
problems: [hdu3371]
depends: [disjoint_set]
---

# Target

给定 $n$ 个点 $m$ 条边的无向图 $G = (V, E)$，求图 $G$ 的最小生成树 $G' = (V, E' \subseteq E)$，且 $\sum_{e \in E'} |e|$ 最小。

# Complexity

* Space:
  * Worst Case: $O(m)$
  * Amortized: $O(m)$
  * Best Case: $O(m)$
* Time:
  * Worst Case: $O(m \log m)$
  * Amortized: $O(m \log m)$
  * Best Case: $O(n \log m)$

# Solution

我们可以贪心地选择最小的边，将它们连起来，如果它们本来不在一个连通块上的话。理由如下：倘若存在边 $e_1 = (u, v), e_2 = (v, w), e_3 = (u, w)$，且 $|e_1| < |e_2| < |e_3|$，则同样可以连起来的方法中总是以 $e_1$ 和 $e_2$ 优先。若我们舍弃了一个较短的边，则在达成条件的前提下总是比选择短边的方案要差。

如此我们用一个 $O(m \log m)$ 的排序来贪心，用并查集维护点联通，若存在事先已连接好的点则将其在并查集上预处理，即可。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：无向边的数量上限。
* `void add_edge(int u, int v, lli len)`：添加一条 $u \leftrightarrow v$，长度为 $len$ 的无向边。
* `void join(int u, int v)`：预先连接点 $u$ 和点 $v$。
* `lli eval()`：计算图 $G$ 的最小生成树的边权之和，在代码中对应位置修改可求得该生成树的构造。
* `void init(int n)`：初始化点数为 $n$ 的图。

