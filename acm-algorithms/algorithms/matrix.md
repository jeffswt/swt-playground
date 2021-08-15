---
title: 矩阵
problems: []
depends: []
---

# Target

给定矩阵 $A, B$，对矩阵进行运算。

# Complexity

* Space:
  * Worst Case: $O(n^2)$
  * Amortized: $O(n^2)$
  * Best Case: $O(n^2)$
* Time:
  * Worst Case: $O(n^2)$
  * Amortized: $O(n^2)$
  * Best Case: $O(n^2)$
* Multiplication Time:
  * Worst Case: $O(n^3)$
  * Amortized: $O(n^3)$
  * Best Case: $O(n^3)$
* Determinant Time:
  * Worst Case: $O(n^3)$
  * Amortized: $O(n^3)$
  * Best Case: $O(n^3)$
* Inversion Time:
  * Worst Case: $O(n^5)$
  * Amortized: $O(n^5)$
  * Best Case: $O(n^5)$


# Solution

矩阵求行列式值方法：逐行消掉元素，仅保留上三角矩阵，记录扩倍的值，最后将主对角线上元素相乘除掉消掉元素过程中乘上的值。

矩阵求行列式的定义法：$|A| = \sum_{p_1, p_2, \ldots, p_n} (-1)^{a(p_1, p_2, \ldots, p_n)} \cdot a_{1,p_1} \cdot a_{2,p_2} \cdots a_{n,p_n}$，递归求或递推求均可。

矩阵求逆的方法：$A^{-1} = \frac{A^*}{|A|}$，其中 $A^*$ 为 $A$ 的增广矩阵，$A^*_{j,i}$ 为 $A$ 去除第 $i$ 行第 $j$ 列对应的代数余子式。

# Invocation

* `typedef typ`：矩阵内元素的类型。
* `int maxn`：矩阵的长宽上限。
* `typ operator () (int i, int j)`：寻址第 $i$ 行第 $j$ 列。
* `matrix eye(int n)`：生成 $n \times n$ 的单位矩阵。
* `matrix zeros(int n, int m)`：生成 $n \times m$ 的零矩阵。
* `matrix ones(int n, int m)`：生成 $n \times m$ 的各位均为 $1$ 的矩阵。
* `matrix A + matrix B = matrix`：求 $A + B$。
* `matrix A - matrix B = matrix`：求 $A - B$。
* `matrix A += matrix B`：求值 $A := A + B$。
* `matrix A -= matrix B`：求值 $A := A - B$。
* `matrix prod(matrix A, matrix B)`：若矩阵 $B$ 维度与 $A$ 相同，则求两个矩阵对应位置相乘得到的新矩阵 $A\ .^* B$；若 $B$ 为列向量，则 $A$ 的每一列与 $B$ 对应元素两两相乘；若 $B$ 为行向量，则 $A$ 的每一行与 $B$ 对应元素两两相乘；若 $B$ 为 $1 \times 1$ 矩阵，则 $A$ 每个元素均与 $B_{1,1}$ 相乘。
* `matrix prod(matrix A, typ b)`：求矩阵 $A$ 和常数 $b$ 的积 $b \cdot A$。
* `matrix A * matrix B = matrix`：求两个矩阵相乘 $A \times B$。
* `matrix A * typ b = matrix`：求矩阵 $A$ 和常数 $b$ 的积 $b \cdot A$。
* `typ a * matrix B = matrix`：求常数 $a$ 和矩阵 $$B 的积 $a \cdot B$。
* `matrix A *= matrix B`：求值 $A := A \times B$。
* `matrix A *= typ b`：求值 $A := b \cdot A$。
* `matrix pow(matrix A, lli b)`：求 $A^b$。
* `matrix transpose(matrix A)`：求矩阵 $A$ 的转置 $A^T$。
* `matrix det_def(matrix A)`：用定义法（$O(n^{n+1})$）求矩阵 $A$ 的行列式 $|A|$。
* `matrix det(matrix A)`：用高斯消元法求矩阵 $A$ 的行列式 $|A|$。
* `matrix inv(matrix A)`：求矩阵 $A$ 的逆。
