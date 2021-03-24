# 缓冲区 - ngx_buf_t

`ngx_buf_t`(缓冲区)是用于处理大量数据的结构,它既应用于内存数据也应用于磁盘。本质上它提供的仅仅是一些指针成员和标志位。

对于HTTP模块来说，需要注意HTTP框架、事件框架是如何设置和使用`pos`、`last` 等指针以及如何处理这些标志位的。



## 结构定义

```c
typedef struct ngx_buf_s  ngx_buf_t;

struct ngx_buf_s {
	/* 当buf所指向的数据在内存里的时候，pos指向的是这段数据开始的位置 */
    u_char          *pos;
	/* 当buf所指向的数据在内存里的时候，last指向的是这段数据结束的位置,
	 * 即是这段数据最后一个有效字符的下一个位置 */
    u_char          *last;
	/* 当buf所指向的数据是在文件里的时候，file_pos指向的是这段数据的开始位置在文件中的偏移量 */
    off_t            file_pos;
	/* 当buf所指向的数据是在文件里的时候，file_last指向的是这段数据的结束位置在文件中的偏移量 */
    off_t            file_last;

	/* 当buf所指向的数据在内存里的时候，这一整块内存包含的内容可能被包含在多个buf中
	(比如在某段数据中间插入了其他的数据，这一块数据就需要被拆分开)。
	那么这些buf中的start和end都指向这一块内存的开始地址和结束地址。
	而pos和last指向本buf所实际包含的数据的开始和结尾 
	*/
    u_char          *start;         /* start of buffer */
    u_char          *end;           /* end of buffer */
	/* 实际上是一个void*类型的指针，使用者可以关联任意的对象上去，只对使用者有意义 */
    ngx_buf_tag_t    tag;
	/* 当buf所包含的内容在文件中时，file字段指向对应的文件对象 */
    ngx_file_t      *file;
	/* 当这个buf完整copy了另外一个buf的所有字段的时候，那么这两个buf指向的实际上是同一块内存，
	 * 或者是同一个文件的同一部分，此时这两个buf的shadow字段都是指向对方的。
	 * 那么对于这样的两个buf，在释放的时候，就需要使用者特别小心，具体是由哪里释放，要提前考虑好，
	 * 如果造成资源的多次释放，可能会造成程序崩溃！
	 */
    ngx_buf_t       *shadow;


    /* the buf's content could be changed 
	 * 为1时表示该buf所包含的内容是在一个用户创建的内存块中，
	 * 并且可以被在filter处理的过程中进行变更，而不会造成问题
	 */
    unsigned         temporary:1;

    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
	 * 为1时表示该buf所包含的内容是在内存中，但是这些内容却不能被进行处理的filter进行变更
     */
    unsigned         memory:1;

    /* the buf's content is mmap()ed and must not be changed 
     * 为1时表示该buf所包含的内容是在内存中, 是通过mmap使用内存映射从文件中映射到内存中的，
     * 这些内容却不能被进行处理的filter进行变更
	 */
    unsigned         mmap:1;

    /* 可以回收的。也就是这个buf是可以被释放的。这个字段通常是配合shadow字段一起使用的，
     * 对于使用ngx_create_temp_buf 函数创建的buf，并且是另外一个buf的shadow，
     * 那么可以使用这个字段来标示这个buf是可以被释放的
	 */
    unsigned         recycled:1;
	/* 为1时表示该buf所包含的内容是在文件中。 */
    unsigned         in_file:1;
	/* 遇到有flush字段被设置为1的的buf的chain，
	 * 则该chain的数据即便不是最后结束的数据（last_buf被设置，标志所有要输出的内容都完了），也会进行输出，
	 * 不会受postpone_output配置的限制，但是会受到发送速率等其他条件的限制
	 */
    unsigned         flush:1;
    unsigned         sync:1;
	/* 数据被以多个chain传递给了过滤器，此字段为1表明这是最后一个buf */
    unsigned         last_buf:1;
	/* 在当前的chain里面，此buf是最后一个。特别要注意的是last_in_chain的buf不一定是last_buf，
	 * 但是last_buf的buf一定是last_in_chain的。这是因为数据会被以多个chain传递给某个filter模块 
	 */
    unsigned         last_in_chain:1;

	/* 在创建一个buf的shadow的时候，通常将新创建的一个buf的last_shadow置为1 */
    unsigned         last_shadow:1;
	/* 由于受到内存使用的限制，有时候一些buf的内容需要被写到磁盘上的临时文件中去，那么这时，就设置此标志 */
    unsigned         temp_file:1;

    /* STUB */ int   num;
};


```
