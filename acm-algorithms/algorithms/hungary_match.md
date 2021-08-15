---
title: 匈牙利算法
problems: [poj3041]
depends: []
---

# Target

给定 $n$ 个点 $m$ 条边的二分图 $G = (V, E)$，求二分图的最大匹配数。

# Complexity

* Space:
  * Worst Case: $O(n + m)$
  * Amortized: $O(n + m)$
  * Best Case: $O(n + m)$
* Time:
  * Worst Case: $O(n m)$
  * Amortized: $O(n m)$
  * Best Case: $O(n + m)$

# Solution

二分图 $G=(V, E)$ 的点集 $V$ 可以分割成互补的两个点集 $V_1, V_2$，且两个点集内分别不存在点集内连边，即 $\forall (u, v) \in E, u \in V_1, u \in V_2$。匈牙利算法试图去匹配任意一个 $V_2$ 内的点，如果该点没有被另一个 $V_1$ 内的点匹配，直接创建点对；反之，记原来已经匹配好的点对 $(u', v') s.t. u' \in V_1, v' \in V_2$，此时可以试图给 $u'$ 再分配一个 $V_2$ 中点 $v''$。

换言之，匈牙利算法本质上就是询问一个点对是否能找到其他可行匹配，可行则贪心匹配一对即可。

# Invocation

* `int maxn`：点的数量上限。
* `int maxm`：有向边的数量上限。
* `void add_edge(int u, int v)`：添加一条 $u \rightarrow v$ 的有向边。
* `lli eval()`：计算该二分图的匹配数。
* `void init(int n)`：初始化点数为 $n$ 的图。

