# 函数

---

## 定义

---

## 返回值

----

## 参数

### 按值传递

传递是实参的副本

```cpp
#include <iostream>

void func(int a)
{
	std::cout << a << std::endl;
}

int main(void)
{
	int value = 10;
	func(value);
	return 0;
}
```

### 传递指针

修改指针地址中内容, 原值也被修改

```cpp
#include <iostream>

void func(int *a)
{
	(*a)++;
}

int main(void)
{
	int value = 10;
	func(&value);
	std::cout << value << std::endl;
	return 0;
}
```

### 传递引用

修改引用别名的值, 原值也被修改

```cpp
#include <iostream>

void func(int& a)
{
	a++;
}

int main(void)
{
	int value = 10;
	func(value);
	std::cout << value << std::endl;
	return 0;
}
```

### 传递数组

数组名当参数传递的时候会退化为指针，一维数组长度没有实际意义, 所以下面的函数`func1/func2/func3`是等效的。

二维数组的第一维组长度没有意义, 第二维有意义,并且不可以省略.

```cpp
#include <iostream>

void func1(int* a, size_t len) {}
void func2(int a[], size_t len) {}
void func3(int a[10], size_t len) {}

void func4(int a[][3], size_t count) {}

int main(void)
{
	//一维数组
	int array[] = { 0,1,2,3,4,5,6,7,8,9 };
	func1(array, 10);
	func2(array, 10);
	func3(array, 10);

	//二维数组
	int arr2[2][3] = { {1,2,3},{4,5,6} };
	func4(arr2, 2);
}
	return 0;
}
```

如果不想让数组退化为指针，可以通过传递`数组的引用`来实现

#### 参数传递一维数组的引用

一维数组的引用写法为 `(&array)[N]` , 括号不可省略,否则就成了引用的数组,但是C++不支持引用的数组。

```cpp
#include <iostream>

void func(const int (&arr)[10])
{
	for (const int& el : arr) {
		std::cout << el << std::endl;
	}
}

int main(void)
{
	int array[10] = {0,1,2,3,4,5,6,7,8,9};
	func(array); 
	return 0;
}
```

#### 参数传递多维数组的引用

二维数组的引用写法为 `(&array)[][N]`

```cpp
#include <iostream>

void func(const int (&arr)[][10], size_t count)
{
	for (size_t i = 0; i < count; i++) {
		for (const int& el : arr[i]) {
			std::cout << el << std::endl;
		}
	}
}

int main(void)
{
	int array[2][10] = { {0,1,2,3,4,5,6,7,8,9}, {0,1,2,3,4,5,6,7,8,9} };
	func(array, 2);
	return 0;
}
```

### 参数的隐式转换

看下面的例子, 我们可以给一个double类型参数,传递一个整型值,这是没有问题的, 编译器常常会帮助隐式地在不同类型之间进行转换。

```cpp
#include <iostream>

void func1(double a) {}
void func2(std::string b) {}
void func3(std::string& c) {}

void func4(int a[][3], size_t count) {}

int main(void)
{
	func1(1);
	func2("hello");
	//func3("hello"); 错误
}

```


