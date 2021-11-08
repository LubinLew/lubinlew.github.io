## linux/netlink.h

```c
struct sockaddr_nl {
        __kernel_sa_family_t    nl_family;      /* AF_NETLINK   */
        unsigned short  nl_pad;         /* zero         */
        __u32           nl_pid;         /* port ID      */
        __u32           nl_groups;      /* multicast groups mask */
};

struct nlmsghdr {
        __u32           nlmsg_len;      /* Length of message including header */
        __u16           nlmsg_type;     /* Message content */
        __u16           nlmsg_flags;    /* Additional flags */
        __u32           nlmsg_seq;      /* Sequence number */
        __u32           nlmsg_pid;      /* Sending process port ID */
};
```

## linux/cn_proc.h

process events connector

```c
enum proc_cn_mcast_op {
        PROC_CN_MCAST_LISTEN = 1,
        PROC_CN_MCAST_IGNORE = 2
};


struct proc_event {
        enum what {
                /* 事件的类型 */
                PROC_EVENT_NONE = 0x00000000,
                PROC_EVENT_FORK = 0x00000001,
                PROC_EVENT_EXEC = 0x00000002,
                PROC_EVENT_UID  = 0x00000004,
                PROC_EVENT_GID  = 0x00000040,
                PROC_EVENT_SID  = 0x00000080,
                PROC_EVENT_PTRACE = 0x00000100,
                PROC_EVENT_COMM = 0x00000200,
                /* "next" should be 0x00000400 */
                /* "last" is the last process event: exit,
                 * while "next to last" is coredumping event */
                PROC_EVENT_COREDUMP = 0x40000000,
                PROC_EVENT_EXIT = 0x80000000
        } what;
        __u32 cpu;
        __u64 timestamp_ns;  /* Number of nano seconds since system boot */
        union { /* must be last field of proc_event struct */
                struct {
                        __u32 err;
                } ack;

                struct fork_proc_event { //PROC_EVENT_FORK 
                        __kernel_pid_t parent_pid;
                        __kernel_pid_t parent_tgid;
                        __kernel_pid_t child_pid;
                        __kernel_pid_t child_tgid;
                } fork;

                struct exec_proc_event { //PROC_EVENT_EXEC
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                } exec;

                struct id_proc_event {
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                        union {
                                __u32 ruid; /* task uid */
                                __u32 rgid; /* task gid */
                        } r;
                        union {
                                __u32 euid;
                                __u32 egid;
                        } e;
                } id;

                struct sid_proc_event {
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                } sid;

                struct ptrace_proc_event { //PROC_EVENT_PTRACE
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                        __kernel_pid_t tracer_pid;
                        __kernel_pid_t tracer_tgid;
                } ptrace;

                struct comm_proc_event { //PROC_EVENT_COMM
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                        char           comm[16];
                } comm;

                struct coredump_proc_event { //PROC_EVENT_COREDUMP
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                } coredump;

                struct exit_proc_event { //PROC_EVENT_EXIT
                        __kernel_pid_t process_pid;
                        __kernel_pid_t process_tgid;
                        __u32 exit_code, exit_signal;
                } exit;

        } event_data;
};
```

## linux/connector.h

```c
/*
 * Process Events connector unique ids -- used for message routing
 */
#define CN_IDX_PROC                     0x1
#define CN_VAL_PROC                     0x1
#define CN_IDX_CIFS                     0x2
#define CN_VAL_CIFS                     0x1
#define CN_W1_IDX                       0x3     /* w1 communication */
#define CN_W1_VAL                       0x1
#define CN_IDX_V86D                     0x4
#define CN_VAL_V86D_UVESAFB             0x1
#define CN_IDX_BB                       0x5     /* BlackBoard, from the TSP GPL sampling framework */
#define CN_DST_IDX                      0x6
#define CN_DST_VAL                      0x1
#define CN_IDX_DM                       0x7     /* Device Mapper */
#define CN_VAL_DM_USERSPACE_LOG         0x1
#define CN_IDX_DRBD                     0x8
#define CN_VAL_DRBD                     0x1
#define CN_KVP_IDX                      0x9     /* HyperV KVP */
#define CN_KVP_VAL                      0x1     /* queries from the kernel */
#define CN_VSS_IDX                      0xA     /* HyperV VSS */
#define CN_VSS_VAL                      0x1     /* queries from the kernel */


#define CN_NETLINK_USERS                11      /* Highest index + 1 */

/*
 * Maximum connector's message size.
 */
#define CONNECTOR_MAX_MSG_SIZE          16384

/*
 * idx and val are unique identifiers which are used for message routing and 
 * must be registered in connector.h for in-kernel usage.
 */
struct cb_id {
        __u32 idx;
        __u32 val;
};

struct cn_msg {
        struct cb_id id;

        __u32 seq;
        __u32 ack;

        __u16 len;              /* Length of the following data */
        __u16 flags;
        __u8 data[0];
};
```
