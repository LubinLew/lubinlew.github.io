# 函数

函数在Go中是第一公民(first class)，下面是其的一些主要特性

- 函数是一种类型，可以像其他类型一样使用，可以作为其他函数的参数和返回值，也可以直接调用
- 函数支持多返回值
- 函数支持闭包
- 函数支持可变参数
- 不支持默认值参数
- 不支持函数重载
- 不支持命名函数嵌套，但支持匿名函数嵌套

## 基本概念

### 函数的定义

函数是Go语言代码的基本构成单元，一个函数的定义由下面四个部分构成：

- 函数声明关键字 `func`
- 函数名
  - 名称需要符合命令规范，首字母大小写决定该函数在其他包的可见性，大写时其他包可见，小写时只有相同的包可以访问
- 参数列表，需要使用`()`包裹
  - 函数可以没有参数，但是括号不能省略；
  - 多个相邻的同类型参数可以使用简写（见下面例子函数D）
- 返回值列表，需要使用`()`包裹
  - 可以没有返回值，默认返回0；
  - 当返回值只有一个的时候且是非命令的参数时，括号可以省略;
  - 支持有名的返回值(见下面例子函数E）；
  - 多个相邻的同类型有名返回值可以简写(见下面例子函数F)；
  - 不支持有名返回值和无名返回值混合的方式(见下面例子函数G)
- 函数体，使用 `{}` 包裹，并且`{` 位于函数返回值同行的行尾

下面是一些例子：

```go
// 函数定义
func funcName(parameter-list)(result-list) {
    function-body
}

//没有参数和返回值的函数
func A() {
   ...
}

//没有参数，但是有一个返回值的函数
func B() int {
    ...
}

//有一个参数 和 一个返回值的函数
func C(a int) int {
    ...
}

//有两个参数和两个返回值的函数
//多个相邻的同类型参数可以使用简写，(a int， b int) 可以简写为 (a，b int)
func D(a, b int)(int, int) {
    ...
}

// 有名返回值
// sum 相当于函数体内最外层的局部变量，并被初始化为该类型的零值
// 最后函数可以不用 return sum，直接 return 也可以返回 sum, 但是 return 不可省略
//
func E(a, b int)(sum int) {
    sum = a + b
    return //不可省略
}

// 多个有名返回值
func F(a, b int)(sum1, sum2 int) {
    sum1 = a + b
    sum2 = (a + b) * 2
    return //不可省略
}

// 本例子为错误的示例
//syntax error: mixed named and unnamed parameters
func G(a, b int) (sum int, int) {
    ...
}
```

### 实参到形参的传递

Go 函数实参到形参的传递永远是值拷贝，即使传递的实参是指针类型，本质上仍然是指针值的拷贝。

> **实参和形参**：
> 
> - 实参(argument):  是实际的具体的值，例如调用函数时作为参数所传入值
> - 形参(parameter): 是一种占位符，例如函数定义或声明的参数
> 
> ```go
> // a, b 是 形参
> func add(a, b int) int {
>    return a + b
> }
> 
> // 1， 2 是实参
> c := add(1, 2)
> ```
> 
> **深拷贝和浅拷贝**：
> 
> - 浅拷贝：拷贝过程中是按字节复制的，对于指针型成员变量只复制指针本身，而不复制指针所指向的目标
> - 深拷贝：拷贝过程中，对于指针型成员变量重新为其申请空间

### 不定参数

Go 函数支持不定数据的形式参数，使用 `param ...type` 的语法格式。
其特点如下：

- 所有不定参数的类型必需是相同的
- 不定参数必须是最后一个参数
- 不定参数名在函数体内相当于切片，对切片的操作同样适合不定参数

```go
func sum(arr ...int)(sum int) {
    for _, v := range arr { //arr 相当于切片
        sum += v
    }

    return
}
```

- 切片可以作为参数传递给不定参数，但是切片名后需要加上`...`
- 数组不能作为参数传递给不定参数

```go
func main() {
    slice := []int{1,2,3,4}
    //array := [...]int{1,2,3,4} //不能使用数组

    sum(slice...)
}
```

- 形参为不定参数的函数和形参为切片的函数类型是不相同的

```go
package main

import "fmt"

//形参为不定参数的函数
func sumA(arr ...int) (sum int) {
    for _, v := range arr {
        sum += v
    }
    return
}

//形参为切片的函数
func sumB(arr []int) (sum int) {
    for _, v := range arr {
        sum += v
    }
    return
}

func main() {
    fmt.Printf("%T\n", sumA) //func(...int) int
    fmt.Printf("%T\n", sumB) //func([]int) int
}
```

### 函数签名

函数类型又叫函数签名，函数类型是指函数定义的首行去掉函数名、参数名 和 `{`。 即上面例子中使用 fmt.Printf("%T") 打印的结果。

两个函数类型相同的条件是：拥有相同的形参列表和返回值列表(列表元素的次序、个数和类型都相同)，形参名可以不同。以下两个函数的类型完全相同：

```go
func add(a, b int) int {return a + b}
func sub(x int, y int)(c int) {return x - y}
```

可以使用 `type` 定义函数类型。函数类型变量可以作为函数的参数或返回值。

```go
package main

func add(a, b int) int         { return a + b }
func sub(x int, y int) (c int) { return x - y }

//定义一个函数类型，上面的 add 和 sub 就是这个类型
type Op func(int, int) int

//函数的第一个参数是函数类型
func do(f Op, a, b int) int {
    return f(a, b) //调用通过参数传递的函数
}
func main() {
    a := do(add, 1, 2)
    b := do(sub, 1, 2)

    println("a =", a, "\nb =", b)

    //有名函数可以直接赋值给变量
    f := sub
    c = f(1,2)
    println("c =", c)
}
```

### 匿名函数

Go函数分为 有名函数 和  匿名函数。匿名函数可以看作函数字面量，所有直接使用函数类型变量的地方都可以由匿名函数代替。匿名函数可以直接赋值给函数变量，可以当作实参，也可以作为返回值，还可以直接调用。

```go
package main

import "fmt"

//匿名函数被直接赋值函数变量
var sum = func(a, b int) int {
    return a + b
}

//函数名作为函数返回值
func wrapper(op string) func(int, int) int {
    switch op {
    case "add":
        return func(a, b int) int {
            return a + b
        }
    case "sub":
        return func(a, b int) int {
            return a - b
        }
    default:
        return nil
    }
}

//匿名函数作为函数形参
func doprint(f func(int, int) int, a, b int) int {
    return f(a, b)
}

func main() {
    //匿名函数直接调用
    defer func() {
        if err := recover(); err != nil {
            fmt.Println(err)
        }
    }()

    sum(1, 2)

    //匿名函数作为实参
    doprint(func(x, y int) int {
        return x + y
    }, 1, 2)

    opFunc := wrapper("add")
    re := opFunc(2, 3)
    fmt.Printf("%d\n", re)
}
```

## 闭包
