# 栈

先入后出

### 有效的括号序列

> https://leetcode.com/problems/valid-parentheses/
> 
> https://www.lintcode.com/problem/423/

```cpp
class Solution {
public:
    bool isValid(string s) {
        map<char, char> m {{')', '(' },{'}', '{'},{']', '['}};
        stack<char> st;
        for (const auto& it : s) {
            auto search = m.find(it);
            if (search != m.end()) {
                if (st.empty() || search->second != st.top()) {
                    return false;
                } else {
                    st.pop();
                }
            } else {
                st.push(it);
            }
        }

        return st.empty();
    }
};
```

### 用栈实现队列

> https://leetcode.com/problems/implement-queue-using-stacks/
> 
> https://www.lintcode.com/problem/40/

原理：使用两个栈 S_IN 和 S_OUT，

- 当插入时，将 S_OUT 全部弹出插入 S_IN，再将新数据插入 S_IN

- 当弹出时，将 S_IN 全部弹出插入 S_OUT，再从 S_OUT 弹出一个数据

```cpp
class MyQueue {
public:
    MyQueue() {

    }

    void push(int x) {
        while (!out.empty()) { //将 S_OUT 全部弹出插入 S_IN
            in.push(out.top());
            out.pop();
        }
        in.push(x); //再将新数据插入 S_IN
    }

    int pop() {      
        int ret = peek();
        out.pop(); //再从 S_OUT 弹出一个数据

       return ret; 
    }

    int peek() {
        while (!in.empty()) { //将 S_IN 全部弹出插入 S_OUT
            out.push(in.top());
            in.pop();
        }

        int ret = out.top();
       return ret; 
    }

    bool empty() {
        return out.empty() && in.empty();
    }
private:
    stack<int> in;
    stack<int> out;
};
```
