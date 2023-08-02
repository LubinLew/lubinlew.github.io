# 算法

## 如何判断一棵树是否是平衡二叉树

要判断一棵树的左右子树的高度差的绝对值是否大于1，
如果大于1，那么它不是平衡树，
如果小于等于1，它就是平衡树。

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

typedef char BTDataType;
typedef struct BinaryTreeNode
{
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
    BTDataType data;
}BTNode;


//二叉树的最大深度
int maxDepth(BTNode* root)
{
    if (root == 0) {
        return 0;
    }
    int LeftDepth = maxDepth(root->left);
    int RightDepth = maxDepth(root->right);
    return LeftDepth > RightDepth ? LeftDepth + 1 : RightDepth + 1;
}


//判断是否为平衡二叉树
bool isBalanced(BTNode* root)
{
    if (root == NULL) {
        //空树是平衡树
        return true;
    }
    int LeftDepth = maxDepth(root->left);
    int RightDepth = maxDepth(root->right);
    return abs(LeftDepth - RightDepth) <= 1
        && isBalanced(root->left)
        && isBalanced(root->right);
}
```

https://blog.csdn.net/weixin_50601177/article/details/128551978

## 最长回文子串 中心扩散法/有动态规划

## 给定字符串s，输出第一个满足：在s中恰好出现3次的字母。保证存在这样的字母且字符串都是小写字母组成。

比较简单，扫一遍开个数组统计，再开个vector存一下就行了。手写代码。
