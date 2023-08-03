# 二叉树

## 时间复杂度

| 操作  | 二叉树               | 平衡二叉树   | 红黑树    |
| --- | ----------------- | ------- | ------ |
| 查询  | 最好: log(N), 最坏: N | log(N)  | log(N) |
| 插入  | 最好: log(N), 最坏: N | log(N)  | log(N) |
| 删除  | 最好: log(N), 最坏: N | 2log(N) | log(N) |

## 二叉搜索树(BST)

对于任意一个结点，其左子树的值必定小于该结点，其右子树的值必定大于该结点。<mark>那么中序遍历的时候，就是有序的了</mark>。

理论上来说，增加，删除，修改的时间复杂度都是O(log(N))。

但是它存在一个致命的问题。退化成链表时，增加，删除，修改的时间复杂度退化为O(N) 

## 平衡二叉搜索树(AVL)

```c
// https://www.programiz.com/dsa/avl-tree
// AVL tree implementation in C

#include <stdio.h>
#include <stdlib.h>

// Create Node
struct Node {
  int key;
  struct Node *left;
  struct Node *right;
  int height;
};

int max(int a, int b);

// Calculate height
int height(struct Node *N) {
  if (N == NULL)
    return 0;
  return N->height;
}

int max(int a, int b) {
  return (a > b) ? a : b;
}

// Create a node
struct Node *newNode(int key) {
  struct Node *node = (struct Node *)
    malloc(sizeof(struct Node));
  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;
  return (node);
}

// Right rotate
struct Node *rightRotate(struct Node *y) {
  struct Node *x = y->left;
  struct Node *T2 = x->right;

  x->right = y;
  y->left = T2;

  y->height = max(height(y->left), height(y->right)) + 1;
  x->height = max(height(x->left), height(x->right)) + 1;

  return x;
}

// Left rotate
struct Node *leftRotate(struct Node *x) {
  struct Node *y = x->right;
  struct Node *T2 = y->left;

  y->left = x;
  x->right = T2;

  x->height = max(height(x->left), height(x->right)) + 1;
  y->height = max(height(y->left), height(y->right)) + 1;

  return y;
}

// Get the balance factor
int getBalance(struct Node *N) {
  if (N == NULL)
    return 0;
  return height(N->left) - height(N->right);
}

// Insert node
struct Node *insertNode(struct Node *node, int key) {
  // Find the correct position to insertNode the node and insertNode it
  if (node == NULL)
    return (newNode(key));

  if (key < node->key)
    node->left = insertNode(node->left, key);
  else if (key > node->key)
    node->right = insertNode(node->right, key);
  else
    return node;

  // Update the balance factor of each node and
  // Balance the tree
  node->height = 1 + max(height(node->left),
               height(node->right));

  int balance = getBalance(node);
  if (balance > 1 && key < node->left->key)
    return rightRotate(node);

  if (balance < -1 && key > node->right->key)
    return leftRotate(node);

  if (balance > 1 && key > node->left->key) {
    node->left = leftRotate(node->left);
    return rightRotate(node);
  }

  if (balance < -1 && key < node->right->key) {
    node->right = rightRotate(node->right);
    return leftRotate(node);
  }

  return node;
}

struct Node *minValueNode(struct Node *node) {
  struct Node *current = node;

  while (current->left != NULL)
    current = current->left;

  return current;
}

// Delete a nodes
struct Node *deleteNode(struct Node *root, int key) {
  // Find the node and delete it
  if (root == NULL)
    return root;

  if (key < root->key)
    root->left = deleteNode(root->left, key);

  else if (key > root->key)
    root->right = deleteNode(root->right, key);

  else {
    if ((root->left == NULL) || (root->right == NULL)) {
      struct Node *temp = root->left ? root->left : root->right;

      if (temp == NULL) {
        temp = root;
        root = NULL;
      } else
        *root = *temp;
      free(temp);
    } else {
      struct Node *temp = minValueNode(root->right);

      root->key = temp->key;

      root->right = deleteNode(root->right, temp->key);
    }
  }

  if (root == NULL)
    return root;

  // Update the balance factor of each node and
  // balance the tree
  root->height = 1 + max(height(root->left),
               height(root->right));

  int balance = getBalance(root);
  if (balance > 1 && getBalance(root->left) >= 0)
    return rightRotate(root);

  if (balance > 1 && getBalance(root->left) < 0) {
    root->left = leftRotate(root->left);
    return rightRotate(root);
  }

  if (balance < -1 && getBalance(root->right) <= 0)
    return leftRotate(root);

  if (balance < -1 && getBalance(root->right) > 0) {
    root->right = rightRotate(root->right);
    return leftRotate(root);
  }

  return root;
}

// Print the tree
void printPreOrder(struct Node *root) {
  if (root != NULL) {
    printf("%d ", root->key);
    printPreOrder(root->left);
    printPreOrder(root->right);
  }
}

int main() {
  struct Node *root = NULL;

  root = insertNode(root, 2);
  root = insertNode(root, 1);
  root = insertNode(root, 7);
  root = insertNode(root, 4);
  root = insertNode(root, 5);
  root = insertNode(root, 3);
  root = insertNode(root, 8);

  printPreOrder(root);

  root = deleteNode(root, 3);

  printf("\nAfter deletion: ");
  printPreOrder(root);

  return 0;
}
```

## 红黑树

```c
// https://www.programiz.com/dsa/red-black-tree
// Implementing Red-Black Tree in C

#include <stdio.h>
#include <stdlib.h>

enum nodeColor {
  RED,
  BLACK
};

struct rbNode {
  int data;
  nodeColor color;
  struct rbNode *link[2];
};

struct rbNode *root = NULL;

// Create a red-black tree
struct rbNode *createNode(int data) {
  struct rbNode *newnode;
  newnode = (struct rbNode *)malloc(sizeof(struct rbNode));
  newnode->data = data;
  newnode->color = RED;
  newnode->link[0] = newnode->link[1] = NULL;
  return newnode;
}

// Insert an node
void insertion(int data) {
  struct rbNode *stack[98], *ptr, *newnode, *xPtr, *yPtr;
  int dir[98], ht = 0, index;
  ptr = root;
  if (!root) {
    root = createNode(data);
    return;
  }

  stack[ht] = root;
  dir[ht++] = 0;
  while (ptr != NULL) {
    if (ptr->data == data) {
      printf("Duplicates Not Allowed!!\n");
      return;
    }
    index = (data - ptr->data) > 0 ? 1 : 0;
    stack[ht] = ptr;
    ptr = ptr->link[index];
    dir[ht++] = index;
  }
  stack[ht - 1]->link[index] = newnode = createNode(data);
  while ((ht >= 3) && (stack[ht - 1]->color == RED)) {
    if (dir[ht - 2] == 0) {
      yPtr = stack[ht - 2]->link[1];
      if (yPtr != NULL && yPtr->color == RED) {
        stack[ht - 2]->color = RED;
        stack[ht - 1]->color = yPtr->color = BLACK;
        ht = ht - 2;
      } else {
        if (dir[ht - 1] == 0) {
          yPtr = stack[ht - 1];
        } else {
          xPtr = stack[ht - 1];
          yPtr = xPtr->link[1];
          xPtr->link[1] = yPtr->link[0];
          yPtr->link[0] = xPtr;
          stack[ht - 2]->link[0] = yPtr;
        }
        xPtr = stack[ht - 2];
        xPtr->color = RED;
        yPtr->color = BLACK;
        xPtr->link[0] = yPtr->link[1];
        yPtr->link[1] = xPtr;
        if (xPtr == root) {
          root = yPtr;
        } else {
          stack[ht - 3]->link[dir[ht - 3]] = yPtr;
        }
        break;
      }
    } else {
      yPtr = stack[ht - 2]->link[0];
      if ((yPtr != NULL) && (yPtr->color == RED)) {
        stack[ht - 2]->color = RED;
        stack[ht - 1]->color = yPtr->color = BLACK;
        ht = ht - 2;
      } else {
        if (dir[ht - 1] == 1) {
          yPtr = stack[ht - 1];
        } else {
          xPtr = stack[ht - 1];
          yPtr = xPtr->link[0];
          xPtr->link[0] = yPtr->link[1];
          yPtr->link[1] = xPtr;
          stack[ht - 2]->link[1] = yPtr;
        }
        xPtr = stack[ht - 2];
        yPtr->color = BLACK;
        xPtr->color = RED;
        xPtr->link[1] = yPtr->link[0];
        yPtr->link[0] = xPtr;
        if (xPtr == root) {
          root = yPtr;
        } else {
          stack[ht - 3]->link[dir[ht - 3]] = yPtr;
        }
        break;
      }
    }
  }
  root->color = BLACK;
}

// Delete a node
void deletion(int data) {
  struct rbNode *stack[98], *ptr, *xPtr, *yPtr;
  struct rbNode *pPtr, *qPtr, *rPtr;
  int dir[98], ht = 0, diff, i;
  enum nodeColor color;

  if (!root) {
    printf("Tree not available\n");
    return;
  }

  ptr = root;
  while (ptr != NULL) {
    if ((data - ptr->data) == 0)
      break;
    diff = (data - ptr->data) > 0 ? 1 : 0;
    stack[ht] = ptr;
    dir[ht++] = diff;
    ptr = ptr->link[diff];
  }

  if (ptr->link[1] == NULL) {
    if ((ptr == root) && (ptr->link[0] == NULL)) {
      free(ptr);
      root = NULL;
    } else if (ptr == root) {
      root = ptr->link[0];
      free(ptr);
    } else {
      stack[ht - 1]->link[dir[ht - 1]] = ptr->link[0];
    }
  } else {
    xPtr = ptr->link[1];
    if (xPtr->link[0] == NULL) {
      xPtr->link[0] = ptr->link[0];
      color = xPtr->color;
      xPtr->color = ptr->color;
      ptr->color = color;

      if (ptr == root) {
        root = xPtr;
      } else {
        stack[ht - 1]->link[dir[ht - 1]] = xPtr;
      }

      dir[ht] = 1;
      stack[ht++] = xPtr;
    } else {
      i = ht++;
      while (1) {
        dir[ht] = 0;
        stack[ht++] = xPtr;
        yPtr = xPtr->link[0];
        if (!yPtr->link[0])
          break;
        xPtr = yPtr;
      }

      dir[i] = 1;
      stack[i] = yPtr;
      if (i > 0)
        stack[i - 1]->link[dir[i - 1]] = yPtr;

      yPtr->link[0] = ptr->link[0];

      xPtr->link[0] = yPtr->link[1];
      yPtr->link[1] = ptr->link[1];

      if (ptr == root) {
        root = yPtr;
      }

      color = yPtr->color;
      yPtr->color = ptr->color;
      ptr->color = color;
    }
  }

  if (ht < 1)
    return;

  if (ptr->color == BLACK) {
    while (1) {
      pPtr = stack[ht - 1]->link[dir[ht - 1]];
      if (pPtr && pPtr->color == RED) {
        pPtr->color = BLACK;
        break;
      }

      if (ht < 2)
        break;

      if (dir[ht - 2] == 0) {
        rPtr = stack[ht - 1]->link[1];

        if (!rPtr)
          break;

        if (rPtr->color == RED) {
          stack[ht - 1]->color = RED;
          rPtr->color = BLACK;
          stack[ht - 1]->link[1] = rPtr->link[0];
          rPtr->link[0] = stack[ht - 1];

          if (stack[ht - 1] == root) {
            root = rPtr;
          } else {
            stack[ht - 2]->link[dir[ht - 2]] = rPtr;
          }
          dir[ht] = 0;
          stack[ht] = stack[ht - 1];
          stack[ht - 1] = rPtr;
          ht++;

          rPtr = stack[ht - 1]->link[1];
        }

        if ((!rPtr->link[0] || rPtr->link[0]->color == BLACK) &&
          (!rPtr->link[1] || rPtr->link[1]->color == BLACK)) {
          rPtr->color = RED;
        } else {
          if (!rPtr->link[1] || rPtr->link[1]->color == BLACK) {
            qPtr = rPtr->link[0];
            rPtr->color = RED;
            qPtr->color = BLACK;
            rPtr->link[0] = qPtr->link[1];
            qPtr->link[1] = rPtr;
            rPtr = stack[ht - 1]->link[1] = qPtr;
          }
          rPtr->color = stack[ht - 1]->color;
          stack[ht - 1]->color = BLACK;
          rPtr->link[1]->color = BLACK;
          stack[ht - 1]->link[1] = rPtr->link[0];
          rPtr->link[0] = stack[ht - 1];
          if (stack[ht - 1] == root) {
            root = rPtr;
          } else {
            stack[ht - 2]->link[dir[ht - 2]] = rPtr;
          }
          break;
        }
      } else {
        rPtr = stack[ht - 1]->link[0];
        if (!rPtr)
          break;

        if (rPtr->color == RED) {
          stack[ht - 1]->color = RED;
          rPtr->color = BLACK;
          stack[ht - 1]->link[0] = rPtr->link[1];
          rPtr->link[1] = stack[ht - 1];

          if (stack[ht - 1] == root) {
            root = rPtr;
          } else {
            stack[ht - 2]->link[dir[ht - 2]] = rPtr;
          }
          dir[ht] = 1;
          stack[ht] = stack[ht - 1];
          stack[ht - 1] = rPtr;
          ht++;

          rPtr = stack[ht - 1]->link[0];
        }
        if ((!rPtr->link[0] || rPtr->link[0]->color == BLACK) &&
          (!rPtr->link[1] || rPtr->link[1]->color == BLACK)) {
          rPtr->color = RED;
        } else {
          if (!rPtr->link[0] || rPtr->link[0]->color == BLACK) {
            qPtr = rPtr->link[1];
            rPtr->color = RED;
            qPtr->color = BLACK;
            rPtr->link[1] = qPtr->link[0];
            qPtr->link[0] = rPtr;
            rPtr = stack[ht - 1]->link[0] = qPtr;
          }
          rPtr->color = stack[ht - 1]->color;
          stack[ht - 1]->color = BLACK;
          rPtr->link[0]->color = BLACK;
          stack[ht - 1]->link[0] = rPtr->link[1];
          rPtr->link[1] = stack[ht - 1];
          if (stack[ht - 1] == root) {
            root = rPtr;
          } else {
            stack[ht - 2]->link[dir[ht - 2]] = rPtr;
          }
          break;
        }
      }
      ht--;
    }
  }
}

// Print the inorder traversal of the tree
void inorderTraversal(struct rbNode *node) {
  if (node) {
    inorderTraversal(node->link[0]);
    printf("%d  ", node->data);
    inorderTraversal(node->link[1]);
  }
  return;
}

// Driver code
int main() {
  int ch, data;
  while (1) {
    printf("1. Insertion\t2. Deletion\n");
    printf("3. Traverse\t4. Exit");
    printf("\nEnter your choice:");
    scanf("%d", &ch);
    switch (ch) {
      case 1:
        printf("Enter the element to insert:");
        scanf("%d", &data);
        insertion(data);
        break;
      case 2:
        printf("Enter the element to delete:");
        scanf("%d", &data);
        deletion(data);
        break;
      case 3:
        inorderTraversal(root);
        printf("\n");
        break;
      case 4:
        exit(0);
      default:
        printf("Not available\n");
        break;
    }
    printf("\n");
  }
  return 0;
}
```

## 二叉树的遍历

- 前序遍历(Pre-order)    <mark>根</mark>-左-右

- 中序遍历(In-order)     左-<mark>根</mark>-右

- 后续遍历(Post-order)   左-右-<mark>根</mark>

```c
#include <stdio.h>
#include <stdlib.h> //malloc

typedef struct _BinaryTreeNode {
    struct _BinaryTreeNode* left;
    struct _BinaryTreeNode* right;
    char data;
} BTNode_t;


//前序遍历
void pre_order_traverse(BTNode_t* root)
{
    if (NULL == root) {
        return;
    }

    printf("%c ", root->data);
    pre_order_traverse(root->left);
    pre_order_traverse(root->right);
}

//中序遍历
void in_order_traverse(BTNode_t* root)
{
    if (NULL == root) {
        return;
    }

    in_order_traverse(root->left);
    printf("%c ", root->data);
    in_order_traverse(root->right);
}

//后序遍历
void post_order_traverse(BTNode_t* root)
{
    if (NULL == root) {
        return;
    }

    post_order_traverse(root->left);
    post_order_traverse(root->right);
    printf("%c ", root->data);
}

BTNode_t* new_node(char data)
{
    BTNode_t* node = calloc(sizeof(BTNode_t), 1);
    node->data = data;

    return node;
}

int main(void)
{
    //https://blog.csdn.net/m0_52226803/article/details/117423431
    BTNode_t* root = new_node('F');
    root->left = new_node('C');
    root->left->left = new_node('A');
    root->left->right = new_node('D');
    root->left->right->left = new_node('B');
    root->right = new_node('E');
    root->right->left = new_node('H');
    root->right->right = new_node('G');
    root->right->right->left = new_node('M');

    printf("[pre-order]  ");
    pre_order_traverse(root);  printf("\n");
    printf("[in-order]   ");
    in_order_traverse(root); printf("\n");
    printf("[post-order] ");
    post_order_traverse(root); printf("\n");

    return 0;
}
```

## AVL树与红黑树对比

- AVL搜索更快，因为其更平衡(balanced)

- 红黑树的插入和删除操作更快，因为其相对宽松的平衡性

- AVL树每个节点都需要存储 平衡因子(factor)或高度(height)，需要一个整形，而红黑树只需要一个bit(存储红或红)

- 红黑树多用于高级语言的 map/set 等功能实现，AVL树更多用于数据库(快速读取数据)

> 红黑树还用于 linux epoll / nginx timer / linux malloc

## 判断一颗二叉树是否为平衡二叉树

可以使用前序遍历(慢) 或者 后序遍历(快) 的方法

```c
#include <stdio.h>
#include <stdlib.h> //malloc


typedef struct _BinaryTreeNode {
    struct _BinaryTreeNode* left;
    struct _BinaryTreeNode* right;
    char data;
} BTNode_t;

#define MAX(_a, _b)     ((_a) > (_b) ? (_a) : (_b))
#define SUB_ABS(_a, _b) ((_a) > (_b) ? (_a) - (_b) : (_b) - (_a))

/* 二叉树的最大深度 */
int maxDepth(BTNode_t* root)
{
    if (NULL == root) {
        return 0;
    }

    int LeftDepth  = maxDepth(root->left);
    int RightDepth = maxDepth(root->right);

    return MAX(LeftDepth, RightDepth) + 1;
}


/* 判断是否为平衡二叉树,  前序遍历(慢)*/
int isBalanced(BTNode_t* root)
{
    if (root == NULL) {//空树是平衡树
        return 1;
    }

    int LeftDepth  = maxDepth(root->left);
    int RightDepth = maxDepth(root->right);
    return (SUB_ABS(LeftDepth, RightDepth) <= 1) && \
        isBalanced(root->left) && \
        isBalanced(root->right);
}


/* 判断是否为平衡二叉树,  后序遍历(快)*/
int isBalanced_op(BTNode_t* root, int *pdepth)
{
    if (NULL == root) {
        *pdepth = 0;
        return 1;
    }

    //按照后序遍历去判断,先判断左右子树，然后记录以当前结点为根树的深度
    int left, right;
    if (isBalanced_op(root->left, &left) && isBalanced_op(root->right, &right)) {
        int gap = SUB_ABS(right, left);
        if (gap <= 1) {
            *pdepth = MAX(left, right) + 1;
            return 1;
        }
    }

    return 0;
}

BTNode_t* new_node(char data)
{
    BTNode_t* node = calloc(sizeof(BTNode_t), 1);
    node->data = data;

    return node;
}


int main(void)
{
    BTNode_t* root = new_node('F');
    root->left = new_node('C');
    root->left->left = new_node('A');
    root->left->right = new_node('D');
    root->left->right->left = new_node('B');
    root->right = new_node('E');
    root->right->left = new_node('H');
    root->right->right = new_node('G');
    root->right->right->left = new_node('M');

    printf("isBalanced: %d\n", isBalanced(root));
    int depth = 0;
    printf("isBalanced_op: %d\n", isBalanced_op(root, &depth));

    return 0;
}
```

https://blog.csdn.net/hansionz/article/details/82745625