# 轮询算法(Round-Robin)

> [负载均衡之加权轮询算法](https://blog.csdn.net/larry_zeng1/article/details/80407745)
> [Nginx加权轮询算法](https://www.cnblogs.com/tenny-peng/p/11532019.html)

## 普通轮询

### 原理

轮询算法是一种简单的负载均衡算法。假设有 N (1,2,3 ... N)个服务器，每次来的请求按照服务器列表的顺序,依次分配。算法的优点是其简洁性，它无需记录当前所有连接的状态，所以它是一种`无状态调度`。

### 实现

```c
#include <stdio.h>

//假设有5台服务器
#define  N 5

//五台服务器具体信息
char* server_list[N] = {
    "192.168.1.10",
    "192.168.1.20",
    "192.168.1.30",
    "192.168.1.40",
    "192.168.1.50"
};

/** 轮询算法实现
 * [IN] request_number : 请求的序号,表示这个请求是负载均衡器上接收的第x个请求
 * [RET] 返回选择服务器在数组中的下标序号
*/
int round_robin(int request_number)
{
    return request_number % N;
}


//测试程序
int main(void)
{
    int request_number, idx;

    for (request_number = 1; request_number < 30; request_number++) {
        idx = round_robin(request_number);
        printf("[%d]%s\n", idx, server_list[idx]);
    }

    return 0;
}
```

## 加权轮询

普通轮询其实是权值相同的加权轮询。权值相同适用于那些服务器性能配置相同的一群服务器。当后端服务器的性能不同时，就需要一个权值，去标志其性能。

### 原理

根据服务器的不同处理能力，给每个服务器分配不同的权值，使其能够接受相应权值数的服务请求。看下面的例子，IP地址后面是权值， 表示有7个请求，服务器'192.168.1.10'应该分配到一个，服务器"192.168.1.20"应该分配到2个，服务器"192.168.1.30"应该分配到7个。

```c
    "192.168.1.10", 1
    "192.168.1.20", 2
    "192.168.1.30", 4
```

这个时候，算法实现简单的来说，可以创建一个总权值大小的数组，不同服务器按照其权值占用不同数量的数组元素。例如像下面这种方式:

```c
"192.168.1.10",
"192.168.1.20",
"192.168.1.20",
"192.168.1.30",
"192.168.1.30",
"192.168.1.30",
"192.168.1.30"
```

这种方式的缺点的是生成的序列不均匀。"192.168.1.30"处理4个请求的时候，其他2台服务器处于空闲状态，没有合理的利用服务器的整体资源。一种叫做 `平滑的加权轮询` 方法就用来解决这个问题。其原理就是生成序列更加均匀的数组。
```c
#include <stdio.h>

#define N (3)

typedef struct {
    char *server;
    int   weight;
    int   current_weight;
} server_t;

server_t server_group[N] = {
    {"192.168.1.10", 4, 4},
    {"192.168.1.20", 2, 2},
    {"192.168.1.30", 1, 1}
};



static server_t *
weighted_round_robin(void)
{
    unsigned int  i, n, total = 0;
    server_t *peer, *best = NULL;

    for (i = 0; i < N; i++) {
        peer = &server_group[i];
       peer->current_weight += peer->weight;
        total += peer->weight;

        if (best == NULL || peer->current_weight > best->current_weight) {
            best = peer;
        }
    }

    if (best == NULL) {
        return NULL;
    }

    best->current_weight -= total;

    printf("%s\n", best->server);

    return best;
}



int main(void)
{
    int i;

    for (i = 0; i < 7; i++) {
        weighted_round_robin();
    }

    return 0;
}
```


