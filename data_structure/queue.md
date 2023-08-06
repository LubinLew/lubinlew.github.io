# 队列

先入先出(FIFO)

### 用队列实现栈

> https://leetcode.com/problems/implement-stack-using-queues/
> 
> https://www.lintcode.com/problem/494/description

解题思路1(双队列)：插入时，插入到临时队列 q2（q2 总是空的），之后将 q1 全部弹出插入 q2，之后将 q2 全部弹出 插入 q1，那么 q1 中的顺序就是栈的顺序，q2 变成空的。

> 插入第一个元素 a，q2 = a，q1 = empty `=>` q1 = a, q2 = empty
> 
> 插入第二个元素 b，q2 = b，q1 = a `=>` q2 = b, a   q1 = empty `=>` q1 = b, a  q2 = empty

```cpp
class MyStack {
public:
    MyStack() {

    }

    void push(int x) {
        q2.push(x);
        while (!q1.empty()) {//q2 = x + q1
            q2.push(q1.front());
           q1.pop();
        }
        while (!q2.empty()) {//q1 = q2, q2 = nil
            q1.push(q2.front());
            q2.pop();
        }
    }

    int pop() {
        int ret = q1.front();
        q1.pop();
        return ret;
    }

    int top() {
        return q1.front();
    }

    bool empty() {
        return q1.empty();
    }

private:
    queue<int> q1;
    queue<int> q2;
};
```

解题思路2(单队列)：每次插入队列后，将之前的元素全部弹出再插入回来

> 插入元素 a， q = a
> 
> 插入元素 b， q = a，b
> 
> 弹出 b 之前的元素 q = b， 再重新插入 q = b，a
> 
> 插入元素 c，q = b，a，c
> 
> 弹出 c 之前的元素 q = c，再重新插入 q = c ，b，a

```cpp
class MyStack {
public:
    MyStack() {

    }

    void push(int x) {
        q.push(x);
        size_t length = q.size()-1;
        for (size_t i = 0; i < length; i++) {
           q.push(q.front());
            q.pop();
        }
    }

    int pop() {
        int ret = q.front();
        q.pop();
        return ret;
    }

    int top() {
        return q.front();
    }

    bool empty() {
        return q.empty();
    }

private:
    queue<int> q;
};
```
