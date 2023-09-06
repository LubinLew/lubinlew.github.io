# 动态数组 vector

## 功能函数

### 增

#### 在尾部插入 push_back 一个元素

```cpp
void push_back( const T& value );           //(until C++20)
constexpr void push_back( const T& value ); //(since C++20)

void push_back( T&& value );                //(since C++11, until C++20)) 
constexpr void push_back( T&& value );      //(since C++20)
```

> 该操作会增加其 size(), 如果 size() 大于了其 capacity(), 会导致重新分配(reallocation), 这时所有的迭代器(包含 end()) 和 所有对元素的引用都是无效的, 否则只有 end() 无效。

### 删

### 改

### 查