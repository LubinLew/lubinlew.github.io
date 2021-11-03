# PROCPS

[procps](https://gitlab.com/procps-ng/procps) 一套工具, 其中包含了 free/kill/ps/sysctl/top/watch 等多个常用命令,
本文这里介绍其中的 libprocps API 用于获取 linux 系统下的 /proc/ 下的信息。

## API声明

```c
#include <proc/readproc.h> //位置:/usr/include/proc/readproc.h

//initialize process information from /proc/
PROCTAB* openproc (int flags, ... );
void closeproc (PROCTAB* PT);

//read information from next /proc/## entry
proc_t* readproc(PROCTAB *PT, proc_t *return_buf);
void freeproc(proc_t *p);

//read information for all current processes at once
proc_t** readproctab(int flags, ... );
void freeproctab(proc_t **p);
```

### openproc flag

| flag                                       | Descption                                                                                                                                                                                                                        |
| ------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| PROC_FILLMEM                               | 从 `/proc/#pid/statm` 读取内存使用信息(单位为内存页)                                                                                                                                                                                            |
| PROC_FILLCOM                               | 从 `/proc/#pid/cmdline` 读取命令行信息,赋值 `proc_t.cmdline` 成员                                                                                                                                                                            |
| PROC_FILLENV                               | 从 `/proc/#pid/environ` 读取命令行信息,赋值 `proc_t.environ` 成员                                                                                                                                                                            |
| PROC_FILLUSR                               | 将用户ID通过 `/etc/passwd` 文件转换为用户名                                                                                                                                                                                                   |
| PROC_FILLGRP                               | 将组ID通过 `/etc/group` 文件转换为组名                                                                                                                                                                                                      |
| PROC_FILLSTATUS                            | 从 `/proc/#pid/status` 读取内存信息                                                                                                                                                                                                     |
| PROC_FILLSTAT                              | 从 `/proc/#pid/stat` 读取状态信息                                                                                                                                                                                                       |
| PROC_FILLARG                               | 等价于 PROC_FILLCOM                                                                                                                                                                                                                 |
| PROC_FILLCGROUP                            | 从 `/proc/#pid/cgroup` 读取 cgroup 信息                                                                                                                                                                                               |
| PROC_FILLSUPGRP                            | resolve supplementary group id -> group name                                                                                                                                                                                     |
| PROC_FILLOOM                               | 向结构体 proc_t 中填充 oom_score 和 oom_adj 信息                                                                                                                                                                                           |
| PROC_FILLNS                                | 向结构体 proc_t 中填充 namespace 信息                                                                                                                                                                                                     |
| PROC_FILLSYSTEMD                           | 向结构体 proc_t 中填充 systemd 信息                                                                                                                                                                                                       |
| PROC_LOOSE_TASKS                           | threat threads as if they were processes                                                                                                                                                                                         |
| PROC_PID (2nd argument pid_t* pidlist)     | 通过第二个参数查找指定进程ID的进程, 第二个参数pidlist 数组(0结尾) [例子](https://qiita.com/rarul/items/e02b4f8faaf69890bc18#%E7%89%B9%E5%AE%9A%E3%83%97%E3%83%AD%E3%82%BB%E3%82%B9%E6%83%85%E5%A0%B1%E3%82%92%E5%BE%97%E3%81%9F%E3%81%84%E5%A0%B4%E5%90%88) |
| PROC_UID (arguments uid_t* uidlist, int n) | 通过第二个和第三个参数查找指定用户ID的进程,第二个参数 uidlist 为数组, 第三个参数为数组的个数                                                                                                                                                                            |
| PROC_EDITCGRPCVT                           | edit cgroup as single vector                                                                                                                                                                                                     |
| PROC_EDITCMDLCVT                           | edit cmdline as single vector                                                                                                                                                                                                    |
| PROC_EDITENVRCVT                           | edit environ as single vector                                                                                                                                                                                                    |

### PROCTAB

```c
// PROCTAB: data structure holding the persistent information readproc needs
// from openproc().  The setup is intentionally similar to the dirent interface
// and other system table interfaces (utmp+wtmp come to mind).
typedef struct PROCTAB {
    DIR*        procfs;
//    char deBug0[64];
    DIR*        taskdir;  // for threads
//    char deBug1[64];
    pid_t       taskdir_user;  // for threads
    int         did_fake; // used when taskdir is missing
    int(*finder)(struct PROCTAB *__restrict const, proc_t *__restrict const);
    proc_t*(*reader)(struct PROCTAB *__restrict const, proc_t *__restrict const);
    int(*taskfinder)(struct PROCTAB *__restrict const, const proc_t *__restrict const, proc_t *__restrict const, char *__restrict const);
    proc_t*(*taskreader)(struct PROCTAB *__restrict const, const proc_t *__restrict const, proc_t *__restrict const, char *__restrict const);
    pid_t*      pids;   // pids of the procs
    uid_t*      uids;   // uids of procs
    int         nuid;   // cannot really sentinel-terminate unsigned short[]
    int         i;  // generic
    unsigned    flags;
    unsigned    u;  // generic
    void *      vp; // generic
    char        path[PROCPATHLEN];  // must hold /proc/2000222000/task/2000222000/cmdline
    unsigned pathlen;        // length of string in the above (w/o '\0')
} PROCTAB;
```

### proc_t

```c
// Basic data structure which holds all information we can get about a process.
// (unless otherwise specified, fields are read from /proc/#/stat)
//
// Most of it comes from task_struct in linux/sched.h
//
typedef struct proc_t {
// 1st 16 bytes
    int
        tid,            // (special)       task id, the POSIX thread ID (see also: tgid)
        ppid;           // stat,status     pid of parent process
    unsigned long       // next 2 fields are NOT filled in by readproc
        maj_delta,      // stat (special) major page faults since last update
        min_delta;      // stat (special) minor page faults since last update
    unsigned
        pcpu;           // stat (special)  %CPU usage (is not filled in by readproc!!!)
    char
        state,          // stat,status     single-char code for process state (S=sleeping)
#ifdef QUICK_THREADS
        pad_1,          // n/a             padding (psst, also used if multi-threaded)
#else
        pad_1,          // n/a             padding
#endif
        pad_2,          // n/a             padding
        pad_3;          // n/a             padding
// 2nd 16 bytes
    unsigned long long
        utime,          // stat            user-mode CPU time accumulated by process
        stime,          // stat            kernel-mode CPU time accumulated by process
// and so on...
        cutime,         // stat            cumulative utime of process and reaped children
        cstime,         // stat            cumulative stime of process and reaped children
        start_time;     // stat            进程的启动时间,这里的数值是系统从开机起经历的时钟嘀嗒数 
#ifdef SIGNAL_STRING
    char
        // Linux 2.1.7x and up have 64 signals. Allow 64, plus '\0' and padding.
        signal[18],     // status          mask of pending signals, per-task for readtask() but per-proc for readproc()
        blocked[18],    // status          mask of blocked signals
        sigignore[18],  // status          mask of ignored signals
        sigcatch[18],   // status          mask of caught  signals
        _sigpnd[18];    // status          mask of PER TASK pending signals
#else
    long long
        // Linux 2.1.7x and up have 64 signals.
        signal,         // status          mask of pending signals, per-task for readtask() but per-proc for readproc()
        blocked,        // status          mask of blocked signals
        sigignore,      // status          mask of ignored signals
        sigcatch,       // status          mask of caught  signals
        _sigpnd;        // status          mask of PER TASK pending signals
#endif
    unsigned KLONG
        start_code,     // stat            address of beginning of code segment
        end_code,       // stat            address of end of code segment
        start_stack,    // stat            address of the bottom of stack for the process
        kstk_esp,       // stat            kernel stack pointer
        kstk_eip,       // stat            kernel instruction pointer
        wchan;          // stat (special)  address of kernel wait channel proc is sleeping in
    long
        priority,       // stat            kernel scheduling priority
        nice,           // stat            standard unix nice level of process
        rss,            // stat            identical to 'resident'
        alarm,          // stat            ?
    // the next 7 members come from /proc/#/statm
        size,           // statm           total virtual memory (as # pages)
        resident,       // statm           resident non-swapped memory (as # pages)
        share,          // statm           shared (mmap'd) memory (as # pages)
        trs,            // statm           text (exe) resident set (as # pages)
        lrs,            // statm           library resident set (always 0 w/ 2.6)
        drs,            // statm           data+stack resident set (as # pages)
        dt;             // statm           dirty pages (always 0 w/ 2.6)
    unsigned long
        vm_size,        // status          equals 'size' (as kb)
        vm_lock,        // status          locked pages (as kb)
        vm_rss,         // status          equals 'rss' and/or 'resident' (as kb)
        vm_rss_anon,    // status          the 'anonymous' portion of vm_rss (as kb)
        vm_rss_file,    // status          the 'file-backed' portion of vm_rss (as kb)
        vm_rss_shared,  // status          the 'shared' portion of vm_rss (as kb)
        vm_data,        // status          data only size (as kb)
        vm_stack,       // status          stack only size (as kb)
        vm_swap,        // status          based on linux-2.6.34 "swap ents" (as kb)
        vm_exe,         // status          equals 'trs' (as kb)
        vm_lib,         // status          total, not just used, library pages (as kb)
        rtprio,         // stat            real-time priority
        sched,          // stat            scheduling class
        vsize,          // stat            number of pages of virtual memory ...
        rss_rlim,       // stat            resident set size limit?
        flags,          // stat            kernel flags for the process
        min_flt,        // stat            number of minor page faults since process start
        maj_flt,        // stat            number of major page faults since process start
        cmin_flt,       // stat            cumulative min_flt of process and child processes
        cmaj_flt;       // stat            cumulative maj_flt of process and child processes
    char
        **environ,      // (special)       environment string vector (/proc/#/environ)
        **cmdline,      // (special)       command line string vector (/proc/#/cmdline)
        **cgroup,       // (special)       cgroup string vector (/proc/#/cgroup)
         *cgname,       // (special)       name portion of above (if possible)
         *supgid,       // status          supplementary gids as comma delimited str
         *supgrp;       // supp grp names as comma delimited str, derived from supgid
    char
        // Be compatible: Digital allows 16 and NT allows 14 ???
        euser[P_G_SZ],  // stat(),status   effective user name
        ruser[P_G_SZ],  // status          real user name
        suser[P_G_SZ],  // status          saved user name
        fuser[P_G_SZ],  // status          filesystem user name
        rgroup[P_G_SZ], // status          real group name
        egroup[P_G_SZ], // status          effective group name
        sgroup[P_G_SZ], // status          saved group name
        fgroup[P_G_SZ], // status          filesystem group name
        cmd[64];        // stat,status     basename of executable file in call to exec(2)
    struct proc_t
        *ring,          // n/a             thread group ring
        *next;          // n/a             various library uses
    int
        pgrp,           // stat            process group id
        session,        // stat            session id
        nlwp,           // stat,status     number of threads, or 0 if no clue
        tgid,           // (special)       thread group ID, the POSIX PID (see also: tid)
        tty,            // stat            full device number of controlling terminal
        /* FIXME: int uids & gids should be uid_t or gid_t from pwd.h */
        euid, egid,     // stat(),status   effective
        ruid, rgid,     // status          real
        suid, sgid,     // status          saved
        fuid, fgid,     // status          fs (used for file access only)
        tpgid,          // stat            terminal process group id
        exit_signal,    // stat            might not be SIGCHLD
        processor;      // stat            current (or most recent?) CPU
    int
        oom_score,      // oom_score       (badness for OOM killer)
        oom_adj;        // oom_adj         (adjustment to OOM score)
    long
        ns[NUM_NS];     // (ns subdir)     inode number of namespaces
    char
        *sd_mach,       // n/a             systemd vm/container name
        *sd_ouid,       // n/a             systemd session owner uid
        *sd_seat,       // n/a             systemd login session seat
        *sd_sess,       // n/a             systemd login session id
        *sd_slice,      // n/a             systemd slice unit
        *sd_unit,       // n/a             systemd system unit id
        *sd_uunit;      // n/a             systemd user unit id
    const char
        *lxcname;       // n/a             lxc container name
} proc_t;
```

### 

### 进程启动时间

**start_time** : 进程的启动时间

次结构体里的数值是系统从开机起经历的时钟嘀嗒数, 这个嘀嗒数除以 每秒的时钟滴答数`sysconf(_SC_CLK_TCK)` 既可以转换为秒数。

系统的的开机时间从 `/proc/stat` 文件中的 btime 行获取, 这样 btime + 上面的描述, 即得到了 进程启动的unix时间戳。

具体的程序见 [process_start_time](https://github.com/LubinLew/codes/blob/main/linux/process_start_time.md)。

### 进程用户

euser：effective user name，它通常与 Real User 相同，但有时会更改以允许非特权用户访问只能由特权用户（如 root）访问的文件。

ruser：real user name，对于进程，Real User 只是启动它的 User。它定义了这个进程可以访问哪些文件。

suser：saved user name，当一个进程以提升的权限（通常是 root）运行时需要做一些低权限的工作时使用，这可以通过临时切换到非权限帐户来实现。

fuser：filesystem user name

>  **Effective user ID**
> 
> The effective UID (`euid`) of a process is used for most access checks. It is also used as the owner for files created by that process. The effective GID (`egid`) of a process also affects access control and may also affect file creation, depending on the semantics of the specific kernel implementation in use and possibly the mount options used. According to BSD Unix semantics, the group ownership given to a newly created file is unconditionally inherited from the group ownership of the directory in which it is created. According to [AT&T](https://en.wikipedia.org/wiki/AT%26T "AT&T") [UNIX System V](https://en.wikipedia.org/wiki/UNIX_System_V "UNIX System V") semantics (also adopted by Linux variants), a newly created file is normally given the group ownership specified by the `egid` of the process that creates the file. Most filesystems implement a method to select whether BSD or AT&T semantics should be used regarding group ownership of a newly created file; BSD semantics are selected for specific directories when the S_ISGID (s-gid) permission is set.[[1]](https://en.wikipedia.org/wiki/User_identifier#cite_note-1)
> 
> **File system user ID**
> 
> Linux also has a file system user ID (`fsuid`) which is used explicitly for access control to the file system. It matches the `euid` unless explicitly set otherwise. It may be root's user ID only if `ruid`, `suid`, or `euid` is root. Whenever the `euid` is changed, the change is propagated to the `fsuid`.
> 
> The intent of `fsuid` is to permit programs (e.g., the [NFS](https://en.wikipedia.org/wiki/Network_File_System "Network File System") server) to limit themselves to the file system rights of some given `uid` without giving that `uid` permission to send them signals. Since kernel 2.0, the existence of `fsuid` is no longer necessary because Linux adheres to [SUSv3](https://en.wikipedia.org/wiki/Single_UNIX_Specification#2001:_Single_UNIX_Specification_version_3,_POSIX:2001 "Single UNIX Specification") rules for sending signals, but `fsuid` remains for compatibility reasons.[[2]](https://en.wikipedia.org/wiki/User_identifier#cite_note-Kerrisk-2)
> 
> **Saved user ID**
> 
> The saved user ID (`suid`) is used when a program running with elevated privileges needs to do some unprivileged work temporarily; changing `euid` from a privileged value (typically `0`) to some unprivileged value (anything other than the privileged value) causes the privileged value to be stored in `suid`.[[3]](https://en.wikipedia.org/wiki/User_identifier#cite_note-3) Later, a program's `euid` can be set back to the value stored in `suid`, so that elevated privileges can be restored; an unprivileged process may set its `euid` to one of only three values: the value of `ruid`, the value of `suid`, or the value of `euid`.
> 
> **Real user ID**
> 
> The real UID (`ruid`) and real GID (`rgid`) identify the real owner of the process and affect the permissions for sending signals. A process without superuser privileges may signal another process only if the sender's `ruid` or `euid` matches receiver's `ruid` or `suid`. Because a child process inherits its credentials from its parent, a child and parent may signal each other.

[User identifier - Wikipedia](https://en.wikipedia.org/wiki/User_identifier)

[Real, Effective and Saved UserID in Linux - GeeksforGeeks](https://www.geeksforgeeks.org/real-effective-and-saved-userid-in-linux/#:~:text=Effective%20UserID%20%3A%20It%20is%20normally,a%20privileged%20user%20like%20root.)

## 安装

```bash
# Centos
$ yum install procps-ng-devel
$ rpm -ql procps-ng-devel
/usr/include/proc/readproc.h
...
/usr/lib64/libprocps.so
/usr/lib64/pkgconfig/libprocps.pc

# Ubuntu
$ apt  install  libprocps-dev
$ apt-file list libprocps-dev
libprocps-dev: /usr/include/proc/readproc.h
...
libprocps-dev: /usr/lib/x86_64-linux-gnu/libprocps.a
libprocps-dev: /usr/lib/x86_64-linux-gnu/libprocps.so
libprocps-dev: /usr/lib/x86_64-linux-gnu/pkgconfig/libprocps.pc
```

## References

https://qiita.com/rarul/items/e02b4f8faaf69890bc18
