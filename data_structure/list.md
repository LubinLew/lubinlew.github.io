# 链表

单链表

双链表

循环链表

## 时间复杂度

| 操作  | 时间复杂度 | 说明       |
| --- | ----- | -------- |
| 查找  | O(n)  | -        |
| 插入  | O(1)  | 不考虑查找的过程 |
| 删除  | O(1)  | 不考虑查找的过程 |

## 面试题

### 反转单链表

> https://leetcode.com/problems/reverse-linked-list/

```cpp
struct ListNode {
    int value;
    ListNode* next;
};

class Solution {
public:
    ListNode* reverseList(ListNode* head) {

        ListNode* prev = nullptr;
        ListNode* next = nullptr;

        while (head) {
            next = head->next;
            head->next = prev;
            prev = head;
            head = next;
        }

        return prev;
    }
};
```

### 反转单链表相邻两个节点

> https://leetcode.com/problems/swap-nodes-in-pairs/
> 
> https://www.lintcode.com/problem/451/

```cpp
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (!head || !head->next) {
            return head;
        }

        ListNode* node1 = nullptr;
        ListNode* node2 = nullptr;
        ListNode* lastNode1  = head;
        ListNode* newHead = head->next;

        while (head && head->next) {
            node1 = head;
            node2 = head->next;

            lastNode1->next = node2; //node1->next = node4
            lastNode1 = node1;

            head = node2->next; //head = node3
            node2->next = node1;
            node1->next = head; //node1->next = node3
        }

        return newHead;
    }
};
```

### 链表是否有环

> https://leetcode.com/problems/linked-list-cycle/
> 
> https://www.lintcode.com/problem/102/

- 方法1: 节点存 set，遍历同时查set，这样的话空间复杂度为 O(n)

- 方法2: 快慢指针(快指针一次走两步，慢指针一次走一步，如果相遇则有环)

- ```cpp
  class Solution {
  public:
      bool hasCycle(ListNode *head) {
          ListNode* fast = head;
          ListNode* slow = head;
  
          while (fast && fast->next && slow) {
              slow = slow->next;
              fast = fast->next->next;
              if (fast == slow) {
                  return true;
              }
          }
          return false;
      }
  };
  ```

### 找出带环链表中环的起点

> https://leetcode.com/problems/linked-list-cycle-ii/
> 
> https://www.lintcode.com/problem/103/
> 
> 解读: https://www.youtube.com/watch?v=kZP8Cij1fxk&ab_channel=HuifengGuan

- 方法1: 用 set 存储节点，这样的话空间复杂度为 O(n)

- 方法2: 仍借助上面的快慢指针方式

> 1. 快慢指针相遇，则相遇点必定在环中
> 
> 2. 设 链表起点到环起点的距离为 m， 环的长度为 n，在环上 p 距离相遇
> 
> 3. 相遇时快指针的路程是慢指针的2倍 m + k<sub>1</sub>n + p = 2(m + k<sub>2</sub>n + p) ,k<sub>1</sub>为快指针圈数， k<sub>2</sub>为慢指针圈数
> 
> 4. 简化后可得  (k<sub>1</sub>-2k<sub>2</sub>)n = m + p，可以得到 从 p 点移动距离 m 是整数倍的圈数
> 
> 5. 从链表起点移动到环起点距离是 m，从相遇点移动 m 也到环起点
>    
>    ![带环链表](resources/cycle_list.svg)

```cpp
class Solution {
public:
    ListNode * detectCycle(ListNode * head) {
        if (!head || !head->next) {
            return nullptr;
        }

        ListNode* fast = head;
        ListNode* slow = head;

        while (fast && fast->next && slow) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) {
                break;
            }
        }
        if (slow != fast) {
            return nullptr;
        }

        while (slow != head) {
            slow = slow->next;
            head = head->next;
        }

        return head;
    }
};
```

### 每 k 个节点一组翻转链表

> https://leetcode.com/problems/reverse-nodes-in-k-group/

### 链表的中点

寻找给定链表的中间(顺序)节点。

> https://leetcode.com/problems/middle-of-the-linked-list/
> 
> https://www.lintcode.com/problem/228/
> 
> lintcode:如果长度是偶数，则返回中间偏左的节点。
> 
> leecode :如果长度是偶数，则返回中间偏右的节点。(思路2 while条件去掉 fast->next->next)

思路1 O(n) : 随着链表长度的增加,中间节点一直是向后移动的,而且是长度为奇数时移动

```c
class Solution {
public:
    /**
     * @param head: the head of linked list.
     * @return: a middle node of the linked list
     */
    ListNode* middleNode(ListNode *head) {
        // write your code here
        ListNode* middle = head;
        int index = 0;

        while (head) {
            ++index;
            if ((index != 1) && (index % 2)) {
                middle = middle->next;
            }
            head = head->next;
        }

        return middle;
    }
};
```

思路2 O(n/2) : 使用快慢指针, 快指针到达末尾时, 慢指针恰好在中点

```cpp
class Solution {
public:
    /**
     * @param head: the head of linked list.
     * @return: a middle node of the linked list
     */
    ListNode* middleNode(ListNode *head) {
        // write your code here
        ListNode* fast = head;
        ListNode* slow = head;

        while (fast && fast->next && fast->next->next) {
            fast = fast->next->next;
            slow = slow->next;
        }
        return slow;
    }
};
```

### 删除链表中倒数第n个节点

> https://www.lintcode.com/problem/174

解题思路: 还是使用快慢指针, 快指针先走 n 步, 然后一起走, 快指针走到末尾, 慢指针走到倒数第 n 个节点。

> 要删除倒数第n个节点,需要其前驱节点, 并且这个节点可能是头节点, 所以申请一个新节点, 其 next 指向头节点,
> 
> 这样慢指针从 新节点开始走, 而快指针还是从头节点走, 快指针走到末尾,慢指针走到倒数第 n+1 个节点
> 
> 慢指针的 next 就是要删除的节点

```cpp
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode* newHead  = new ListNode(); //新节点
        newHead->next = head; //其 next 指向头节点

        ListNode* fast = head;    //快指针还是从头节点走
        ListNode* slow = newHead; //慢指针从新节点开始走

        /* 快节点先走 n */
        for (int i = 0; i < n; i++) {
            fast = fast->next;
        }

        while (fast) {
            fast = fast->next;
            slow = slow->next;
        }
        // 删除 slow->next 就完成任务
        fast = slow->next;
        slow->next = fast->next;
        delete fast;

        head = newHead->next; //新的头节点
        delete newHead;  //删除新节点

        return head;
    }
};
```

### 在O(1)时间复杂度删除链表节点

> https://www.lintcode.com/problem/372

解题思路: 要删除链表中的一个节点, 需要知道其前置节点, 但是前置节点无法获取, 那么就只能将其后节点的值覆盖当前节点, 然后将问题转化为删除下一个节点的问题

```cpp
class Solution {
public:
    /*
     * @param node: the node in the list should be deleted
     * @return: nothing
     */
    void deleteNode(ListNode * node) {
        // write your code here
        if (node){
            node->val  = node->next->val; // 非表头或表尾, 所以 next 一定不为空
            node->next = node->next->next;
        }
    }
};
```
