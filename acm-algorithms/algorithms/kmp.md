---
title: Knuth-Morris-Pratt 字符串匹配
problems: [hdu1686, hdu2087]
depends: []
---

# Target

给定模板串 $str$ 和模式串 $patt$，求 $str$ 有多少个（相交或互不相交）子串与 $patt$ 相等。

记 $|str| = n, |patt| = m$。

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

记 $next_i$ 为前 $i$ 个字符中最长的长度 $k$，使得 $patt[1, k] = patt[i-k+1, n]$。由于 $next$ 数组可以在 $O(m)$ 时间内用递推法求出，且可以辅助跳过已经匹配过的后缀（前缀符合，所以下一个符合匹配条件的就直接跳到 $next_j$ 即可。

本模板内数组下标从 $0$ 开始。

# Invocation

* `int maxn`：字符串长度上限。
* `int[] match(bool overlap)`：用模式串 $patt$ 匹配模板串 $str$，参数 $overlap$ 决定匹配时是否允许匹配结果相互交叉。
* `void init_patt(int m, char[] patt)`：加载模式串。
* `void init_str(int n, char[] str)`：加载模板串。
