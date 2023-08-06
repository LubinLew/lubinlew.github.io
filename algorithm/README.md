# 算法

## 时间复杂度与空间复杂度

时间复杂度简单来说就是 对于给定的 n，该算法完成计算所需要执行语句的条数

> 这里的条数并不准确，指的是完成一个功能的语句块

空间复杂度简单来说就是 对于给定的 n，该算法完成计算所需要的额外存储空间

复杂度和 n 用一个函数形式表示，这个形式是 [Big O notation](https://en.wikipedia.org/wiki/Big_O_notation) 。



常见复杂度如下：

| 标记               | 复杂度   | 说明                        |
| ---------------- | ----- | ------------------------- |
| O(1)             | 常数复杂度 | 无论 n 是多少，计算花费的资源(时间或空间)固定 |
| O(log n)         | 对数复杂度 |                           |
| O(n)             | 线性复杂度 |                           |
| O(n<sup>2</sup>) | 平方复杂度 |                           |
| O(n<sup>3</sup>) | 立方复杂度 |                           |
| O(2<sup>n</sup>) | 指数复杂度 | 2不是固定的，可以是任何常数            |
| O(n!)            | 阶乘复杂度 |                           |

### 时间复杂度例子

#### O(1)

```cpp
int algorithm1(int n) {
    n = n % 10;  // 无论 n 是多少，都执行固定条数的语句,这里是一条

    return n;
}


int algorithm2(int n) {
    n = n * 2;  // 无论 n 是多少，都执行固定条数的语句,这里是2条
    n = n % 10;  

    return n;
}

int algorithm3(int n) {
    n = n + 1;  // 无论 n 是多少，都执行固定条数的语句，这里是4条
    n = n + 2;
    n = n + 3;
    n = n + 4;

    return n;  
}
```

#### O(n)

```cpp
int algorithm(int n) {
    int sum = 0;

    for (int i = 0; i < n; i++) {
        sum += i;
    }

    return sum;
}
```

> n 是多少，for 循环就要执行多少次

#### O(n<sup>2</sup>)

```cpp
int algorithm(int n) {
    int sum = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += i * j;
        }
    }

    return sum;
}
```

> n 是多少 就行 n<sup>2</sup> 次的计算
> 
> 例子的计算没什么实际意义。

#### O(n<sup>3</sup>)

```cpp
int algorithm(int n) {
    int sum = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                sum += i * j * k;
            }
        }
    }

    return sum;
}
```

> n 是多少 就行 n<sup>3</sup> 次的计算

#### O(log n)

```cpp
int algorithm(int n) {
    int sum = 0;

    for (int i = 0; i < n; i = i * 2) {
        sum += i;
    }

    return sum;
}
```

> 每次 for 循环 i 会乘2

#### O(2<sup>n</sup>)

```cpp
int algorithm(int n) {
    int sum = 0;

    int end = pow(2, n); // 2的 n 次幂
    for (int i = 0; i < end; i++) {
        sum += i;
    }

    return sum;
}
```

> 例子不是很好

#### O(n!)

> 没有好的例子

### 递归程序的时间复杂度

这个的计算需要 [主定理(master theorem)](https://en.wikipedia.org/wiki/Master_theorem_(analysis_of_algorithms)) 来计算和论证，非常复杂。

| 算法                  | 时间复杂度      |
| ------------------- | ---------- |
| 二分查找(binary search) | O(log n)   |
| 二叉树遍历               | O(n)       |
| 最优排序矩阵搜索            | O(n)       |
| 归并排序(merge soft)    | O(n log n) |
