---
title: HLPP 最大流
problems: [luogu4722]
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
  * Worst Case: $O(n^2 \sqrt{m})$
  * Amortized: $O(n^2 \sqrt{m})$
  * Best Case: $O(n + m)$

# Solution

HLPP，Highest Label Preflow Push，是预流推进算法的改进版。所谓预流推进就是不停从有剩余流量的点往外推出流量，直到没有任何点可以继续往外推出流量为止。HLPP 是 Tarjan 得到的一个结论，每次推进标号 $level_i$ 最高的点可以保证时间复杂度下降到 $O(n^2 \sqrt{m})$。

在极端数据上，HLPP 的速度比普通 ISAP 要快 $3$ 倍以上。用 `vector<edge>` 的写法在事实上比 `edge*` 的链式前向星写法还要快 $5$ 倍。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限，使用 `vector` 写法的不需要此常量。
* `lli infinit`：规定的无限大流量。
* `void add_edge(int u, int v, lli flow)`：添加一条 $u \rightarrow v$，流量为 $flow$ 的有向边。
* `lli eval()`：计算该有向图的最大流。
* `void init(int n, int s, int t)`：初始化点数为 $n$，源点为 $s$，汇点为 $t$ 的图。

