# 优先队列

普通的队列是一种先进先出的数据结构，元素在队列尾追加，而从队列头删除。
在优先队列(priority queue)中元素被赋予优先级。
当访问元素时，具有最高优先级的元素最先删除。
优先队列具有优先级最高先出的性质。

通常采用堆或二叉树等数据结构来实现。

## C++ 语法

> https://en.cppreference.com/w/cpp/container/priority_queue
> 
> https://cplusplus.com/reference/queue/priority_queue/

```cpp
#include <queue>

template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class priority_queue;
```

类模板需要提供三个参数, 其中后两个拥有默认值

| 模板参数      | 说明                                                                                                                                                                                                                                   | 默认值        | 可选值                                                                                                                                           |
| --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ---------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| T         | 优先队列中存储元素的类型                                                                                                                                                                                                                         | -          |                                                                                                                                               |
| Container | 用于存储元素的底层容器类型;<br>必须是[序列容器](https://en.cppreference.com/w/cpp/named_req/SequenceContainer)<br>其迭代器必须符合[遗留随机访问迭代器](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator)<br>必须提供下面方法 `front()`/`push_back()`/`pop_back()` | vector<T>  | vector,deque                                                                                                                                  |
| Compare   | 提供[严格弱排序](https://www.boost.org/sgi/stl/StrictWeakOrdering.html)的比较类型                                                                                                                                                                | less (大顶堆) | [less](https://en.cppreference.com/w/cpp/utility/functional/less),[greater](https://en.cppreference.com/w/cpp/utility/functional/greater),自定义 |

常用成员函数

| 成员函数                                     | 说明           |
| ---------------------------------------- | ------------ |
| const value_type& **top**(void)          | 查询优先队列的第一个元素 |
| bool ***empty***(void)                   | 是否为空队列       |
| size_t ***size***(void)                  | 队列中元素个数      |
| void ***push***(const value_type& value) | 插入一个元素       |
| void ***pop***(void)                     | 删除第一个元素      |

例子:

```cpp
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int main(void)
{
	priority_queue<int> q1;
	q1.push(1);
	q1.push(2);
	q1.push(3);
	cout << "max heap top: " << q1.top() << endl;

	priority_queue<int, vector<int>, greater<int>> q2;
	q2.push(1);
	q2.push(2);
	q2.push(3);
	cout << "min heap top: " << q2.top() << endl;

	return 0;
}
```

### 寻找数据流中的中位数

> https://www.lintcode.com/problem/81/solution/16796



