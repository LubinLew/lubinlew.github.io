# constexpr

`constexpr` 是 C++11 引入的，一方面是为了引入更多的编译时计算能力，另一方面也是解决 C++98 的 `const` 的双重语义问题。

在 C 里面，`const` 很明确只有 **「只读」** 一个语义，不会混淆。C++ 在此基础上增加了 **「常量」** 语义，`const` 并未区分出编译期常量和运行期常量, 从而引出来一些奇怪的问题。

```cpp
const int a = 5;  // const 常量

int func(const int x) // const 只读变量
{
	return x + 1;
}

int main(void)
{
	auto b { func(a) };

	return 0;
}
```

C++11 把 **「常量」** 语义拆出来，交给新引入的 `constexpr` 关键字。`constexpr` 限定了是编译期常量。

```cpp
constexpr int a = 5; //常量

int func(const int x) //
{
	return x + 1;
}

int main(void)
{
	auto b { func(a) };

	return 0;
}
```

`constexpr` 也可用用来修饰函数, 但是函数返回值不一定是常量。编译器会根据情况进行使用的情景自动选择, 如下例中 `foo(5)` 可以在编译期间确定函数返回值, 那么 `foo(5)` 就是常量, `foo(i)` 因为 `i` 是变量, `foo(i)` 不能在编译期间确定返回值, 所以不是常量。那么 `constexpr` 修饰的函数就和普通函数一样了。不过，我们不必因此而写两个版本，所以如果函数体适用于`constexpr`函数的条件，可以尽量加上`constexpr` 关键字。

```cpp
constexpr int foo(int i)
{
	return i + 5;
}

int main()
{
	int i = 10;
	std::array<int, foo(5)> arr; // OK

	foo(i); // Call is Ok

	// But...
	std::array<int, foo(i)> arr1; // Error

	return 0;
}
```

> 检测 `constexpr` 函数是否产生编译时期值的方法很简单，就是利用`std::array`需要编译期常值才能编译通过的小技巧。这样的话，即可检测你所写的函数是否真的产生编译期常值了。

## 参考文章

[C++ const 和 constexpr 的区别？](https://www.zhihu.com/question/35614219)


