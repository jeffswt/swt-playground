View the contents of an N-d array in another perspective without changing its contents. This only changes the order in which elements are indexed, while the data themselves stay in place.

```C
int x[3][2][2];
int cnt = 0;
for (int i = 0; i < 3; i++)
    for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++)
            x[i][j][k] = ++cnt;
// Transpose!
int (*p)[2][3] = (int(*)[2][3])(x);  // auto p, if you're in c++
cnt = 0;
for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
        for (int k = 0; k < 3; k++)
            printf("[%d, %d, %d] = %d\n", i, j, k, p[i][j][k]);
```

Results:

```
[0, 0, 0] = 1
[0, 0, 1] = 2
[0, 0, 2] = 3
[0, 1, 0] = 4
[0, 1, 1] = 5
[0, 1, 2] = 6
[1, 0, 0] = 7
[1, 0, 1] = 8
[1, 0, 2] = 9
[1, 1, 0] = 10
[1, 1, 1] = 11
[1, 1, 2] = 12
```

