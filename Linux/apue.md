## 第一章 基础知识

#### 1.1  引言

所有操作系统都为他们所运行的程序提供服务。典型的服务包括：执行新程序、打开文件、读文件、分配存储区以及获取当前时间等，

本书集中阐述不同版本的UNIX操作系统所提供的服务。

**本章会简要的介绍UNIX提供的各种服务**，在以后的各章中将对这些概念做更详细的说明。

#### 1.2  UNIX体系结构

从严格意义上说，**可将操作系统定义为一种软件**，它控制计算机硬件资源，提供程序运行环境。**我们通常将这种软件称为内核**。

下图显示了UNIX系统的体系结构。

<img src=".\img_apue\linux内核.png" style="zoom:50%;" />

#### 1.4  文件和目录

###### 1.4.1 文件系统

UNIX文件系统是目录和文件的一种层次结构，**所有东西的起点都是根的目录，这个目录名称是一个字符“/”**。

**目录是一个包含目录项的文件，**可以认为每个目录项都包含一个文件名，同时还包含该文件属性的信息(ls -l可以查看)。第四章会详细说明文件的各种属性。

###### 1.4.2 文件名

目录中的各个名字成为文件名，创建目录时会自动创建两个文件名：**.(当前目录)和..(父目录)**  (可见为什么 `cd ..` 会回到父目录)

###### 1.4.3 路径名

以斜线开头的路径名称成为绝对路径名（可以理解为以根目录开头的相对路径名），否则为相对路径名。

###### 1.4.4  工作目录

每个进程都有一个工作目录，有时称其为当前工作目录，进程可以用`chdir`函数更改其工作目录

###### 1.4.5 起始目录

登录时，工作目录设置为起始目录，该起始目录从口令文件中相应用户的登录项中取得。

#### **1.5  输入与输出**

###### 1.5.1 文件描述符

文件描述符通常是一个小的非负整数，**用来标识一个特定进程正在访问的文件**。当内核打开一个现有文件或创建一个新文件时，

它都返回一个文件描述符，**可以通过这个文件描述符进行读写文件**。

###### 1.5.2  标准输入、标准输出和标准错误

每当运行一个新程序时，所有的shell都为其打开3个文件描述符，即**标准输入(0)、标准输出(1)和标准错误(2)**，如果不做特殊处理，

则这3个描述符都链接到终端。

###### 1.5.3  不带缓冲的I/O

函数`open`、`read`、`write`、`lseek`以及close提供了不带缓冲的I/O。**这些函数都使用文件描述符**

下面程序展示了如何复制UNIX下的普通文件 

```c
#include "apue.h"

#define BUFFSIZE 4096

int main(void)
{
    int n;
    char buf[BUFFSIZE];
    while((n=read(STDIN_FILENO,buf,BUFFSIZE))>0)
        if(write(STDOUT_FILENO,buf,n)!=n)
            err_sys("write error");
    if(n<0)
        err_sys("read error");
    exit(0);
}
```

**大多数shell都提供一种方法，使其中任何一个或所有这3个描述符都能重定向到某个文件**。例如`ls > file.list` 可以将ls的输出(标准输出 文件描述符为1)重定向到名为`file.list`的文件。

如下执行上面的程序： 

`./a.out > data` 可以把程序的标准输出重定向到文件data  

`./a.out < infile > outfile` 把程序的标准输入重定向到`infile`  把标准输出重定向到`outfile`，实现了文件的复制。

########## 1.5.4  标准I/O

**标准I/O 为那些不带缓冲的I/O函数提供了一个带缓冲的接口。**

下面演示使用标准I/O复制UNIX文件

```c
#include "apue.h"

int main(void)
{
    int c;
    while((c=getc(stdin))!=EOF)
        if(putc(c,stdout)==EOF)
            err_sys("output error");
    if(ferror(stdin))
        err_sys("input error");
    exit(0);
}
```

#### 1.6  **程序与进程** 

###### 1 .6.1 程序

程序是一个存储在磁盘上某个目录中的可执行文件

###### 1.6.2 进程和进程ID

程序的执行实例被称为进程，**UNIX系统确保每个进程都有一个唯一的数字标识符**，称为进程ID。进程ID总是一个非负整数

下面程序用于打印进程ID

```c
#include "apue.h"

int main(void)
{
    printf("hello world from process ID %ld\n",(long)getpid());
    exit(0);
}
```

###### 1.6.3 进程控制

有3个用于进程控制的主要函数：`fork`、`exec`和`waitpid`。

UNIX系统的进程控制功能可以用一个简单得程序说明，下面程序从标准输入读取命令，然后执行命令，类似于shell程序的基本实施部分。

```c
#include "apue.h"
#include <sys/wait.h>

int main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;
    printf("%% ");
    while(fgets(buf,MAXLINE,stdin)!=NULL)
    {
        if(buf[strlen(buf)-1]=='\n')
            buf[strlen(buf)-1]=0;
        if((pid=fork())<0)
            err_sys("fork error");
        else if(pid==0)
        {
            execlp(buf,buf,(char *)0);
            err_ret("couldn't execute:%s",buf);
            exit(127);
        }
        if((pid=waitpid(pid,&status,0))<0)
            err_sys("waitpid error");
        printf("%% ");
    }
    exit(0);
}
```

###### 1.6.4  线程和线程ID

与进程相同，线程也用ID标识。但是，线程ID只在它所属的进程内起作用。

#### **1.7  出错处理**

当UNIX系统函数出错时，通常会返回一个负值，而且整形变量`errno`通常被设置为具有特定信息的值。

文件`<error.h>`中定义了`error`以及可以赋予它的各种常量。

C标准定义了两个函数，用于打印出错信息

```c
#include <string.h>
char *strerror(int errnum);
```

 `strerror`函数将`errnm`(通常就是error值)映射为一个出错消息字符串，并且返回此字符串的指针。 

```c
#include <stdio.h>
void perror(const char *msg);
```

 `perror`函数基于`errno`的当前值，在标准错误上产生一条出错信息，然后返回。 

下面程序显示这两个出错函数的使用方法 

```c
#include "apue.h"
#include <errno.h>

int main(int argc,char *argv[])
{
    fprintf(stderr,"EACCES:%s\n",strerror(EACCES));
    errno=ENOENT;
    perror(argv[0]);
    exit(0);
}
```

#### **1.8  用户标识** 

###### 1.8.1 用户ID

用户ID是一个数值，用来确定一个用户。用户ID为0为根用户或超级用户，超级用户对系统有自由的支配权。

###### 1.8.2 组ID

组ID是由系统管理员在指定用户登录名时分配的，可以把多个用户分成一组。

下面程序用于打印用户ID和组ID

```c
#include "apue.h"

int main(void)
{
    printf("uid=%d,gid=%d\n",getuid(),getgid());
    exit(0);
}
```

###### 1.8.3 附属组ID

除了在口令文件中对一个登录名指定一个组ID之外，大多数UNIX系统版本还允许一个用户属于另外一些组。

#### **1.9 信号**

信号用于通知进程发生了某种情况。例如，若某一进程执行除法操作，其除数为0，则将名为`SIGFPE`(浮点异常)的信号发送给进程。

进程有以下3种处理信号的方式：

**1 忽略信号**

**2 按系统默认方式处理**

**3 提供一个函数，信号发生时调用该函数，这被称为捕捉该信号**

终端键盘上有两种产生信号的方法，分别称为中断键(通常是`Delete`或`Ctrl+C`)和退出键(通常是`Ctrl+`\)

修改之前的`shell`实例，使程序可以捕获`SIGINT`信号，我们会在第10章详细的介绍信号。

```c
#include "apue.h"
#include <sys/wait.h>

static void sig_int(int);

int main(void)
{
    char buf[MAXLINE];
    pid_t pid;
    int status;
    if(signal(SIGINT,sig_int)==SIG_ERR);
                err_sys("signal error");
    printf("%% ");
    while(fgets(buf,MAXLINE,stdin)!=NULL)
    {
        if(buf[strlen(buf)-1]=='\n')
            buf[strlen(buf)-1]=0;
        if((pid=fork())<0)
            err_sys("fork error");
        else if(pid==0)
        {
            execlp(buf,buf,(char *)0);
            err_ret("couldn't execute:%s",buf);
            exit(127);
        }
        if((pid=waitpid(pid,&status,0))<0)
            err_sys("waitpid error");
        printf("%% ");
    }
    exit(0);
}
void sig_int(int signo)
{
        printf("interrupt\n%%");
}
```

#### **1.10  时间值** 

历史上，UNIX系统使用过两种不同的时间值

1 日历时间。该值是从1970年1月1日00:00:00这个特定时间以来所经过的秒数的累计值。

2 进程时间。也被称为CPU时间

  当度量一个进程的执行时间时，UNIX系统为一个进程维护了3个进程时间值

  时钟时间  即进程运行的时间总量，其值与系统中同时运行的进程数有关

  用户CPU时间  执行用户指令所用的时间量 

  系统CPU时间  执行系统调用的时间

#### **1.11 系统调用和库函数**

系统调用提供的函数如`open`, `close`, `read`, `write`, `ioctl`等，系统调用发生在内核空间

标准C库函数提供的文件操作函数`1fopen`, `fread`, `fwrite`, `fclose`, `fflush`, `fseek`等属于库函数，底层也是通过系统调用来实现的

<img src=".\img_apue\1_2_malloc函数和sbrk系统调用.png" style="zoom:50%;" />

## 第三章 文件与I/O

本章开始讨论UNIX系统，先说明可用的文件I/O函数---打开文件、读写文件等

UNIX系统中的大多数文件I/O只需用到5个函数：`open`、`read`、`write`、`lseek`以及`close`

#### **2.1  open函数** 

**返回一个最小的未用描述符** 

```c
#include <fcntl.h>
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

 其中path参数是打开或创建文件的名字，flags参数由下列一个或多个常量进行“或”运算得到： 

**`O_RDONLY`  只读打开**

**`O_WRONLY`  只写打开**

**`O_RDWR`   读、写打开**

`O_EXEC`  只执行打开

`O_SEARCH`  只搜索打开（应用于目录）



这5个常量中必须指定一个，下列常量则是可选的：

**`O_APPEND`  每次写时都追加到文件的尾端**

`O_CLOEXEC`  把`FD_CLOEXEC`常量设置为文件描述符标志

**`O_CREAT`  若此文件不存在则创建它。使用此选项时，open函数必须同时说明第三个参数mode**

`O_DIRECTORY`  如果path引用的不是目录，则出错

`O_EXEL`  如果同时指定了`O_CREAT`，而文件已经存在，则出错

`O_NOCTTY`  如果path引用的是终端设备，则不讲该设备分配作为此进程的控制终端

`O_NOFOLLOW`  如果path应用的是一个符号链接，则出错

**`O_NONBLOCK`  如果path引用的是一个FIFO、一个块特殊文件或一个字符特殊文件，则设置此文件本次打开操作跟后续的I/O操作为非阻塞方式**

`O_SYNC`  使每次write等待物理I/O操作完成

**`O_TRUNC`  如果此文件存在，而且为只写或读写成功打开，则将其长度截断为0**

`O_TTY_INIT`  如果打开一个还未打开的终端设备，设置非标准`termios`参数值

#### 2.2 create函数

```c
#include <fcntl.h>
int creat(const char *path,mode_t mode);
```

该函数相当于`open(path,O_WRONLY|O_CREATE|O_TRUNC,mode);`

在第四章我们会详细说明文件访问权限，并说明如何指定mode

#### 2.3 **close函数** 

```c
#include <unistd.h>
int close(int fd);
```
 close函数用于关闭一个打开文件 

#### 2.4 **lseek函数** 

```c
#include <unistd.h>
off_t lseek(int fd,off_t offset,int whence);
```

对参数offset的解释与参数whence的值有关。

**若`whence`是`SEEK_SET`，则将该文件的偏移量设置为距文件开始处`offset`个字节**

**若`whence`是`SEEK_CUR`，则将该文件的偏移量设置为其当前值加`offset`，`offset`可为正或负**

**若`whence`是`SEEK_END`，则将该文件的偏移量设置为文件长度加`offset`，`offset`可正可负**

若`lseek`成功执行，则返回新的文件偏移量，为此可以用下列方式确定打开文件的当前偏移量

```c
off_t currpos;
currpos=lseek(fd,0,SEEK_CUR);
```

如果文件描述符指向的是一个管道、FIFO、或者网络套接字，则`lseek`返回-1，并将`errno`设置为`ESPIPE`。 

#### 2.5  **read函数** 

```c
#include <unistd.h>
ssize_t read(int fd,void *buf,size_t nbytes);
```

如read成功，则返回读到的字节数。如已到达文件的尾端，则返回0。

读操作从文件的当前偏移量处开始，在成功返回之前，该偏移量将增加实际读到的字节数。

#### 2.6  **write函数** 

```C
#include <unistd.h>
ssize_t write(int fd,const void *buf,size_t nbytes);
```

其返回值通常与参数`nbytes`的值相同，否则表示出错。write出错的一个常见的原因是磁盘已写满，或者超过了一个给定进程的文件长度限制。

对于普通文件，写操作从文件的当前偏移量处开始，如果打开该文件时，指定了`O_APPEND`选项，则在每次写操作之前，将文件偏移量设置在文件的当前结尾处。

在一次成功写之后，该文件偏移量增加实际写得字节数。

#### 2.7 **文件共享** 

 进程为打开文件维护3张表 

<img src=".\img_apue\2_1.png" style="zoom:50%;" />

 如果两个独立进程各自打开了同一个文件，则有下图所示的关系 

<img src=".\img_apue\2_2.png" style="zoom:50%;" />

`digit1>&digit2`表示要将描述符`digit1`重定向至描述符`digit2`的同一文件

理解`./a.out > outfile 2>&1`与`./a.out 2>&1 >outfile`的区别

#### **2.8 fcnt函数** 

```c
#include <fcntl.h>
int fcntl(int fd,int cmd,.../* int arg */);
```

`fcntl`函数可以改变已经打开文件的属性，它有以下5种功能

1 复制一个已有的描述符（`cmd=F_DUPFD`或`F_DUPFD_CLOEXEC`）

2 获取/设置文件描述符标志（`cmd=F_GETFD`或`F_SETFD`）

3 获取/设置文件状态标志（`cmd=F_GETFL`或`F_SETFL`）

4 获取/设置异步I/O所有权（`cmd=F_GETOWN`或`F_SETOWN`）

5 获取/设置记录锁（`cmd=F_GETLK`、`F_SETLK`或`F_SETLKW`）

我们先说明`cmd`中的前面8种

`F_DUPFD`  复制文件描述符`fd`，返回新的文件描述符。它是尚未打开的各描述符中大于或等于第三个参数值中各值的最小值

`F_DUPFD_CLOEXEC`  复制文件描述符，设置与新描述符关联的`FD_CLOEXEC`文件描述符标志的值，返回新的文件描述符

`F_GETFD` 对应于`fd`的文件描述符标志做为函数值返回，当前只定义了一个文件描述符标志`FD_CLOEXEC`

`F_SETFD`  对于`fd`设置文件描述符标志。

`F_GETFL`  对应于`fd`的文件状态标志作为函数值返回，下图列出`fcntl`的文件状态标志

<img src=".\img_apue\3_1.png" style="zoom:50%;" />

其中，3种访问方式标志（`O_RDONLY`,`O_WRONLY`,`O_RDWR`）并不各占一位，因此首先必须用屏蔽字`O_ACCMODE`取得访问方式位，然后将结果与这3个值得每一个比较。

`F_SETFL` 将文件状态标志设置为第三个参数的值。可以更改的几个标志是`O_APPEND`、`O_NONBLOCK`、`O_SYNC`、`O_DSYNC`、`O_RSYNC`、`O_FSYNC`和`O_ANSYNC`

`F_GETTOWN` 获取当前接收`SIGIO`和`SIGURG`信号的进程ID或进程组ID。

`F_SETOWN`  设置接收`SIGIO`和`SIGURG`信号的进程ID或进程组ID

```c
#include "apue.h"
#include <fcntl.h>

int
main(int argc, char *argv[])
{
    int        val;

    if (argc != 2)
        err_quit("usage: a.out <descriptor#>");

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
        err_sys("fcntl error for fd %d", atoi(argv[1]));

    switch (val & O_ACCMODE) {
    case O_RDONLY:
        printf("read only");
        break;

    case O_WRONLY:
        printf("write only");
        break;

    case O_RDWR:
        printf("read write");
        break;

    default:
        err_dump("unknown access mode");
    }

    if (val & O_APPEND)
        printf(", append");
    if (val & O_NONBLOCK)
        printf(", nonblocking");
    if (val & O_SYNC)
        printf(", synchronous writes");

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC)
        printf(", synchronous writes");
#endif

    putchar('\n');
    exit(0);
}
```

 下面函数用于设置文件状态标志 

```c
#include "apue.h"
#include <fcntl.h>

void
set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int        val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");

    val |= flags;        /* turn on flags */

    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}
```

## 第四章 文件和目录

本章将描述文件系统的其他特性和文件的性质。 

#### **4.1 函数stat、fstat、fstatat和lstat** 

```c
#include <sys/stat.h>
int stat(const char *restrict pathname,struct stat *restrict buf);
int fstat(int fd,struct stat *buf);
int lstat(const char *restrict pathname,struct stat *restrict buf);
int fstatat(int fd,char *restrict pathname,struct stat *restrict buf,int flag);
```

一旦给出`pathname`，`stat`函数将返回与此命名文件相关的信息结构（存储在`struct stat`）

`fstat`函数获得已在描述符`fd`上打开文件的有关信息

`lstat`函数类似于`stat`，但是当命名文件时一个符号链接时，`lstat`返回该符号链接的有关信息，而不是该符号链接引用的文件的信息（符号链接有点像windows上的快捷方式）

stat结构基本形式如下：

```c
struct stat {
               dev_t     st_dev;     /* ID of device containing file */
               ino_t     st_ino;     /* inode number */
               mode_t    st_mode;    /* protection */
               nlink_t   st_nlink;   /* number of hard links */
               uid_t     st_uid;     /* user ID of owner */
               gid_t     st_gid;     /* group ID of owner */
               dev_t     st_rdev;    /* device ID (if special file) */
               off_t     st_size;    /* total size, in bytes */
               blksize_t st_blksize; /* blocksize for file system I/O */
               blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
               time_t    st_atime;   /* time of last access */
               time_t    st_mtime;   /* time of last modification */
               time_t    st_ctime;   /* time of last status change */
};
```

#### **4.2 文件类型** 

1 普通文件  包含某种形式的数据（文本或者二进制数据）

2 目录文件  包含了其他文件的名字以及指向这些文件有关信息的指针

3 块特殊文件  这种类型的文件提供对设备（如磁盘）带缓冲的访问，每次访问以固定长度为单位进行

4 字符特殊文件  这种类型的文件提供对设备不带缓冲的访问，每次访问长度可变，系统中的所有设备要么是字符特殊文件，要么是块特殊文件

5 FIFO  这种类型的文件用于进程间通信，有时也称为命名管道

6 套接字  这种类型的文件用于进程间的网络通信

7 符号链接  这种类型的文件指向另一个文件

 文件类型信息包含在stat结构的st_mode成员中，可以用宏确定文件类型，下面程序将打印出文件信息 

```c
#include "apue.h"

int
main(int argc, char *argv[])
{
    int            i;
    struct stat    buf;
    char        *ptr;

    for (i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);
        if (lstat(argv[i], &buf) < 0) {
            err_ret("lstat error");
            continue;
        }
        if (S_ISREG(buf.st_mode))
            ptr = "regular";
        else if (S_ISDIR(buf.st_mode))
            ptr = "directory";
        else if (S_ISCHR(buf.st_mode))
            ptr = "character special";
        else if (S_ISBLK(buf.st_mode))
            ptr = "block special";
        else if (S_ISFIFO(buf.st_mode))
            ptr = "fifo";
        else if (S_ISLNK(buf.st_mode))
            ptr = "symbolic link";
        else if (S_ISSOCK(buf.st_mode))
            ptr = "socket";
        else
            ptr = "** unknown mode **";
        printf("%s\n", ptr);
    }
    exit(0);
}
```

#### **4.3 设置用户ID和设置组ID** 

 与一个进程相关联的ID有6个或更多，如下图所示 

<img src=".\img_apue\4_1.png" style="zoom:50%;" />

####  **4.4 umask函数** 

 `umask`函数为进程设置文件模式创建屏蔽字，并返回之前的值 

```c
#include <sys/stat.h>
mode_t umask(mode_t cmast);
```

参数`cmask`是上节列出的9个常量中的按若干个按位“或”组成的

下面程序创建两个程序，演示了`umask`函数的作用

```c
#include "apue.h"
#include <fcntl.h>

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int
main(void)
{
    umask(0);
    if (creat("foo", RWRWRW) < 0)
        err_sys("creat error for foo");
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (creat("bar", RWRWRW) < 0)
        err_sys("creat error for bar");
    exit(0);
}
```

我们用shell的umask命令在运行程序前后打印文件模式创建屏蔽字，可见更改进程的文件模式创建屏蔽字并不影响父进程（常常是shell）的屏蔽字

用户可以设置umask值以控制他们所创建文件的默认权限。该值表示成八进制数，一位代表一种屏蔽的权限，如下图所示

<img src=".\img_apue\4_2.png" style="zoom:50%;" />

#### **4.5 函数chmod、fchmod和fchmodat** 

```c
#include <sys/stat.h>
int chmod(const char *pathname,mode_t mode);
int fchmod(int fd,mode_t mode);
int fchmodat(int fd,const char *pathname,mode_t mode,int flag);
```

`chmod`函数在指定的文件上进行操作，而`fchmod`函数则对已打开的文件进行操作。

参数mode是下图所示常量的按位或

<img src=".\img_apue\4_3.png" style="zoom:50%;" />

 下面程序修改了上节两个文件的模式 

```c
#include "apue.h"

int
main(void)
{
    struct stat        statbuf;

    /* turn on set-group-ID and turn off group-execute */

    if (stat("foo", &statbuf) < 0)
        err_sys("stat error for foo");
    if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        err_sys("chmod error for foo");

    /* set absolute mode to "rw-r--r--" */

    if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
        err_sys("chmod error for bar");

    exit(0);
}
```

#### **4.6 函数chown、fchown、fchownat和lchown** 

 下面几个函数可用户更改文件的用户ID和组ID 

```c
#include <unistd.h>
int chown(const char *pathname,uid_t owner,gid_t group);
int fchown(int fd,uid_t owner,gid_t group);
int fchownat(int fd,const char *pathname,uid_t owner,gid_t group,int flag);
int lchown(const char *pathname,uid_t owner,gid_t gruop);
```

#### **4.7 文件系统** 

我们可以把一个磁盘分成一个或多个分区，每个分区可以包含一个文件系统，如下图所示。i节点是固定长度的记录项，它包含有关文件的大部分信息。 

<img src=".\img_apue\4_4.png" style="zoom:50%;" />

 把i结点跟数据块分离出来，则可以看到下图所示的情况 

<img src=".\img_apue\4_5.png" style="zoom:50%;" />

图中两个目录项指向同一个i节点。每个i节点都有一个链接计数，其值是指向该i节点的目录项数。只有当链接计数减少至0时，才可删除文件。

i节点包含了文件有关的所有信息：文件类型、文件的访问权限、文件长度和指向文件数据块的指针等。stat结构中大所属信息取自i节点。

######  **函数link、linkat、unlink、unlinkat和remove** 

 创建一个指向现有文件的链接的方法是使用`link`函数或`linkat`函数 

```c
#include <unistd.h>
int link(const char *existringpath,const char *newpath);
int linkat(int efd,const char *existingpath,int nfd,const char *newpath,int flag);
```

这两个函数创建一个新目录项`newpath`，它引用现有文件`existringpath`。只创建`newpath`最后一个分量，路径中的其他部分应当已经存在。

为了删除一个现有的目录项，可以调用unlink函数

```c
#include <unistd.h>
int unlink(const char *pathname);
int unlink(int fd,const char *pathname,int flag);
```

这两个函数删除目录项，并将由pathname所引用文件的链接计数减1，只有当链接计数达到0时，该文件的内容才可被删除。

我们也可以用remove函数解除对一个文件或目录的链接。

```c
#include <stdio.h>
int remove(const char *pathname);
```

###### **函数rename和renameat** 

 文件或目录可以用`rename`函数或者`renameat`函数进行重命名 

```c
#include <stdio.h>
int rename(const char *oldname,const char *newname);
int renameat(int oldfd,const char *oldname,int newfd,const char *newname);
```

###### **符号链接**

符号链接是对一个文件的间接指针，下图列出各个函数对符号链接的处理

<img src=".\img_apue\4_6.png" style="zoom:50%;" />

######  **创建和读取符号链接** 

 下面函数创建一个指向`actualpath`的新目录项`sympath`。 

```c
#include <unistd.h>
int symlink(const char *actualpath,const char *sympath);
int symlinkat(const char *actualpath,int fd,const char *sympath);
```

 因为open函数跟随符号链接，所以需要有一种方法打开该符号链接，并读取链接中的名字。下面函数提供了这种功能 

```c
#include <unistd.h>
ssize_t readlink(const char *restrict pathname,char *restrict buf,size_t bufsize);
ssize_t readkinkat(int fd,const char *restrict pathname,char *restrict buf,size_t bufsize);
```

 两个函数组合了open、read和close的所有操作 

####  **文件的时间** 

 对每个文件维护3个时间字段，它们的意义如下 

<img src=".\img_apue\4_7.png" style="zoom:50%;" />

 下图列出我们已说明的各种函数对这3个时间的作用。 

<img src=".\img_apue\4_8.png" style="zoom:50%;" />

####  **函数futimens、utimensat和utimes** 

一个文件的访问和修改时间可以用以下几个函数更改

```c++
#include <sys/stat.h>
int futimens(int fd,const struct timespec time[2]);
int utimensat(int fd,const char *path,const struct timespec times[2],int flag);
```

这两个函数的times数组的第一个元素包含访问时间、第二个元素包含修改时间。

`timespec`结构如下

```c
struct timespec {
        time_t tv_sec;        /* seconds */
        long   tv_nsec;       /* nanoseconds */
};
```

如果times参数是一个空指针，则访问时间和修改时间两者都设置为当前时间。

`utimes`函数对路径名进行操作

```c
#include <sys/time.h>
int utimes(const char *pathname,const struct timeval times[2]);
```

`timeval`结构如下

```c
struct timeval {
       time_t tv_sec;        /* seconds */
       long tv_usec;       /* microseconds */
};
```

#### **函数mkdir、mkdirat和rmdir**

用mkdir和mkdirat函数创建目录，用rmdir函数删除目录

```c
#include <sys/stat.h>
int mkdir(const char *pathname,mode_t mode);
int mkdirat(int fd,const char *pathname,mode_t mode);
```

用rmdir函数删除一个空目录。空目录是只包含.和..这两项的目录

```c
#include <unistd.h>
int rmdir(const char *pathname);
```

**读目录**

对某个目录具有访问权限的任一用户都可以读该目录，一个目录的写权限位和执行权限位决定了在该目录中能否创建文件以及删除文件。

下面所示的是对读目录的一些函数

```c
#include <dirent.h>
DIR *opendir(const char *pathname);
DIR *fdopendir(int fd);

struct dirent *readdir(DIR *dp);

void rewinddir(DIR *dp);
int closedir(DIR *dp);

long telldir(DIR *dp);

void seekdir(DIR *dp,long loc);
```

dirent结构与实现有关，实现对此结构所做的定义至少包含下列两个成员：

```c
ino_t d_ino;  /* i-node number */
char d_name[]  /* null-terminated filename */
```

#### **函数chdir、fchdir和getcwd**

进程可以调用chdir或fchidr函数更改当前工作目录

```c
#include <unistd.h>
int chdir(const char *pathname);
int fchdir(int fd);
```

跟umask函数一样，执行程序前后shell的当前工作目录并没有改变。

下面函数可以得到当前目录完整的绝对路径名

```c
#include <unistd.h>
char *getcwd(char *buf,size_t size);
```

## 第五章 标准I/O

本章讲述标准I/O库

#### **流和FILE对象**

对于标准I/O库，它们的操作是围绕流进行的。流的定向决定了所读、写的字符是单字节还是多字节的。

```
#include <stdio.h>
#include <wchar.h>
int fwide(FILE *fp,int mode);
```

fwide函数可用于流的定向。根据mode参数的不同值，fwide函数执行不同的工作

若mode参数值为负，fwide将试图使指定的流是字节定向的

若mode参数值为正，fwide将试图使指定的流是宽定向的

若mode参数值是0，fwide将不试图设置流的定向，但返回标志该流定向的值

当一个流最初被创建时，它并没有定向。若在未定向的流上使用一个多字节（单字节）I/O函数，则将该流设置为宽（字节）定向。fwide并不改变已定向流的定向。

 

#### **标准输入、标准输出和标准错误**

对一个进程预定义了3个流：stdin、stdout、stderr。

 

#### **缓冲**

标准I/O提供了以下3种类型的缓冲

1 全缓冲，在这种情况下，在填满标准I/O缓冲区后才进行实际I/O操作。如磁盘上的文件通常实施全缓冲。

2 行缓冲，在这种情况下，当在输入和输出中遇到换行符，标准I/O执行I/O操作。如标准输入输出。

3 不带缓冲，标准I/O库不对字符进行缓冲存储。标准错误流stderr通常是不带缓冲的。

我们可以调用下面两个函数更改缓冲类型

```
#include <stdio.h>
void setbuf(FILE *restrict fp,char *restrict buf);
int setvbuf(FILE *restrict fp,char *buf,int mode,size_t size);
```

可以使用setbuf打开或关闭缓冲机制，参数buf指定一个长度为BUFSIZ的缓冲区。将buf设置为NULL可以关闭缓冲。

使用setvbuf，我们可以精确地说明所需的缓冲类型。这是用mode参数实现的：

_IOFBF  全缓冲

_IOLBF  行缓冲

_IONBF  不带缓冲

我们可以通过fflush函数冲洗一个流

```c
#include <stdio.h>
int fflush(FILE *fp);
```

#### **打开流**

下面3个函数打开一个标准I/O流

```c
#include <stdio.h>
FILE *fopen(const char *restrict pathname,const char *restrict type);
FILE *freopen(const char *restrict pathname,const char *restrict type,FILE *restrict fp);
FILE *fdopen(int fd,const char *type);
```

type参数指定对该I/O流的读、写方式，ISO C规定type参数可以有如下15种不同的值

<img src=".\img_apue\5_1.png" style="zoom:50%;" />

其中b作为type的一部分，这使得标准I/O系统可以区分文本文件和二进制文件

调用fclose关闭一个打开的流

```c
#include <stdio.h>
int fclose(FILE *fp);
```

#### **读和写流**

以下3个函数可用于一次读一个字符

```
#include <stdio.h>
int getc(FILE *fp);
int fgetc(FILE *fp);
int getchar(void);   //等同于getc(stdin)
```

对应上面所述的每个输入函数都有一个输出函数

```
#include <stdio.h>
int putc(int c,FILE *fp);
int fputc(int c,FILE *fp);
int putchar(int c);
```

每次一行I/O

下面两个函数提供每次输入一行的功能

```
#include <stdio.h>
char *fgets(char *restrict buf,int n,FILE *restrict fp); //buf为缓冲区地址，读入的行将送入其中，参数n指定缓冲的长度
char *gets(char *buf);  //不推荐使用
```

fputs和puts提供每次输出一行的功能

```
#include <stdio.h>
int fputs(const char *restrict str,FILE *restrict fp);
int puts(const char *str);
```

#### **二进制I/O**

下列两个函数执行二进制I/O操作

```c
#include <stdio.h>
size_t fread(void *restrict ptr,size_t size,size_t nobj,FILE *restrict fp);
size_t fwrite(const void *restrict ptr,size_t size,size_t nobj,FILE *restrict fp);
```

 参数size为欲写结构的长度，nobj为欲写的元素个数，函数返回的是读或写的对象数。这些函数有以下两种常见的用法 

```c
float data[10];
if(fwrite(&data[2],sizeof(float),4,fp)!=4)
    err_sys("fwrite error");
```

```c
//读写一个结构
struct{
      short count;
      long total;
      char name[NAMESIZE];  
}item;

if(fwrite(&item,sizeof(item),1,fp)!=1)
     err_sys("fwrite error");
```

#### **定位流**

有3种方法定位标准I/O流

```c
#include <stdio.h>
long ftell(FILE *fp);   //若成功，则返回当前文件位置指示，出错则返回-lL
int fseek(FILE *fp,long offset,int whence);  
void rewind(FILE *fp);
```

除了偏移量的类型是off_t而非long以外，ftello函数与ftell相同，fseeko函数与fseek相同

```c
#include <stdio.h>
off_t ftello(FILE *fp);
int fseeko(FILE *fp,off_t offset,int whence);
```

下面函数是ISO C标准引入的

```c
#include <stdio.h>
int fgetpos(FILE *restrict fp,fpos_t *restrict pos);
int fsetpos(FILE *fp,const fpos_t *pos);
```

#### **格式化I/O**

格式化输出是由printf函数处理的

```c
#include <stdio.h>
int printf(const char *restrict format,...);
int fprintf(FILE *restrict fp,const char *restrict format,...);
int dprintf(int fd,const char *restrict format,...);

int sprintf(char *restrict buf,const char *restrict format,...);
int snprintf(char *restrict buf,size_t n,const char *restrict format,...);    //参数n指定缓冲区长度
```

执行格式化输入处理的是3个scanf函数

```c
#include <stdio.h>
int scanf(const char *restrict format,...);
int fscanf(FILE *restrict fp,const char *restrict format,...);
int sscanf(const char *restrict buf,const char *restrict format,...);
```



#### **临时文件**

ISO C标准I/O库提供了两个函数以帮助创建临时文件

```c
#include <stdio.h>
char *tmpnam(char *ptr);
FILE *tmpfile(void);
```

tmpnam函数产生一个与现有文件名不同的一个有效路径名字符串

tmpfile创建一个临时二进制文件（类型wb+），在关闭该文件或程序结束时将自动删除。

## 第六章 系统数据文件和信息

 UNIX系统的正常运作需要使用大量与系统有关的数据文件，例如，口令文件`/etc/passwd`和组文件`/etc/group`就是经常被多个程序频繁使用的两个文件。 

#### **口令文件**

UNIX系统口令文件包含如下字段，这些字段包含在`<pwd.h>`中定义的`passwd`结构中

<img src=".\img_apue\6_1.png" style="zoom:50%;" />

口令文件是`/etc/passwd`，每一行包含上面各字段，字段之间用冒号分隔。可以使用finger命令打印指定用户的有关信息：finger -p 用户名

`POSIX.1`定义了两个获取口令文件项的函数，在给定用户登录名或数值用户ID后，这两个函数就能查看相关项

```c
#include <pwd.h>
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);
```

 下面两个函数可以实现查看整个口令文件 

```c
#include <pwd.h>
struct passwd *getpwent(void);
void setpwent(void);
void endpwent(void);
```

 调用getpwent时，它返回口令文件中的下一个记录项。函数setpwent反绕它所使用的文件（定位到文件开始处），endpwent则关闭这些文件，下面给出使用这3个函数实现getpwnam函数的程序 

```c
#include <pwd.h>
#include <stddef.h>
#include <string.h>

struct passwd *
getpwnam(const char *name)
{
    struct passwd  *ptr;

    setpwent();
    while ((ptr = getpwent()) != NULL)
        if (strcmp(name, ptr->pw_name) == 0)
            break;        /* found a match */
    endpwent();
    return(ptr);    /* ptr is NULL if no match found */
}
```

#### **阴影文件**

现在，某些系统将加密口令存放在另一个通常称为阴影口令的文件中:/etc/shadow

与访问口令文件的一组函数相类似，有另一组函数可用于访问阴影口令文件

```
#include <shadow.h>
struct spwd *getspnam(cosnt char *name);
struct spwd *getspent(void);
void setspent(void);
void endspent(void);
```

#### **组文件**

UNIX组文件包含下图所示字段（/etc/group文件中的字段），这些字段包含在<grp.h>中所定义的group结构中。

<img src=".\img_apue\6_2.png" style="zoom:50%;" />

字段gr_mem是一个指针数组，其中每一个指针指向一个属于该组的用户名。该数组以null指针结尾。

可以使用下列两个函数来查看组相关项

```c
#include <grp.h>
struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char *name);
```

 类似于口令文件，如果需要搜索整个组文件，可以使用下面3个函数 

```
#include <grp.h>
struct group *getgrent(void);
void setgrent(void);
void endgrent(void);
```

**附属组ID**

当用户登录时，系统就按口令文件记录项中的数值组ID，赋给它实际组ID。在任何时刻执行newgrp更改组ID

一个用户可能参与多个项目，因此也就要同时属于多个组，使用附属组ID的优点是不必再显式地经常更改组。

为了获取和设置附属组ID，提供了下列3个函数

```c
#include <unistd.h>
int getgroups(int gidsetsize,gid_t grouplist[]);
int setgroups(int ngroups,const gid_t grouplist[]);
int init groups(const char *username,gid_t basegid);
```

getgroups将进程所属用户的各附属组ID填写到数组grouplist中，最多为gidsetsize个

setgroups可由超级用户调用以便为调用进程设置附属组ID表。

通常，只有initgroups函数调用setgroups，用来设置用户的附属组ID。

其他数据文件

下图给出访问系统数据文件的一些例程

<img src=".\img_apue\6_3.png" style="zoom:50%;" />

对于这些数据文件的接口都都与上述对口令文件和组文件的相似，一般情况下，对于每个数据文件至少提供一下3个函数

1 get函数：读下一个记录

2 set函数：打开相应数据文件，然后反绕该文件

3 end函数：关闭相应数据文件

**系统标识**

POSIX.1定义了uname函数，它返回与主机和操作系统有关的信息

```c
#include <sys/utsname.h>
int uname(struct utsname *name);
```

POSIX.1定义了该结构中最少需提供的字段

```
 struct utsname {
         char sysname[];    /* Operating system name (e.g., "Linux") */
         char nodename[];   /* Name within "some implementation-defined network" */
         char release[];    /* Operating system release (e.g., "2.6.28") */
         char version[];    /* Operating system version */
         char machine[];    /* Hardware identifier */
}
```

#### **时间和日期例程**

time函数返回当前时间和日期。它是自公元1970年1月1日00:00:00以来经过的秒数（日历时间）

```
#include <time.h>
time_t time(time_t *calptr);
```

如果参数非空，则时间值也存放在由calptr指向的单元内。

下面两个函数将日历时间转换成分解的时间，并将这些存放在一个tm结构中

```
#include <time.h>
struct tm *gmtime(const time_t *calptr);
struct tm *localtime(const time_t *calptr);
```

```c
struct tm {
         int tm_sec;         /* seconds [0-60] */
         int tm_min;         /* minutes  [0-59] */
         int tm_hour;        /* hours [0-23] */
         int tm_mday;        /* day of the month [1-31] */
         int tm_mon;         /* month [0-11] */
         int tm_year;        /* year since 1900*/
         int tm_wday;        /* day of the week [0-6]*/
         int tm_yday;        /* day in the year [0-365]*/
         int tm_isdst;       /* daylight saving time */
};
```

mktime函数将tm结构转换成time_t值

```c
#include <time.h>
time_t mktime(struct tm *tmptr);
```

strftime用于格式化时间值

```c
#include <time.h>
size_t strftime(char *restrict buf,size_t maxsize,const char *restrict format,const struct tm *restrict tmptr);
size_t strftime_l(char *restrict buf,size_t maxsize,const char *restrict format,const struct tm *restrict tmptr,locale_t locale);
```

 下面列出format参数的格式 

<img src=".\img_apue\6_4.png" style="zoom: 80%;" />

`strptime`是`strftime`的反过来的版本，将字符串时间转换成分解时间

```
#include <time.h>
char *strptime(const char *restrict buf,const char *restrict format,struct tm *restrict tmptr);
```

## 第七章 进程环境

#### **main函数**

C程序总是从main函数开始执行，main函数的原型是：

```
int main(int argc,char *argv[]);
```

其中，argc是命令行参数的数目，argv是指向参数的各个指针所构成的数组。

#### **进程终止**

3个函数用于正常终止一个程序：_exit和_Exit立即进入内核，exit则先执行一些清理处理（对于所有打开流调用fclose函数），然后返回内核。

```
#include <stdlib.h>
void exit(int status);
void _Exit(int status);
#include <unistd.h>
void _exit(int status);
```

3个退出函数都带一个整形参数，称为终止状态。main函数返回一个整形值与用该值调用exit是等价的：exit(0),return(0)。

shell中echo $?可以打印上次运行程序的终止码。

#### atexit函数

```
#include <stdlib.h>
int atexit(void (*func)(void));
```

按照ISO C规定，一个进程可以登记多至32个函数，这些函数将由exit自动调用。我们称这些函数为终止处理程序，并调用atexit函数来登记这些函数。

下图显示了一个C程序时如何启动的，以及它终止的各种方式

<img src=".\img_apue\7_1.png" style="zoom: 67%;" />

#### **命令行参数**

当执行一个程序时，调用exec的进程可将命令行参数传递给该新程序。下面所示的程序将其所有命令行参数都回显到标准输出上。

```c
#include "apue.h"

int
main(int argc, char *argv[])
{
    int        i;

    for (i = 0; i < argc; i++)        /* echo all command-line args */
        printf("argv[%d]: %s\n", i, argv[i]);
    exit(0);
}
```

#### **环境表**

每个程序都接收到一张环境表。与参数表一样，环境表也是一个字符指针数组（以null结束），全局变量environ则包含了该指针数组的地址：

```
extern char **environ;
```

通常使用getenv和putenv函数来访问特定的环境变量。

####  **C程序的存储空间布局**

C程序由下列几个部分组成：

1 正文段。这是CPU执行的机器指令部分。

2 初始化数据段。通常称为数据段，它包含了程序中需明确地赋初值的变量。（全局变量）

3 未初始化数据段。通常称为bss段，在程序开始执行之前，内核将此段中的数据初始化为0或空指针。（全局变量）

4 栈。自动变量以及每次函数调用时所需保存的信息存放在此段中。

5 堆。通常在堆中进行动态存储分配。

size命令报告正文段、数据段和bss段的长度（以字节为单位）

#### **共享库**

共享库使得可执行文件不再需要包含公用的库函数，而只需在所有进程都可饮用的存储区中保存这种库例程的一个副本。

程序第一次执行或第一次调用某个库函数时，用动态链接的方法将程序和共享库函数相链接。

共享库的另一个优点是可以用库函数的新版本代替老版本而无需对使用该库的程序重新链接编辑。

#### **存储空间分配**

ISO C说明了3个用于存储空间动态分配的函数。

```
#include <stdlib.h>
void *malloc(size_t size); //分配指定字节数的存储区。此存储区中的初始值不确定。
void *calloc(size_t nobj,size_t size);  //为指定数量指定长度的对象分配存储空间。该空间中的每一位都初始化为0
void *realloc(void *ptr,size_t newsize);  //增加或减少以前分配去的长度，新增区域内的初始值不确定。
void free(void *ptr);  //释放分配的存储区
```

#### **环境变量**

环境字符串的形式是：name=value

下面函数用于获取跟修改环境变量

```c
#include <stdlib.h>
char *getenv(const char *name);  //返回指向与name关联的value的指针；若未找到，返回NULL
int putenv(char *str); //参数为形式为name=value的字符串，如果name已经存在，则先删除其原来的定义
int setenv(const char *name,const char *value,int rewrite); //将name设置为value，如果name已经存在：(a)rewrite非0，则首先删除其现有的定义 (b)rewrite为0，不删除其现有定义，不设置新的value
int unsetenv(const char *name); //删除name的定义
```

**函数setjmp和longjmp**

setjmp和longjmp函数可以实现函数之间的跳转

```
#include <setjmp.h>
int setjmp(ump_buf env);
void longjmp(jmp_buf env,int val);  //val将成为setjmp处返回的值
```

**函数getrlimt和setrlimit**

每个进程都有一组资源限制，其中一些可以用下面两个函数查询和更改。

```
#include <sys/resource.h>
int getrlimit(int resource,struct rlimit *rlptr);
int setrlimit(int resource,const struct rlimit *rlptr);

struct rlimit {
        rlim_t rlim_cur;  /* Soft limit */
        rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
};
```

这两个函数的resource参数可取下列值之一

RLIMIT_CORE   core文件的最大字节数，若其值为0则阻止创建core文件。

RLIMIT_CPU   CPU时间的最大量值（秒），当超过此软限制时，向该进程发送S I G X C P U信号。

RLIMIT_DATA   一个进程的[数据段](http://baike.baidu.com/view/1005328.htm)最大字节长度。数据段中初始化数据、非初始化数据以及堆的总和。当调用函数[brk](http://baike.baidu.com/view/653956.htm)动态改变一个进程的数据段大小时，若失败，errno值将被设置为ENOMEM。

RLIMIT_FSIZE   可以创建的文件的最大字节长度。当超过此软限制时，则向该进程发送SIGXFSZ信号。

RLIMIT_NOFILE   每个进程能打开的最多文件数。

RLIMIT_STACK   栈的最大字节长度。系统不会动态增加栈的大小限制。

RLIMIT_VMEM  可映照[地址空间](http://baike.baidu.com/view/1507129.htm)的最大字节长度。

RLIMIT_AS  进程可用内存最大字节数。

## 第八章 进程控制

 本章介绍UNIX系统的进程控制，包括创建新进程、执行程序和进程终止。 

#### **进程标识**

每一个进程都有一个非负整数表示的唯一进程ID，除了进程ID，每个进程还有一些其他标识符。下列函数返回这些标识符

```
#include <unistd.h>
pid_t getpid(void);
pid_t getppid(void);
uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
```

#### **函数fork**

一个现有的进程可以调用fork函数创建一个新进程

```
#include <unistd.h>
pid_t fork(void);
```

由fork创建的新进程被称为子进程。**fork函数被调用一次，但是返回两次**：

子进程的返回值是0，父进程的返回值则是新建子进程的进程ID。

fork之后是父进程还是子进程先执行是不确定的，这取决于内核所使用的调度算法。

**子进程和父进程继续执行fork调用之后的指令，子进程获得父进程数据空间、堆和栈的副本。注意，父子进程并不共享这些存储空间部分，只共享正文段。**

#### **函数vfork**

vfork函数用于创建一个新进程，而该新进程的目的是exec一个新程序。

vfork与fork一样都创建一个子进程，但是它并不将父进程的地址空间完全复制到子进程中，因为子进程会立即调用exec（或exit）。

不过在子进程调用exec或exit之前，它在父进程的空间中运行。

vfork和fork之间的另一个区别是：vfork保证子进程先运行，在它调用exec或exit之后父进程才可能被调度运行。

#### **函数wait和waitpid**

```
#include <sys/wait.h>
pid_t wait(int *statloc);
pid_t waitpid(pid_t pid,int *statloc,int options);
```

这两个函数的区别如下：

在一个子进程终止前，wait使其调用者阻塞，而waitpid有一选项，可使调用者不阻塞。

waitpid并不等待在其调用之后的第一个终止子进程，它有若干个选项，可以控制它所等待的进程。

对于waitpid函数中pid参数的作用解释如下：

pid==-1  等待任一子进程。与wait等效

pid>0  等待进程ID与pid相等的子进程

pid==0  等待组ID等于调用进程组ID的任一子进程

pid<-1  等待组ID等于pid绝对值的任一子进程

options参数使我们进一步控制waitpid的操作。此参数或者是0，或者是下图中常量按位或运算的结果

![](.\img_apue\8-1.png)

#### **函数exec**

有7种不同的exec函数可供使用，当进程调用一种exec函数时，该进程执行的程序完全替换成新程序。

```
#include <unistd.h>
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg,..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[],char *const envp[]);
int fexecve(int fd,char *const argv[],char *const envp[]);
```

path指定路径名为参数，当指定file作为参数时：

1 如果file中包含/，则就将其视为路径名

2 否则就按PATH环境变量，在它指定的各目录中搜寻可执行文件

7个exec函数之间的区别

![](.\img_apue\8-2.png)

 这7个函数只有execve是内核的系统调用。另外6个只是库函数，最终都要调用该系统调用。下图是这7个函数之间的关系 

<img src=".\img_apue\8-3.png" style="zoom:75%;" />

 

####  **函数system**

```
#include <stdlib.h>
int system(coast char *cmdstring);
```

## 第九章 进程关系

本章将详细地说明进程组以及POSIX.1引入的会话的概念。还将介绍登录shell和所有从登录shell启动的进程之间的关系

 

#### **终端登录**

BSD终端登录。系统管理者创建通常名为/etc/ttys的文件，其中每个终端设备都有一行，用来说明设备名和传到getty程序的参数。

当系统自举时，内核创建进程ID为1的进程（init进程）。init进程读取文件/etc/ttys，对每一个允许登录的终端设备调用一次fork，它所生成的子进程则exec getty程序，如下图所示：

<img src=".\img_apue\9-1.png" style="zoom: 50%;" />

 getty对终端设备调用open函数，以读、写方式将终端打开。一旦设备被打开，则文件描述符0、1、2就被设置到该设备。然后getty输出“login：”之类的信息，并等待用户键入用户名。

当用户键入用户名后，getty的工作就完成了。然后它以类似于下列方式调用login程序：

```
execle("/bin/login","login","-p",username,(char *)0,envp);
```

下图显示了login刚被调用后这些进程的状态

<img src=".\img_apue\9-2.png" style="zoom:50%;" />

如果用户正确登录，login就将完成如下工作：

1 将当前工作目录更改为该用户的起始目录（chdir）

2 调用chown更改该终端的所有权，是登录用户成为它的所有者

3 将该终端设备的访问权限改变成“用户读和写”

4 调用setgid及initgroups设置进程的组ID

5 用login得到的所有信息初始化环境：起始目录（HOME）、shell（SHELL）、用户名（USERNAME和LOGNAME）以及一个系统默认路径（PATH）

6 login进程更改为登录用户的用户ID（setuid）并调用该用户的登录shell，其方式类似于：execl("/bin/sh","-sh",(char *)0)

####  **网络登录**

在上节所述的终端登录中，init知道哪些终端设备科用来进行登录，并为每个设备生成一个getty进程。但是对网络登录情况则有所不同，事先并不知道将会有多少这样的登录。

BSD网络登录。作为系统启动的一部分，init调用一个shell，使其执行shell脚本/etc/rc。由此shell脚本启动一个守护进程inetd。

inetd等待TCP/IP连接请求到达主机，而当一个连接请求到达时，它执行一次fork，然后生成的子进程exec适当的程序。

TELNET是使用TCP协议的远程登录应用程序。下图显示了在执行TELNET服务进程中所涉及的进程序列

<img src=".\img_apue\9-3.png" style="zoom: 33%;" />

然后，telnetd进程打开一个伪终端设备，并用fork分成两个进程。父进程处理通过网络连接的通信，子进程则执行login程序。

 

 

#### **进程组**

每个进程除了有一进程ID之外，还属于一个进程组，进程组是一个或多个进程的集合，每个进程组有一个唯一的进程组ID。

可以使用getpgrp返回调用进程的进程组ID

```
#include <unistd.h>
pid_t getpgrp(void);
```

每个进程组有一个组长进程。组长进程的进程组ID等于其进程ID

进程调用setpgid可以加入一个现有的进程组或者创建一个新进程组

```
#include <unistd.h>
int setgpid(pid_t pid,pid_t pgid);
```

setpgid函数将pid进程的进程组ID设置为pgid。如果这两个参数相等，则由pid指定的进程变成进程组组长。

一个进程只能为它自己或它的子进程设置进程组ID。在它的子进程调用exec后，它就不再更改该子进程的进程组ID。

####  **会话**

会话是一个或多个进程组的集合。

进程调用setsid函数建立一个新会话。

```
#include <unistd.h>
pid_t setsid(void);
```

如果调用此函数的进程不是一个进程组的组长，则此函数创建一个新会话。具体会发生以下3件事。

1 该进程变成新会话的会话首进程

2 该进程成为一个新进程组的组长进程。新进程组ID是该调用进程的进程ID

3 该进程没有控制终端

如果该调用进程已经是一个进程组的组长，则此函数返回出错。getsid函数返回会话首进程的进程组ID

```
#include <unistd.h>
pid_t getsid(pid_t pid);  //如果pid是0，函数返回调用进程的会话首进程的进程组ID
```

#### **控制终端**

会话和进程组还有一些其他特性

1 一个会话可以有一个控制终端。这通常是终端设备（在终端登录情况下）或伪终端设备（在网络登录情况下）。

2 建立与控制终端连接的会话首进程被称为控制进程。

3 一个会话的几个进程组可被分成一个前台进程组以及一个或多个后台进程组。

4 如果一个会话有一个控制终端，则它有一个前台进程组，其他进程组为后台进程组。

5 无论何时键入终端的中断键（常常是Delete或Ctrl+C），都会将终端信号发送至前台进程组的所有进程。

5 无论何时键入终端的退出键（常常是Ctrl+\），都会讲退出信号发送至前台进程组的所有进程。

6 如果终端接口检测到调制解调器（或网络）已经断开，则将挂断信号发送至终端进程（会话首进程）

这些特性示于下图中

<img src=".\img_apue\9-4.png" style="zoom:75%;" />

**作业控制**

**一个作业只是几个进程的集合**，通常是一个进程管道。

例如：vi main.c 在前台启动了只有一个进程组成的作业。

而下面命令（命令后面加&可以是作业在后台运行）：

pr *.c | lpr &

make all &

在后台启动了两个作业。这连个后台作业调用的所有进程都在后台运行。

 

当启动一个后台作业时，shell赋予它一个作业标识符，并打印一个或多个进程ID。下面演示这一点：

<img src=".\img_apue\9-5.png" style="zoom:50%;" />

当作业完成而且键入回车时，shell通知作业已经完成。

 

 

只有前台作业接收终端输入，如果后台作业试图读终端，则向后台作业发送一个特定信号SIGTTIN。

该信号通常会停止此后台作业，而shell则向有关用户发送这种情况的通知，然后用户就可用shell命令将此作业转为前台作业运行，于是它就可读终端。下面将演示这一点：

<img src=".\img_apue\9-6.png" style="zoom:50%;" />

 使用stty命令可以允许或禁止后台作业输出到控制终端 

<img src=".\img_apue\9-7.png" style="zoom:50%;" />

## 第十章 信号

本章先对信号机制进行综述，并说明每种信号的一般用法。

 

#### **信号概念**

每个信号都有一个名字，这些名字都以3个字符SIG开头。在头文件<signal.h>中，信号名都被定义为正整形常量。

在某个信号出现时，可以按下列3种方式之一进行处理：

1 忽略该信号。大多数信号都可以使用这种方式进行处理，但有两种信号却决不能被忽略：SIGKILL和SIGSTOP（只能执行系统默认动作）。

2 捕获信号。通知内核在某信号发生时，调用一个用户函数对这种时间进行处理。

3 执行系统默认动作。对于大多数信号的系统默认动作是终止该进程。

 

#### **信号类型**

程序错误类信号：默认动作使进程流产，产生core文件。

SIGABRT：  调用abort函数生成的信号。

SIGFPE：   浮点计算错误。

SIGILL：   非法指令错误。

SIGBUS/SIGSEGV： 硬件错误-非法地址访问。

SIGEMT：  硬件错误

SIGSYS:    非法系统调用。

SIGTRAP：  硬件错误（通常为断点指令）。

 

程序终止类信号：默认动作使进程终止，我们通常要处理这类信号，做一些清理工作，句柄函数应在结束时为此信号指定默认动作，然后再次生成该信号，使得程序终止。

SIGHUP：终端断开连接时，生成此信号给控制进程。

SIGINT：Ctrl-C或Delete按下时，由终端驱动生成，并发送给前台进程组中的所有进程。

SIGKILL：使程序立即终止，不能被捕获或忽略，也不能被阻塞。

SIGQUIT：Ctrl-\,如SIGINT，并且产生core。

SIGTERM：该信号使程序终止，但是可以阻塞、捕获、忽略。

 

闹钟类信号：通知定时器到期，默认动作是终止程序，但通常会设置句柄。

SIGALRM：alarm/setitimer函数设置定时到期后，会产生此信号。

SIGPROF：

SIGVTALRM：

 

I/O类信号：通知进程在描述字上发生了感兴趣事件，支持信号驱动IO。

SIGIO： fd准备执行输入输出时发送此信号。

SIGPOLL：异步I/O信号。

SIGURG：网络收到带外数据时可选择生成此信号。

 

作业控制类信号：

SIGCHLD：  进程终止或停止时会向其父进程发送该信号，默认动作为忽略。

SIGCONT:   使停止的进程恢复运行。

SIGSTOP:   停止进程。

SIGTSTP/SIGTTIN/SIGTTOU:

 

操作错误类信号：默认动作终止程序。

SIGPIPE：  管道破裂。

SIGXCPU/SIGXFSZ：

#### **函数signal**

UNIX系统信号机制最简单的接口是signal函数

```
#include <signal.h>
void (*signal(int signo,void (*func)(int)))(int);
```

可以使用typedef使其变得简单一点

```
typedef void Sigfunc(int);Sigfunc *signal(int,Sigfunc *);
```

第一个int参数是要捕获的信号（整形常量），第二个参数是一个函数指针（处理函数），该函数指针指向的函数返回值是void，参数是int。

下面给出一个简单得信号处理程序：

```
#include "apue.h"

static void    sig_usr(int);    /* one handler for both signals */

int
main(void)
{
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        err_sys("can't catch SIGUSR1");
    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        err_sys("can't catch SIGUSR2");
    for ( ; ; )
        pause();
}

static void
sig_usr(int signo)        /* argument is signal number */
{
    if (signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if (signo == SIGUSR2)
        printf("received SIGUSR2\n");
    else
        err_dump("received signal %d\n", signo);
}
```

 我们使该程序在后台运行，而且用kill命令将信号发送给它： 

<img src=".\img_apue\10-1.png" style="zoom:50%;" />

#### **中断的系统调用**

早期UNIX系统的一个特性是：如果进程在执行一个低速系统调用而阻塞期间捕获到一个信号，则该系统调用就被中断不再继续执行。

该系统调用返回出错，其errno设置为EINTR。后面的章节会更多的涉及到被中断的系统调用。

#### **可重入函数**

进程捕获到信号并对其进行处理时，进程正在执行的正常指令序列就被信号处理程序临时中断，它首先执行该信号处理程序中的指令。

如果从信号处理程序返回，则继续执行在捕获到信号时进程正在执行的正常指令序列。

在信号处理函数中调用某些函数可能对导致安全问题（其结果是不可预知的），下面列出了这些异步信号安全的函数，没有列入图中的大多数函数是不可重入的。

<img src=".\img_apue\10-2.png" style="zoom:75%;" />

#### **函数kill和raise**

kill函数将信号发送给进程或进程组，raise函数则允许进程向自身发送信号。

```
#include <signal.h>
int kill(pid_t pid,int signo);
int raise(int signo);
```

#### **函数alarm和pause**

函数alarm设置一个定时器，当定时器超时时，产生SIGALRM信号。

```
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
```

pause函数使调用进程挂起直至捕捉到一个信号

```
#include <signal.h>
int pause(void);
```

**只有执行了一个信号处理程序并从其返回时，pause才返回。**此时，pause返回-1，errno设置为EINTR。

#### **信号集**

信号集是能表示多个信号的数据结构（sigset_t），下面列出5个处理信号集的函数

```
#include <signal.h>
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set,int signo);
int sigdelset(sigset_t *set,int signo);
int siggismember(const sigset_t *set,int signo);
```

在使用信号集之前，要对该信号集进行初始化（调用sigemptyset或者sigfillset）。

#### **函数sigprocmask**

进程的信号屏蔽字规定了当前阻塞而不能递送给该进程的信号集。调用函数sigprocmask可以检测或更改进程的信号屏蔽字。

```
#include <signal.h>
int sigprocmask(int how,const sigset_t *restrict set,sigset_t *restrict oset);
```

若oset是非空指针，那么进程的当前信号屏蔽字通过oset返回。

若set是一个非空指针，则参数how指示如何修改当前信号屏蔽字。SIG_BLOCK是或操作，SIG_SETMASK则是赋值操作

<img src=".\img_apue\10-3.png" style="zoom:50%;" />

#### **函数sigpending**

sigpending函数返回一信号集，对于调用进程而言，其中的各信号是阻塞不能递送的，因而也一定是当前未决的。

```
#include <signal.h>
ing sigpending(sigset_t *set);
```

下面展示信号设置和sigprocmask实例

```
#include "apue.h"

static void    sig_quit(int);

int
main(void)
{
    sigset_t    newmask, oldmask, pendmask;

    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        err_sys("can't catch SIGQUIT");

    /*
     * Block SIGQUIT and save current signal mask.
     */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");

    sleep(5);    /* SIGQUIT here will remain pending */

    if (sigpending(&pendmask) < 0)
        err_sys("sigpending error");
    if (sigismember(&pendmask, SIGQUIT))
        printf("\nSIGQUIT pending\n");

    /*
     * Restore signal mask which unblocks SIGQUIT.
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
    printf("SIGQUIT unblocked\n");

    sleep(5);    /* SIGQUIT here will terminate with core file */
    exit(0);
}

static void
sig_quit(int signo)
{
    printf("caught SIGQUIT\n");
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
        err_sys("can't reset SIGQUIT");
}
```

进程开始阻塞SIGQUIT信号，保存了当前信号屏蔽字（以便以后恢复），然后休眠5秒。在此期间所产生的退出信号SIGQUIT都被阻塞，不递送至该进程。

5秒休眠后，检查该信号是否是未决的，然后将SIGQUIT设置为不再阻塞。

运行程序，在5s之内键入退出字符Ctril+\（产生SIGQUIT信号），然后在第二个5s之内再次键入退出字符。

<img src=".\img_apue\10-4.png" style="zoom:50%;" />

**函数sigaction**

sigaction函数的功能是检查或修改与制定信号相关联的处理动作。此函数取代了UNIX早期版本使用的signal函数。

```
#include <signal.h>
int sigaction(int signo,const struct sigction *restrict act,struct sigaction *restrict oact);
```

参数signo是要检测或修改其具体动作的信号编号。若act指针非空，则根据参数act修改其动作。若oact指针非空，则由oact指针返回该信号的上一个动作。

此函数使用下列结构：

```
struct sigaction {
        void     (*sa_handler)(int);
        void     (*sa_sigaction)(int, siginfo_t *, void *);
        sigset_t   sa_mask;
        int        sa_flags;
};
```

sa_handler字段包含一个信号捕捉函数的地址。

sa_mask字段说明了一个信号集，在调用该信号捕捉函数之前，这一信号集要加到进程的信号屏蔽字中。仅当从信号捕捉函数返回时将进程的信号屏蔽字恢复为原先值。

sa_flags字段指定对信号进行处理的各个选项。

<img src=".\img_apue\10-5.png" style="zoom:50%;" />

 

sa_sigaction字段是一个替代的信号处理程序，当sa_flags设置为SA_SIGINFO时，使用该信号处理程序。

通常按下列方式调用信号处理程序：

void handler(int signo);

在设置了SA_SIGINFO标志，那么按下列凡是调用信号处理程序：

void handler(int signo,siginfo_t *info,void *context);

下面使用sigaction实现signal函数，它力图阻止被中断的系统调用重启动

```
typedef void Sigfunc(int);Sigfunc* mysignal(int signo,Sigfunc *func)
{     struct sigaction    act,oact;
     act.sa_handler = func;  
     sigemptyset(&act.sa_mask);  
     act.sa_flags = 0;
     if(signo == SIGALRM)   
     {
 #ifdef SA_INTERRUPT
         act.sa_flags |= SA_INTERRUPT;
 #endif
     }
     else   
     {
 #ifdef  SA_RESTART
         act.sa_flags |= SA_RESTART;
 #endif
     }
     if(sigaction(signo,&act,&oact)<0)
         return (SIG_ERR);
     return (oact.sa_handler);
 }
```

#### **函数sigsetjmp和siglongjmp**

之前说明了setjmp和longjmp函数可以用户非局部转移，sigsetjmp跟siglongjmp指定了对信号屏蔽字的作用。

在信号处理程序中进行非局部转移时应当使用这两个函数。

```
#include <setjmp.h>
int sigsetjmp(sigjmp_buf env,int savemask);
void siglongjmp(sigjmp_buf env,int val);
```

与setjmp和longjmp函数唯一的区别是sigsetjmp增加了一个参数savemask。

如果savemask非0，则sigsetjmp在env中保存在env中保存进程的当前信号屏蔽字。调用siglongjmp时，从已经保存的env中恢复保存的信号屏蔽字。

#### **函数sigsuspend**

sigsuspend用于在接收到某个信号之前，**临时用sigmask替换进程的信号屏蔽字，并暂停进程执行，直到捕捉到一个信号而且从该信号处理程序返回**，并且进程的信号屏蔽字设置为调用sigsuspend之前的值。

```
#include <signal.h>
int sigsuspend(const sigset_t *sigmask);
```

下面显示了保护代码临界区，使其不被特定信号中断的正确方法

```
#include "apue.h"

static void    sig_int(int);

int
main(void)
{
    sigset_t    newmask, oldmask, waitmask;

    pr_mask("program start: ");

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal(SIGINT) error");
    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    /*
     * Block SIGINT and save current signal mask.
     */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");

    /*
     * 代码临界区
     */
    pr_mask("in critical region: ");

    /*
     * Pause, allowing all signals except SIGUSR1.
     */
    if (sigsuspend(&waitmask) != -1)
        err_sys("sigsuspend error");

    pr_mask("after return from sigsuspend: ");

    /*
     * Reset signal mask which unblocks SIGINT.
     */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");

    /*
     * And continue processing ...
     */
    pr_mask("program exit: ");

    exit(0);
}

static void
sig_int(int signo)
{
    pr_mask("\nin sig_int: ");
}
```

 下面是程序运行结果： 

<img src=".\img_apue\10-6.png" style="zoom:50%;" />

#### **函数abort**

abort函数的功能是使程序异常终止

```
#include <stdlib.h>
void abort(void);
```

此函数将SIGABRT信号发送给调用进程。让进程捕捉SIGABRT信号目的是在进程终止之前由其执行所需的清理操作。默认情况是终止调用进程。

#### **函数system**

POSIX.1要求system函数忽略SIGINT和SITQUIT信号，阻塞SIGCHLD。

#### **函数sleep**

此函数使调用进程被挂起，直到满足下列条件之一：

（1）已经经过seconds所指定的墙上时钟时间。

（2）调用进程捕捉到一个信号并从信号处理程序返回。



## 第十一章 线程

本章将进一步深入理解进程，了解如何使用多个控制线程（简单得说就是线程）在单进程环境中执行多个任务。

 

 

#### **线程概念**

每个线程都包含有表示执行环境所必须的信息：线程ID、一组寄存器值、栈、调度优先级和策略、信号屏蔽字、errno变量以及线程私有数据。

一个进程的所有信息对该进程的所有线程都是共享的，包括可执行程序的代码、程序的全局内存和堆内存、栈以及文件描述符。

 

 

#### **线程标识**

每个线程都有一个线程ID，线程ID只有在它所属的进程上下文中才有意义。

可以使用下面函数来对两个线程ID进行比较

```
#include <pthread.h>
int pthread_equal(pthread_t tid1,pthread_t tid2);
```

可以通过pthread_self函数获得自身的线程ID

```
#include <pthread.h>
pthread_t pthread_self(void);
```

#### **线程创建**

```
#include <pthread.h>
int pthread_create(pthread_t *restrict tidp,const pthread_attr_t *restrict attr,void *(*start_rtn)(void *),void *restrict arg);
```

当pthread_create成功返回时，新创建线程的线程ID会被设置成tidp指向的内存空间。

attr属性用于定制各种不同的线程属性。

新创建的线程从start_rtn函数的地址开始运行，该函数只有一个无类型指针参数arg。

下面程序将演示线程的创建，打印出进程ID、新线程的线程ID以及初始线程的线程ID：

```
#include "apue.h"
#include <pthread.h>

pthread_t ntid;

void
printids(const char *s)
{
    pid_t        pid;
    pthread_t    tid;

    pid = getpid();
    tid = pthread_self();
    printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
      (unsigned long)tid, (unsigned long)tid);
}

void *
thr_fn(void *arg)
{
    printids("new thread: ");
    return((void *)0);
}

int
main(void)
{
    int        err;

    err = pthread_create(&ntid, NULL, thr_fn, NULL);
    if (err != 0)
        err_exit(err, "can't create thread");
    printids("main thread:");
    sleep(1);
    exit(0);
}
```

线程创建时并不能保证哪个线程先会运行：是新创建的线程，还是调用线程。本程序让主线程休眠，确保新线程有机会运行。

#### **线程终止**

如果进程中任意线程调用了exit、_Exit或者_exit，那么整个进程就会终止。

单个线程可以通过3种方式退出，因此可以在不终止整个进程的情况下，停止它的控制流。

1 线程可以简单地从启动例程中返回，返回值的线程的退出码。

2 线程可以被同一进程中的其他线程取消。

3 线程调用pthread_exit。

```
#include <pthread.h>
void pthread_exit(void *rval_ptr);
```

rval_ptr参数是一个无类型指针，进程中的其他线程也可以通过调用pthread_join函数访问到这个指针

```
#include <pthread.h>
int pthread_join(pthread_t thread,void **rval_ptr);
```

调用pthread_join后，调用线程将一直阻塞，直到指定的线程退出。

如果线程简单地从它的启动例程返回，rval_ptr将包含返回码。如果线程被取消，由rval_ptr指定的内存单元就设置成PTHREAD_CANCELED。

下面演示如何获取已终止线程的退出码：

```
#include "apue.h"
#include <pthread.h>

void *
thr_fn1(void *arg)
{
    printf("thread 1 returning\n");
    return((void *)1);
}

void *
thr_fn2(void *arg)
{
    printf("thread 2 exiting\n");
    pthread_exit((void *)2);
}

int
main(void)
{
    int            err;
    pthread_t    tid1, tid2;
    void        *tret;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)
        err_exit(err, "can't create thread 1");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)
        err_exit(err, "can't create thread 2");
    err = pthread_join(tid1, &tret);
    if (err != 0)
        err_exit(err, "can't join with thread 1");
    printf("thread 1 exit code %ld\n", (long)tret);
    err = pthread_join(tid2, &tret);
    if (err != 0)
        err_exit(err, "can't join with thread 2");
    printf("thread 2 exit code %ld\n", (long)tret);
    exit(0);
}
```

线程可以通过调用pthread_cancel函数来请求取消同一进程中的其他进程。

```
#include <pthread.h>
int pthread_cancel(pthread_t tid);
```

pthread_cancel并不等待线程终止，它仅仅提出请求，线程可以选择忽略取消或者控制如何被取消。

线程可以安排它退出时需要调用的函数，这与进程在退出时可以用atexit函数安排退出时类似的。

**如果线程是通过从它的启动例程中退出返回而终止的话，它的清理处理程序就不会被调用。**

```
#include <pthread.h>
void pthread_cleanup_push(void (*rtn)(void *),void *arg);
void pthread_cleanup_pop(int execute);
```

如果execute参数设置为非0，则调用并删除上次pthread_cleanup_push调用建立的清理处理程序。

如果execute参数为0，则清理函数将不被调用（只删除）。

我们可以调用pthread_detach分离线程。

```
#include <pthread.h>
int pthread_detach(pthread_t tid);
```

#### **线程同步**

当一个线程可以修改的变量，其他线程可以读取或者修改的时候，我们就需要对这些线程进行同步，确保他们在访问变量的存储内容时不会访问到无效的值。

为了解决这个问题，线程不得不使用锁，同一时间只允许一个线程访问该变量。

#### **互斥量**

可以使用pthread的互斥接口来保护数据，确保同一时间只有一个线程访问数据。

互斥量从本质上说是一把锁，在访问共享资源前对互斥量进行设置（加锁），在访问完成后释放（解锁）互斥量。

互斥变量使用pthread_mutex_t数据类型表示的。在使用之前，必须对它进行初始化，如果动态分配互斥量，在释放内存前需要调用pthread_mutex_destroy。

```
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

要用默认的属性初始化互斥量，只需把attr设为NULL，也可以把互斥量设置为常量PTHREAD_MUTEX_INITIALIZER（只适用于静态分配的互斥量）进行初始化。

互斥量有以下3种功能

```
#include <pthread.h>
int pthread_mutex_lock(pthread_mutex_t *mutex); 
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

可以使用pthread_mutex_lock对互斥量进行加锁，如果互斥量已经上锁，调用线程将阻塞直到互斥量被解锁。

可以使用pthread_mutex_unlock对互斥量解锁。

如果不希望被阻塞，可以使用pthread_mutex_trylock尝试对互斥量进行加锁。如果互斥量处于未锁住状态，则锁住互斥量，否则返回EBUSY。

#### **避免死锁**

如果线程试图对同一个互斥量加锁两次，那么它自身就会陷入死锁状态。

如果两个线程以相反的顺序锁住两个互斥量，也会导致死锁，两个线程都无法向前运行。

在同时需要两个互斥量时，让他们以相同的顺序加锁，这样可以避免死锁。

#### **函数pthread_mutex_timedlock**

与pthread_mutex_lock不同的是，pthread_mutex_timedlock允许绑定线程阻塞时间，如果超过时间值，pthread_mutex_timedlock不会对互斥量进行加锁，而是返回错误码ETIMEDOUT。

```
#include <pthread.h>
#include <time.h>
int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,const struct timespec *restrict tsptr);
```

下面给出如何用pthread_mutex_timedlock避免永久阻塞

```
#include "apue.h"
#include <pthread.h>

int
main(void)
{
    int err;
    struct timespec tout;
    struct tm *tmp;
    char buf[64];
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&lock);
    printf("mutex is locked\n");
    clock_gettime(CLOCK_REALTIME, &tout);
    tmp = localtime(&tout.tv_sec);
    strftime(buf, sizeof(buf), "%r", tmp);
    printf("current time is %s\n", buf);
    tout.tv_sec += 10;    /* 10 seconds from now */
    /* caution: this could lead to deadlock */
    err = pthread_mutex_timedlock(&lock, &tout);
    clock_gettime(CLOCK_REALTIME, &tout);
    tmp = localtime(&tout.tv_sec);
    strftime(buf, sizeof(buf), "%r", tmp);
    printf("the time is now %s\n", buf);
    if (err == 0)
        printf("mutex locked again!\n");
    else
        printf("can't lock mutex again: %s\n", strerror(err));
    exit(0);
}
```

 这个程序对已有的互斥量加锁，演示了pthread_mutex_timedlock是如何工作的。 

#### **读写锁**

读写锁与互斥量类似，不过读写锁允许更高的并行性。

读写锁可以有3种状态：读模式下加锁状态，写模式下加锁状态，不加锁状态。

**一次只有一个线程可以占有写模式的读写锁，但是多个线程可以同时占有读模式的读写锁。**

\1. 当读写锁是写加锁状态时，在这个锁被解锁之前，所有试图对这个所加锁的线程都会被阻塞。

\2. 当读写锁是读加锁状态时，所有试图以读模式对它进行加锁的线程都可以得到访问权，但是任何希望以写模式对此进行加锁的线程都会阻塞，知道所有的线程释放它们的读锁为止。

读写锁在使用之前必须初始化，在释放他们底层的内存之前必须销毁。

```
#include <pthread.h>
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
```

下面是读写锁的3种用法

```
#include <pthread.h>
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

与互斥量一样，读写锁定义了下面两个函数

```
#include <pthread.h>
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrwrock(pthread_rwlock_t *rwlock);
```

#### **带有超时的读写锁**

与互斥量一样，有两个带有超时的速写锁加锁函数

```
#include <pthread.h>
#include <time.h>
int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock,const struct timespec *restrict tsptr);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock,const struct timespec *restrict tsptr);
```

#### **条件变量**

在使用条件变量之前，必须对它进行初始化，在释放底层的内存空间之前，可以使用pthread_cond_destroy函数对条件变量进行反初始化

```
#include <pthread.h>
int pthread_cond_init(pthread_cond_t *restrict cond,const pthread_condattr_t *restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);
```

条件本身是由互斥量保护的。线程在改变条件状态之前必须首先锁住互斥量，然后调用下面函数等待条件变量为真。

```c
#include <pthread.h>
int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex,const struct timespec *restrict tsptr);
```

调用者把锁住的互斥量传给函数，**函数自动把调用线程放到等待条件的线程列表上，对互斥量解锁。pthread_cond_wati返回时，互斥量再次被锁住。**

pthread_cond_timedwait则添加了一个超时值，如果超过到期时条件还是没有出现，则函数重新获取互斥量，然后返回ETIMEDOUT。

两个函数调用成功返回时，线程需要重新计算条件，因为另一个线程可能已经在运行并改变条件。

下面函数用于通知线程条件已经满足：

```
#include <pthread.h>
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

phread_cond_signal函数至少能唤醒一个等待该条件的线程，而pthread_cond_broadcast函数则能唤醒等待该条件的所有线程。

下面将结合条件变量和互斥量对线程进行同步

```c
#include <pthread.h>

struct msg {
    struct msg *m_next;
    /* ... more stuff here ... */
};

struct msg *workq;

pthread_cond_t qready = PTHREAD_COND_INITIALIZER;

pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void
process_msg(void)
{
    struct msg *mp;

    for (;;) {
        pthread_mutex_lock(&qlock);
        while (workq == NULL)
            pthread_cond_wait(&qready, &qlock);
        mp = workq;
        workq = mp->m_next;
        pthread_mutex_unlock(&qlock);
        /* now process the message mp */
    }
}

void
enqueue_msg(struct msg *mp)
{
    pthread_mutex_lock(&qlock);
    mp->m_next = workq;
    workq = mp;
    pthread_mutex_unlock(&qlock);
    pthread_cond_signal(&qready);
}
```

#### **自旋锁**

自旋锁与互斥量类似，但它不是通过休眠使进程阻塞，而是在获取锁之前一直处于忙等（自旋）阻塞状态。

自旋锁可用于以下情况：锁被持有的时间短，而且线程并不希望在重新调度上花费太多的成本。

自旋锁的接口与互斥量的接口类似，提供了以下的5个函数。

```c
#include <pthread.h>
int pthread_spin_init(pthread_spinlock_t *lock,int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);

int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
```

#### **屏障**

屏障是用户协调多个线程并行工作的同步机制。

屏障允许每个线程等待，直到有的合作线程到达某一点，然后从该点继续执行。pthread_join函数就是一种屏障，允许一个线程等待，直到另一个线程退出。

可以使用下面函数对屏障进行初始化跟反初始化

```
#include <pthread.h>
int pthread_barrier_init(pthread_barrier_t *restrict barrier,const pthread_barrierattr_t *restrict attr,unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
```

count参数可以用来指定在允许所有线程继续运行之前，必须到达屏障的线程数目。

可以使用pthread_barrier_wait函数来表明，线程已经完成工作，准备等所有其他线程赶上来

```
#include <pthread.h>
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

调用pthread_barrier_wait的线程在屏障计数（调用pthread_barrier_init时设定）未满足条件时，会进入休眠状态。

如果该线程是最后一个调用pthread_barrier_wait的线程，就满足了屏障计数，所有的线程都被唤醒。

下面给出在一个任务上合作的多个线程之间如何用屏障进行同步

```
#include "apue.h"
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>

#define NTHR   8                /* number of threads */
#define NUMNUM 8000000L            /* number of numbers to sort */
#define TNUM   (NUMNUM/NTHR)    /* number to sort per thread */

long nums[NUMNUM];
long snums[NUMNUM];

pthread_barrier_t b;

#ifdef SOLARIS
#define heapsort qsort
#else
extern int heapsort(void *, size_t, size_t,
                    int (*)(const void *, const void *));
#endif

/*
 * Compare two long integers (helper function for heapsort)
 */
int
complong(const void *arg1, const void *arg2)
{
    long l1 = *(long *)arg1;
    long l2 = *(long *)arg2;

    if (l1 == l2)
        return 0;
    else if (l1 < l2)
        return -1;
    else
        return 1;
}

/*
 * Worker thread to sort a portion of the set of numbers.
 */
void *
thr_fn(void *arg)
{
    long    idx = (long)arg;

    heapsort(&nums[idx], TNUM, sizeof(long), complong);
    pthread_barrier_wait(&b);

    /*
     * Go off and perform more work ...
     */
    return((void *)0);
}

/*
 * Merge the results of the individual sorted ranges.
 */
void
merge()
{
    long    idx[NTHR];
    long    i, minidx, sidx, num;

    for (i = 0; i < NTHR; i++)
        idx[i] = i * TNUM;
    for (sidx = 0; sidx < NUMNUM; sidx++) {
        num = LONG_MAX;
        for (i = 0; i < NTHR; i++) {
            if ((idx[i] < (i+1)*TNUM) && (nums[idx[i]] < num)) {
                num = nums[idx[i]];
                minidx = i;
            }
        }
        snums[sidx] = nums[idx[minidx]];
        idx[minidx]++;
    }
}

int
main()
{
    unsigned long    i;
    struct timeval    start, end;
    long long        startusec, endusec;
    double            elapsed;
    int                err;
    pthread_t        tid;

    /*
     * Create the initial set of numbers to sort.
     */
    srandom(1);
    for (i = 0; i < NUMNUM; i++)
        nums[i] = random();

    /*
     * Create 8 threads to sort the numbers.
     */
    gettimeofday(&start, NULL);
    pthread_barrier_init(&b, NULL, NTHR+1);
    for (i = 0; i < NTHR; i++) {
        err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM));
        if (err != 0)
            err_exit(err, "can't create thread");
    }
    pthread_barrier_wait(&b);
    merge();
    gettimeofday(&end, NULL);

    /*
     * Print the sorted list.
     */
    startusec = start.tv_sec * 1000000 + start.tv_usec;
    endusec = end.tv_sec * 1000000 + end.tv_usec;
    elapsed = (double)(endusec - startusec) / 1000000.0;
    printf("sort took %.4f seconds\n", elapsed);
    for (i = 0; i < NUMNUM; i++)
        printf("%ld\n", snums[i]);
    exit(0);
}
```

 在这个实例中，使用8个线程分解了800万个数的排序工作。每个线程用堆排序算法对100万个数进行排序，然后主线程调用一个函数对这些结果进行合并。 

## 第十二章 线程控制

 本章将讲解控制线程行为方面的详细内容，而前面的章节中使用的都是它们的默认行为 

#### **线程属性**

pthread接口允许我们通过设置每个对象关联的不同属性来细调线程和同步对象的行为。
管理这些属性的函数都遵循相同的模式：
1.每个对象与自己类型的属性对象进行关联（线程与线程属性关联，互斥量与互斥量属性关联等）
2.有一个初始化函数，把属性设置为默认值
3.有一个销毁属性对象的函数
4.每个属性都有一个从属性对象中获取属性值的函数
5.每个属性都有一个设置属性值的函数

下面是线程属性pthread_attr_t的初始化跟销毁函数

```
#include <pthread.h>
int pthread_attr_t_init(pthread_attr_t *attr);
int pthread_attr_t_destroy(pthread_attr_t *attr);
```

下图总结了POSIX.1定义了线程的属性

<img src=".\img_apue\11-1.png" style="zoom:75%;" />

####  **每个属性都有相应的获取跟设置函数**

如果在创建线程时不需要了解线程的终止状态，就可以修改pthread_attr_t结构中detachstate线程属性，让线程一开始就处于分离状态
detachstate可以设置成两个合法值之一：PTHREAD_CREATE_DETACHED和PTHREAD_CREATE_JOINABLE（默认）。 

```
#include <pthread.h>
int pthread_attr_getdetachstate(const pthread_attr_t *restrict attr,int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr,int *detachstate);
```

 下面给出一个以分离状态创建线程的函数 

```c
#include "apue.h"
#include <pthread.h>

int
makethread(void *(*fn)(void *), void *arg)
{
    int                err;
    pthread_t        tid;
    pthread_attr_t    attr;

    err = pthread_attr_init(&attr);
    if (err != 0)
        return(err);
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err == 0)
        err = pthread_create(&tid, &attr, fn, arg);
    pthread_attr_destroy(&attr);
    return(err);
}
```

同样的，可以使用下面函数对线程栈属性进行管理

```
#include <pthread.h>
int pthread_attr_t_getstack(const pthread_attr_t *restrict attr,void **restrict stackaddr,size_t *restrict stacksize);
int pthread_attr_t_setstack(pthread_attr_t *attr,void *stackaddr,size_t stacksize);
```

可以使用下面函数来设置和读取线程属性stacksize

```
#include <pthread.h>
int pthread_attr_getstacksize(const pthread_attr_t *restrict attr,size_t *restrict stacksize);
int pthread_attr_setstacksize(pthread_attr_t *attr,size_t stacksize);
```

线程属性guardsize控制着线程栈末尾之后用以避免栈溢出的扩展内存的大小。

```
#include <pthread.h>
int pthread_attr_getguardsize(const pthread_attr_t *restrict attr,size_t *restrict guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr,size_t guardsize);
```

#### **同步属性**

跟线程具有属性一样，线程的同步对象也有属性。本节将讨论互斥量属性、读写锁属性、条件变量属性和屏障属性。

#### **互斥量属性**

初始化跟反初始化函数

```
#include <pthread.h>
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
```

值得注意的3个属性是：进程共享属性、健壮属性和类型属性。

下面两个函数用于设置跟获取进程共享属性

```
#include <pthread.h>
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict attr,int *restrict pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,int pshared);
```

pshared可以设置为两个值：PTHREAD_PROCESS_PRIVATE（默认），PTHREAD_PROCESS_SHARED（从多个进程彼此之间共享的内存数据块中分配的互斥量就可以用于这些线程的同步）

下面两个函数用于设置跟获取健壮属性

```
#include <pthread.h>
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict attr,int *restrict robust);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr,int robust);
```

健壮性属性取值有两种可能：

默认是PTHREAD_MUTEX_STALLED，这意味着持有互斥量的进程终止时不需要采取特别的动作；

另一个取值是PTHREAD_MUTEX_ROBUST，这个值导致线程调用pthread_mutex_lock获取锁，而该锁被另一个进程持有，但它终止时并没有对该锁进行解锁，此时线程会阻塞，从pthread_mutex_lock返回的值是EOWNERDEAD而不是0。

使用下面两个函数可以设置跟获取互斥量类型属性

```
#include <pthread.h>
int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr,int *restrict type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr,int type);
```

#### **读写锁属性**

初始化跟反初始化函数

```
#include <pthread.h>
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
```

读写锁支持的唯一属性是进程共享属性。它与互斥量的进程共享属性是相同的。

```
#include <pthread.h>
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *restrict attr,int *restrict pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr,int pshared);
```

#### **条件变量属性**

有一对函数用于初始化和反初始化条件变量属性

```
#include <pthread.h>
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destory(pthread_condattr_t *attr);
```

目前定义了条件变量的两个属性：进程共享属性和时钟属性

与其他的同步属性一样，条件变量支持进程共享属性，下面两个函数用于设置跟获取进程共享属性

```
#include <phread.h>
int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr,int *restrict pshared);
int pthread_condattr_setpshared(pthrea_condattr_t *attr,int *pshared);
```

时钟属性控制计算pthread_cond_timedwait函数的超时参数采用的是哪个时钟，合法值是下图列出的时钟ID

![](.\img_apue\11-2.png)

下面两个函数用于设置跟获取时钟属性

```
#include <pthread.h>
int pthread_condattr_getclock(const pthread_condattr_t *restrict attr,clockid_t *restrict clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr,clockid_t *clock_id);
```

#### **屏障属性**

```
#include <pthread.h>
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
```

目前定义的屏障属性只有进程共享属性，作用与其他同步对象一样

```
#include <pthread.h>
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *restrict attr,int *restrict pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr,int pshared);
```

#### **重入**

如果一个函数在相同的时间点可以被多个线程安全地调用，就称该函数是线程安全的。下图列出POSIX.1中不能保证线程安全的函数。

<img src=".\img_apue\11-3.png" style="zoom:50%;" />

支持线程安全函数的操作系统实现会在<unistd.h>中定义符号_POSIX_THREAD_SAFE_FUNCTIONS，

对POSIX.1中的一些非线程安全函数，它会提供可替代的线程安全版本。下面列出这些函数的线程安全版本。

<img src=".\img_apue\11-4.png" style="zoom:50%;" />

POSIX.1还提供了以线程安全的方式管理FILE对象的方法。可以使用flockfile和ftrylockfile获取给定FILE对象关联的锁。

这个锁是递归的：当你占有这把锁的时候，还是可以再次获取该锁，而且不会导致死锁。

```c
#include <stdio.h>
int ftrylockfile(FILE *fp);
void flockfile(FILE *fp);

void funlockfile(FILE *fp); 
```

下面显示了getenv的一个非可重入版本

```c
#include <limits.h>
#include <string.h>

#define MAXSTRINGSZ    4096

static char envbuf[MAXSTRINGSZ];

extern char **environ;

char *
getenv(const char *name)
{
    int i, len;

    len = strlen(name);
    for (i = 0; environ[i] != NULL; i++) {
        if ((strncmp(name, environ[i], len) == 0) &&
          (environ[i][len] == '=')) {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
            return(envbuf);
        }
    }
    return(NULL);
}
```

如果两个线程同时调用这个函数，将会看到不一致的结果。

下面演示getenv的可重入版本

```c
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

extern char **environ;

pthread_mutex_t env_mutex;

static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void
thread_init(void)
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&env_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

int
getenv_r(const char *name, char *buf, int buflen)
{
    int i, len, olen;

    pthread_once(&init_done, thread_init);
    len = strlen(name);
    pthread_mutex_lock(&env_mutex);
    for (i = 0; environ[i] != NULL; i++) 
    {
        if ((strncmp(name, environ[i], len) == 0) &&
          (environ[i][len] == '=')) 
          {
            olen = strlen(&environ[i][len+1]);
            if (olen >= buflen) 
            {
                pthread_mutex_unlock(&env_mutex);
                return(ENOSPC);
            }
            strcpy(buf, &environ[i][len+1]);
            pthread_mutex_unlock(&env_mutex);
            return(0);
        }
    }
    pthread_mutex_unlock(&env_mutex);
    return(ENOENT);
}
```

#### **线程特定数据**

线程特定数据，也称为线程私有数据，是存储和查询某个特定线程相关数据的一种机制。

在分配线程特定数据之前，需要创建与该数据关联的键。这个键将用于获取对线程特定数据的访问。使用pthread_key_create创建一个键

```
#include <pthread.h>
int pthread_key_create(pthread_key_t *keyp,void (*destructor)(void *));
```

创建的键存储在keyp指向的内存单元中，这个键可以被进程中的所有线程使用，但每个线程把这个键与不同的线程特定数据地址进行关联。

创建新建时，每个线程的数据地址设为空值。

pthread_key_create可以为该键关联一个可选择的析构函数。当这个线程退出时，如果数据地址已经被置为非空值，那么析构函数就会被调用。

线程通常使用malloc为线程特定数据分配内存。析构函数通常释放已分配的内存。

对于所有的线程，我们都可以通过调用pthread_key_delete来取消键与线程特定数据值之间的关联关系。

```
#include <pthread.h>
int pthread_key_delete(pthread_key_t key);
```

键一旦创建以后，就可以通过pthread_setspecific函数把键和线程特定数据关联起来，可以通过pthread_getspecific函数获得线程特定数据的地址。

```c
#include <pthread.h>
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key,const void *value);
```

下面演示使用线程特定数据来维护每个线程的数据缓冲区副本的getenv可重入实现。

```c
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAXSTRINGSZ    4096

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void
thread_init(void)
{
    pthread_key_create(&key, free);
}

char *
getenv(const char *name)
{
    int        i, len;
    char    *envbuf;

    pthread_once(&init_done, thread_init);
    pthread_mutex_lock(&env_mutex);
    envbuf = (char *)pthread_getspecific(key);
    if (envbuf == NULL) {
        envbuf = malloc(MAXSTRINGSZ);
        if (envbuf == NULL) {
            pthread_mutex_unlock(&env_mutex);
            return(NULL);
        }
        pthread_setspecific(key, envbuf);
    }
    len = strlen(name);
    for (i = 0; environ[i] != NULL; i++) {
        if ((strncmp(name, environ[i], len) == 0) &&
          (environ[i][len] == '=')) {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
            pthread_mutex_unlock(&env_mutex);
            return(envbuf);
        }
    }
    pthread_mutex_unlock(&env_mutex);
    return(NULL);
}
```

#### **取消选项**

有两个线程属性并没有包含在pthread_attr_t结构中，它们是可取消状态和可取消类型，这两个属性影响着线程在相应pthread_cancel函数调用时锁呈现的行为。

可取消状态属性可以是PTHREAD_CANCEL_ENABLE，也可以是PTHREAD_CANCLE_DISABLE。线程可以通过调用pthread_setcancelstate修改它的可取消状态。

```
#include <pthread.h>
int pthread_setcancelstat(int state,int *oldstate);
```

pthread_cancle调用并不等待线程终止。在默认情况下，线程在取消请求发出以后还是继续运行，知道线程达到某个取消点。

取消点是线程检查它是否被取消的一个位置，如果取消了，则按照请求行事。POSIX.1保证在线程调用如下列出的任何函数时，取消点就会出现。

<img src=".\img_apue\11-5.png" style="zoom:50%;" />

线程启动时默认的可取消状态时PTHREAD_CANCEL_ENABLE。当状态设为PTHREAD_CANCLE_DISABLE时，对pthread_cancel的调用并不会杀死线程。

取消请求对这个线程来说还处于挂起状态，当取消状态再次变为PTHREAD_CANCLE_ENABLE时，线程将在下一个取消点上对所有的取消请求进行处理。

可以调用pthread_testcancel函数在程序中添加自己的取消点。

```
#include <pthread.h>
void pthread_testcancel(void);
```

上面描述的默认取消类型是推迟取消。可以通过调用pthread_setcanceltype来修改取消类型。

```
#include <pthread.h>
int pthread_setcanceltype(int type,int *oldtype);
```

type参数可以是PTHREAD_CANCEL_DEFERRED（默认），也可以是PTHREAD_CANCEL_ASYNCHRONOUS（异步取消）。

如果使用异步取消。线程可以在任意时间撤销，而不是遇到取消点才能被取消。

#### **线程和信号**

信号的处理时进程中所有线程共享的。如果一个线程选择忽略某个给定信号，那么另一个线程就可以通过以下两种方式撤销上述线程的信号选择：

恢复信号的默认处理行为，或者为信号设置一个新的信号处理程序。

进程中的信号是递送给单个线程的。如果一个信号与硬件故障相关，那么该信号一般会被发送到引起该事件的线程中去，而其他的信号则被发送到任意一个线程。

第十章讨论了进程如何使用sigprocmask函数来阻止信号发送。而线程则必须使用pthread_sigmask。

```
#include <signal.h>
int pthread_sigmask(int how,const sigset_t *restrict set,sigset_t *restrict oset);
```

pthread_sigmask函数与sigprocmask函数基本相同，不过pthread_sigmask失败时返回错误码，不再像sigprocmask函数那样设置errno并返回-1。

线程可以通过sigwait等待一个或多个信号的出现。

```
#include <signal.h>
int sigwait(const sigset_t *restrict set,int *restrict signop); //发送信号的数量
```

要把信号发送给线程，可以调用pthread_kill。

```
#include <signal.h>
int pthread_kill(pthread_t thread,int signo);
```

#### **线程和fork**

当线程调用fork时，就为整个子进程创建了整个进程地址空间的副本，子进程会从父进程那儿继承了每个互斥量、读写锁和条件变量的状态。

要清除锁状态，可以通过调用pthread_atfork函数建立fork处理程序。

```c
#include <pthread.h>
int pthread_atfork(void (*prepare)(void),void (*parent)(void),void (*child)(void));
```

prepare处理程序在父进程fork创建子进程前调用，作用是获取父进程定义的所有锁。

parent处理程序是创建子进程之后、返回之前在父进程上下文中调用的，作用是对获取的所有锁进行解锁。

child处理程序在fork返回之前在子进程上下文中调用，跟parent处理程序一样，作用是对获取的所有锁进行解锁。

下面程序描述了如何使用它pthread_atfork和fork处理程序

```c
#include "apue.h"
#include <pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void
prepare(void)
{
    int err;

    printf("preparing locks...\n");
    if ((err = pthread_mutex_lock(&lock1)) != 0)
        err_cont(err, "can't lock lock1 in prepare handler");
    if ((err = pthread_mutex_lock(&lock2)) != 0)
        err_cont(err, "can't lock lock2 in prepare handler");
}

void
parent(void)
{
    int err;

    printf("parent unlocking locks...\n");
    if ((err = pthread_mutex_unlock(&lock1)) != 0)
        err_cont(err, "can't unlock lock1 in parent handler");
    if ((err = pthread_mutex_unlock(&lock2)) != 0)
        err_cont(err, "can't unlock lock2 in parent handler");
}

void
child(void)
{
    int err;

    printf("child unlocking locks...\n");
    if ((err = pthread_mutex_unlock(&lock1)) != 0)
        err_cont(err, "can't unlock lock1 in child handler");
    if ((err = pthread_mutex_unlock(&lock2)) != 0)
        err_cont(err, "can't unlock lock2 in child handler");
}

void *
thr_fn(void *arg)
{
    printf("thread started...\n");
    pause();
    return(0);
}

int
main(void)
{
    int            err;
    pid_t        pid;
    pthread_t    tid;

    if ((err = pthread_atfork(prepare, parent, child)) != 0)
        err_exit(err, "can't install fork handlers");
    if ((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0)
        err_exit(err, "can't create thread");

    sleep(2);
    printf("parent about to fork...\n");

    if ((pid = fork()) < 0)
        err_quit("fork failed");
    else if (pid == 0)    /* child */
        printf("child returned from fork\n");
    else        /* parent */
        printf("parent returned from fork\n");
    exit(0);
}
```

#### **线程与I/O**

第三章介绍了pread函数和pwrite函数，它们使偏移量和数据的读写成为一个原子操作。

## 第十三章 守护进程

本章将说明守护进程结构，以及如何编写守护进程程序。

守护进程，也就是通常说的Daemon进程，是Unix中的后台服务进程。它是一个生存期较长的进程，通常独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。

 

#### **编程规则**

在编写守护进程程序时需遵循一些基本规则，以防止产生不必要的交互作用。下面将说明这些规则。

1.调用umask将文件模式创建屏蔽字设置为一个已知值（通常是0）

2.调用fork，然后使父进程exit，保证了子进程不是进程组的组长进程，是下面进行setsid调用的先决条件

3.调用setsid创建一个新会话。然后它会执行3个步骤：（a）成为新会话的首进程 （b）成为一个新进程组的组长进程 （c）没有控制终端

4.将当前工作目录更改为根目录。

5.关闭不再需要的文件描述符

6.某些守护进程打开/dev/null使其具有文件描述符0、1、2

下面函数演示了创建守护进程的基本步骤

```c
#include "apue.h"
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

void
daemonize(const char *cmd)
{
    int                    i, fd0, fd1, fd2;
    pid_t                pid;
    struct rlimit        rl;
    struct sigaction    sa;

    /*
     * Clear file creation mask.
     */
    umask(0);

    /*
     * Get maximum number of file descriptors.
     */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        err_quit("%s: can't get file limit", cmd);

    /*
     * Become a session leader to lose controlling TTY.
     */
    if ((pid = fork()) < 0)
        err_quit("%s: can't fork", cmd);
    else if (pid != 0) /* parent */
        exit(0);
    setsid();

    /*
     * Ensure future opens won't allocate controlling TTYs.
     */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: can't ignore SIGHUP", cmd);
    if ((pid = fork()) < 0)
        err_quit("%s: can't fork", cmd);
    else if (pid != 0) /* parent */
        exit(0);

    /*
     * Change the current working directory to the root so
     * we won't prevent file systems from being unmounted.
     */
    if (chdir("/") < 0)
        err_quit("%s: can't change directory to /", cmd);

    /*
     * Close all open file descriptors.
     */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    /*
     * Attach file descriptors 0, 1, and 2 to /dev/null.
     */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    /*
     * Initialize the log file.
     */
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
          fd0, fd1, fd2);
        exit(1);
    }
}
```

 

#### **出错记录**

因为守护进程本就不应该有控制终端，所以不能简单地把出错信息写到标准错误上。

syslog设施是一个集中的守护进程出错记录设施，下图演示了syslog设施的详细组织结构：

![](.\img_apue\13-1.png)

有以下3种产生日志消息的方法：

1.内核例程可以调用log函数。任何一个用户进程都可以通过打开并读取/dev/klog设备来读取这些信息。

2.大多数用户进程（守护进程）调用syslog函数来产生日志消息。这使消息被发送至UNIX域数据报套接字/dev/log。

3.可将日志消息发向UDP端口514

通常，syslogd守护进程读取所有3种格式的日志消息。此守护进程在启动时读一个配置文件，其文件名一般为/etc/syslog.conf，该文件决定了不同类型的消息应送向何处。

该设施的接口是syslog函数

```
#include <syslog.h>
void openlog(const char *ident,int option,int facility);
void syslog(int priority,const char *format,...);
void closelog(void);
int setlogmask(int maskpri);
```

调用openlog是可选的。如果不调用openlog，则在第一次调用syslog时，自动调用openlog。

调用closelog也是可选的。因为它只是关闭增被用于与syslogd守护进程进行通信的描述符。

调用openlog使我们可以指定一个ident（一般是程序的名称），以后，此ident将被加至每则日志消息中。

option参数是指定各种选项的位屏蔽。下图介绍了可用的option选项：

<img src=".\img_apue\13-2.png" style="zoom:75%;" />

facility参数值选取自下图，用来让配置文件说明来自不同设施的消息将以不同的方式进行处理。

<img src=".\img_apue\13-3.png" style="zoom:75%;" />

调用syslog产生一个日志消息。其priority参数是facility和level的组合。其中level的值按优先级由最高到最低一次排列如下：

<img src=".\img_apue\13-4.png" style="zoom:75%;" />

将format参数以及其他所有参数传至vsprintf函数以便进行格式化。其中，format中每个出现的%m字符都先被替换成与errno值相对应的出错消息字符串。

setlogmask函数用于设置进程的记录优先级屏蔽字。各条消息除非已在记录优先级屏蔽字中进行了设置，否则将不被记录。

除了syslog，很多平台还提供它的一种辩题来处理可变参数列表。

```
#include <syslog.h>
#include <stdarg.h>
void vsyslog(int priority,const char *format,va_list arg);
```

#### **单实例守护进程**

为了正常运作，某些守护进程会实现为在任一时刻只运行该守护进程的一个副本。

文件和记录锁机制（第十四章）为一种方法提供了基础，该方法保证了一个守护进程只有一个副本，下面函数将演示这一点。



```
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

extern int lockfile(int);

int
already_running(void)
{
    int        fd;
    char    buf[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return(1);
        }
        syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    return(0);
}
```

 其中lockfile函数如下

```
#include <unistd.h>
#include <fcntl.h>

int
lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}
```

#### **守护进程实例**

下面程序说明了守护进程可以重读其配置文件的一种方法



```
#include "apue.h"
#include <pthread.h>
#include <syslog.h>

sigset_t    mask;

extern int already_running(void);

void
reread(void)
{
    /* ... */
}

void *
thr_fn(void *arg)
{
    int err, signo;

    for (;;) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            syslog(LOG_ERR, "sigwait failed");
            exit(1);
        }

        switch (signo) {
        case SIGHUP:
            syslog(LOG_INFO, "Re-reading configuration file");
            reread();
            break;

        case SIGTERM:
            syslog(LOG_INFO, "got SIGTERM; exiting");
            exit(0);

        default:
            syslog(LOG_INFO, "unexpected signal %d\n", signo);
        }
    }
    return(0);
}

int
main(int argc, char *argv[])
{
    int                    err;
    pthread_t            tid;
    char                *cmd;
    struct sigaction    sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;

    /*
     * Become a daemon.
     */
    daemonize(cmd);

    /*
     * Make sure only one copy of the daemon is running.
     */
    if (already_running()) {
        syslog(LOG_ERR, "daemon already running");
        exit(1);
    }

    /*
     * Restore SIGHUP default and block all signals.
     */
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        err_quit("%s: can't restore SIGHUP default");
    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
        err_exit(err, "SIG_BLOCK error");

    /*
     * Create a thread to handle SIGHUP and SIGTERM.
     */
    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0)
        err_exit(err, "can't create thread");

    /*
     * Proceed with the rest of the daemon.
     */
    /* ... */
    exit(0);
}
```

下面程序说明一个单线程守护进程然后捕捉SIGHUP并重读其配置文件

```
#include "apue.h"
#include <syslog.h>
#include <errno.h>

extern int lockfile(int);
extern int already_running(void);

void
reread(void)
{
    /* ... */
}

void
sigterm(int signo)
{
    syslog(LOG_INFO, "got SIGTERM; exiting");
    exit(0);
}

void
sighup(int signo)
{
    syslog(LOG_INFO, "Re-reading configuration file");
    reread();
}

int
main(int argc, char *argv[])
{
    char                *cmd;
    struct sigaction    sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;

    /*
     * Become a daemon.
     */
    daemonize(cmd);

    /*
     * Make sure only one copy of the daemon is running.
     */
    if (already_running()) {
        syslog(LOG_ERR, "daemon already running");
        exit(1);
    }

    /*
     * Handle signals of interest.
     */
    sa.sa_handler = sigterm;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        syslog(LOG_ERR, "can't catch SIGTERM: %s", strerror(errno));
        exit(1);
    }
    sa.sa_handler = sighup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        syslog(LOG_ERR, "can't catch SIGHUP: %s", strerror(errno));
        exit(1);
    }

    /*
     * Proceed with the rest of the daemon.
     */
    /* ... */
    exit(0);
}
```

## 第十四章 高级I/O

本章涵盖了从多概念和函数：非阻塞I/O、记录锁、I/O多路转换、异步I/O、readv和writev函数以及存储映射I/O

#### **非阻塞I/O**

非阻塞I/O使我们可以发出open、read和write这样的I/O操作，并使这些操作不会永远阻塞。如果这种操作不能完成，则调用立即返回出错。

对于一个给定的文件描述符，有两种为其制定非阻塞I/O的方法：

1.如果调用open获得描述符，则可制定O_NONBLOCK标志（第三章）

2.对于已经打开的一个描述符，则可调用fcntl，由该函数打开O_NONBLOCK文件状态标志

下面程序时一个非阻塞I/O的实例

```
#include "apue.h"
#include <errno.h>
#include <fcntl.h>

char    buf[500000];

int
main(void)
{
    int        ntowrite, nwrite;
    char    *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", ntowrite);

    set_fl(STDOUT_FILENO, O_NONBLOCK);    /* set nonblocking */

    ptr = buf;
    while (ntowrite > 0) {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        if (nwrite > 0) {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }

    clr_fl(STDOUT_FILENO, O_NONBLOCK);    /* clear nonblocking */

    exit(0);
}
```

#### **记录锁**

记录锁的功能是：当第一个进程正在读或修改文件的某个部分时，使用记录锁可以阻止其它进程修改同一文件区（锁住文件的一个区域）。

#### **fcntl记录锁**

```
#include <fcntl.h>
int fcntl(int fd,int cmd,.../* struct flock *flockptr */);
```

对于记录锁，cmd是F_GETLK、F_SETLK或F_SETLKW。第三个参数是一个指向flock结构的指针

```
 struct flock {
         ...
         short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
         short l_whence;  /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
         off_t l_start;   /* Starting offset for lock */
         off_t l_len;     /* Number of bytes to lock */
         pid_t l_pid;     /* PID of process blocking our lock(F_GETLK only) */
         ...
};
```

对flock结构的说明如下：

1.所希望的锁类型：F_RDLCK（共享读锁）、F_WRLCK（独占性写锁）或F_UNLCK（解锁）

2.要加锁或解锁区域的起始字节偏移量（l_start和l_whence）

3.区域的字节长度（l_len）

4.进程的ID（l_pid）持有的锁能阻塞当前进程（仅由F_GETLK返回）

5.如若l_len为0，则表示锁的返回可以扩展到最大可能偏移量

上面提到了两种类型的锁：F_RDLCK和F_WRLCK。下面演示在多个进程中的其兼容性规则

<img src=".\img_apue\14-1.png" style="zoom: 50%;" />

在单个进程中情况就不一样：如果一个进程对一个文件区间已经有了一把锁，后来该进程又企图在同一文件区间再加一把锁，那么心锁将替换已有锁。

下面说明fcntl函数的3种命令

F_GETLK  判断由flockptr所描述的锁是否会被另外一把锁排斥。如果存在这样的锁，则讲该锁的信息重写到flockptr指向的信息中，如果不存在，则将l_type设置为F_UNLCK

F_SETLK  设置由flockptr锁描述的锁。如果该操作不符合兼容性规则，则函数立即出错返回，并将errno设置为EACCES或EAGAIN

F_SETLKW 这个命令是F_SETLK的阻塞版本（W代表wait）。如果该操作不符合兼容性规则，则调用进程被置为休眠，知道请求创建的锁已经可用，才将该进程唤醒

####  **对fcntl记录锁函数的封装** 

```
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len )
{
    struct flock lock;
    lock.l_type = type; /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset; /* byte offset, relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len; /* #bytes (0 means to EOF) */
    return( fcntl(fd, cmd, lock) );
}

#define read_lock(fd,offset,whence,len)
lock_reg(fd,F_SETLK,F_RDLCK,offset,whence,len)
#define needw_lock(fd,offset,whence,len)
lock_reg(fd,F_SETLKW,F_RDLCK,offset,whence,len)
#define write_lock(fd,offset,whence,len)
lock_reg(fd,F_SETLK,F_WRLCK,offset,whence,len)
#define writew_lock(fd,offset,whence,len) lock_reg(fd,F_SETLKW,F_WRLCK,offset,whence,len)
#define un_lock(fd,offset,whence,len) lock_reg(fd,F_SETLK,F_UNLCK,offset,whence,len)
```

#### **锁的隐含继承和释放有3条规则**

 

1.记录锁和文件两者相关联：（a）当一个进程终止时，它所建立的锁全部释放；（b）无论一个描述符何时关闭，该进程通过这一描述符引用的文件的任何一把锁都会释放

  如果执行下列4步：

  fd1=open(pathname,...);

  read_lock(fd1,...);

  fd2=dup(fd1);  //fd2=open(pathname,...);

  close(fd2);

  则在close(fd2)后，在fd1上设置的锁被释放，如果将dup替换为open，效果也一样。

2.由fork产生的子进程不继承父进程所设置的锁

3.在执行exec后，新程序可以继承原执行程序的锁。

 

#### **在文件尾端加锁**

考虑下列代码序列：

writew_lcok(fd,0,SEEK_END,0);

write(fd,0,SEEK_END,0);

un_lock(fd,0,SEEK_END);

write(fd,buf,1);

这段代码序列造成的文件锁状态如下图所示

<img src=".\img_apue\14-2.png" style="zoom:50%;" />

可见，在对相对于文件尾端的字节范围加锁或解锁时需要特别小心。

 

#### **建议性锁和强制性锁**

强制性锁会让内核检查每一个open、read和write，验证调用进程是否违背了正在访问的文件上的某一把锁。

如果一个进程试图读或写一个强制性锁起作用的文件，有以下8种情况

<img src=".\img_apue\14-4.png" style="zoom:50%;" />

如果欲打开的文件具有强制性记录锁，而且open调用中的标志指定为O_TRUNC或O_CREAT,则open立即出错返回，errno设置为EAGAIN。

下面程序可以用来测试系统是否支持强制性锁

```
#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
    int                fd;
    pid_t            pid;
    char            buf[5];
    struct stat        statbuf;

    if (argc != 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        exit(1);
    }
    if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
        err_sys("open error");
    if (write(fd, "abcdef", 6) != 6)
        err_sys("write error");

    /* turn on set-group-ID and turn off group-execute */
    if (fstat(fd, &statbuf) < 0)
        err_sys("fstat error");
    if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        err_sys("fchmod error");

    TELL_WAIT();

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {    /* parent */
        /* write lock entire file */
        if (write_lock(fd, 0, SEEK_SET, 0) < 0)
            err_sys("write_lock error");

        TELL_CHILD(pid);

        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
    } else {                /* child */
        WAIT_PARENT();        /* wait for parent to set lock */

        set_fl(fd, O_NONBLOCK);

        /* first let's see what error we get if region is locked */
        if (read_lock(fd, 0, SEEK_SET, 0) != -1)    /* no wait */
            err_sys("child: read_lock succeeded");
        printf("read_lock of already-locked region returns %d\n",
          errno);

        /* now try to read the mandatory locked file */
        if (lseek(fd, 0, SEEK_SET) == -1)
            err_sys("lseek error");
        if (read(fd, buf, 2) < 0)
            err_ret("read failed (mandatory locking works)");
        else
            printf("read OK (no mandatory locking), buf = %2.2s\n",
              buf);
    }
    exit(0);
}
```

#### **I/O多路转换**

###### **函数select和pselect**

```
#include <sys/select.h>
int select(int maxfdp1,fd_set *restrict readfds,fd_set *restrict writefds,fd_set *restrict exceptfds,struct timeval *restrict tvptr);
```

参数tvptr指定愿意等待的时间，有下面3种情况

1.tvptr==NULL 永远等待，直到所指定的描述符中的一个已经准备好或捕捉到一个信号返回。如果捕捉到一个信号，则select返回-1，errno设置为EINTR

2.tvptr->tv_sec==0 && tvptr->tv_usec==0 不等待，测试所有指定的描述符并立即返回

3.tvptr->tv_sec!=0 || tvptr->tv_usec!=0  等待指定的秒数和微秒数。当指定的描述符之一已准备好，或当指定的时间值已经超过时立即返回

中间3个参数readfds、writefds和exceptfds是指向描述符集的指针。这3个描述符集说明了我们关心的可读、可写和处于异常条件的描述符集合。

对于描述符集fd_set结构，提供了下面4个函数

```
#include <sys/select.h>
int FD_ISSET(int fd,fd_set *fdset);
void FD_CLR(int fd,fd_set *fdset);
void FD_SET(int fd,fd_set *fdset);
void FD_ZERO(fd_set *fdset);
```

select第一个参数maxfdp1的意思是“最大文件描述符编号值加1”，在上面3个描述符集中找到最大描述符编号值，然后加1就是第一个参数值。

select有3个可能的返回值

1.返回值-1表示出错。如果在所指定的描述符一个都没准备好时捕捉到一个信号，则返回-1

2.返回0表示没有描述符准备好，指定的时间就过了。

3.一个正返回值说明了已经准备好的描述符数，在这种情况下，3个描述符集中依旧打开的位对应于已准备好的描述符。

POSIX.1也定义了一个select的变体，称为pselect

```
#include <sys/select.h>
int pselect(int maxfdp1,fd_set *restrict readfds,fd_set *restrict writefds,fd_set *restrict exceptfds,const struct timespec *restrict tsptr,const sigset_t *restrict sigmask);
```

除了下列几点外，pselect与select相同

1.超时值使用的结构不一样，pselect使用的是timespec结构

2.pselect的超时值被声明为const，保证了调用pselect不会改变此值

3.pselect可使用可选信号屏蔽字，在调用pselect时，以原子操作的方式安装该信号屏蔽字。在返回时，恢复以前的信号屏蔽字。

###### **函数poll**

```
#include <poll.h>
int poll(struct pollfd fdarray[],nfds_t nfds,int timeout);
```

与select不同，poll构造一个pollfd结构的数组，每个数组元素指定一个描述符编号以及我们对描述符感兴趣的条件。

```
struct pollfd {
        int   fd;         /* file descriptor */
        short events;     /* requested events */
        short revents;    /* returned events */
};
```

fdarray数组中的元素数由nfds指定

结构中的events成员应设置为下图所示值的一个或几个

<img src=".\img_apue\14-5.png" style="zoom: 67%;" />

后三个只能作为描述字的返回结果存储在revents中，而不能作为测试条件用于events中。

poll最后一个参数指定的是我们愿意等待多久时间（毫秒值）

#### **异步I/O**

使用一个信号（System V是SIGPOLL，在BSD中是SIGIO）通知进程，对某个描述符锁关心的某个时间已经发生。

#### **函数readv和writev**

readv和writev函数用于在一次函数调用读、写多个非连续缓冲区。有时也将这两个函数成为散布读和聚集写。

```
#include <sys/uio.h>
ssize_t readv(int fd,const struct iovec *iov,int iovcnt);
ssize_t writev(int fd,const struct iovec *iov,int iovcnt);
                           // 两个函数的返回值：已读或一些的字节数；出错返回-1
```

这两个函数的第二个参数是指向iovec结构数组的一个指针：

```
 struct iovec {
        void  *iov_base;    /* Starting address */
        size_t iov_len;     /* Number of bytes to transfer */
 };
```

 下图显示了iovec结构 

<img src=".\img_apue\14-6.png" style="zoom:50%;" />

iov数组中元素数由iovcnt指定

writev函数从缓冲区中聚集输出数据的顺序是：iov[0]、iov[1]知道iov[iovcnt-1]

readv函数则将读入的数据按上述同样顺序散布到缓冲区中

 

 

#### **函数readn和writen**

通常，在读、写一个管道、网络设备或终端时，需要考虑一下特性：

1.一次read操作所返回的数据可能少于所要求的数据，即使还没达到文件尾端也可能是这样。这不是一个错误，应当继续读该设备。

2.一次write操作的返回值也可能少于指定输出的字符数。

下面两个函数的功能分别是读、写指定的N字节数据，并处理返回值可能少于要求值的情况。

```
#include "apue.h"
ssize_t readn(int fd,void *buf,size_t nbytes);
ssize_t writen(int fd,void *buf,size_t nbytes);
```

下面是这两个函数的实现

readn函数

```
#include "apue.h"

ssize_t             /* Read "n" bytes from a descriptor  */
readn(int fd, void *ptr, size_t n)
{
    size_t        nleft;
    ssize_t        nread;

    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break;      /* error, return amount read so far */
        } else if (nread == 0) {
            break;          /* EOF */
        }
        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);      /* return >= 0 */
}
```

 writen函数 

```
#include "apue.h"

ssize_t             /* Write "n" bytes to a descriptor  */
writen(int fd, const void *ptr, size_t n)
{
    size_t        nleft;
    ssize_t        nwritten;

    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break;      /* error, return amount written so far */
        } else if (nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n - nleft);      /* return >= 0 */
}
```

#### **存储映射I/O**

存储映射I/O能将一个磁盘文件映射到存储空间中的一个缓冲区上。于是，当从缓冲区取数据时，就相当于读文件中的相应字节。

与此类似，将数据存入缓冲区，相应字节就自动写入文件。

## 第十五章 进程间通信

本章将说明进程之间相互通信的其它技术----进程间通信（IPC）

#### **管道**

管道只能在具有公共祖先的两个进程之间只用。通常，一个管道由一个进程创建，在进程调用fork后，这个管道就能在父进程和子进程之间使用了。

管道是通过调用pipe函数创建的：

```
#include <unistd.h>
int pipe(int fd[2]);
```

经由参数fd返回两个文件描述符：fd[0]为读而打开，fd[1]为写而打开。fd[1]是输出，fd[0]是输入。

下图演示从父进程到子进程的管道（父进程关闭管道的读端(fd[0])，子进程关闭管道的写端(fd[1])）

<img src=".\img_apue\15-1.png" style="zoom:50%;" />

 下面程序创建了一个父进程到子进程的管道，并且父进程经由该管道向子进程传送数据 

```
#include "apue.h"

int
main(void)
{
    int        n;
    int        fd[2];
    pid_t    pid;
    char    line[MAXLINE];

    if (pipe(fd) < 0)
        err_sys("pipe error");
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {        /* parent */
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    } else {                    /* child */
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
    }
    exit(0);
}
```

#### **函数popen和pclose**

这两个函数实现的操作是：创建一个管道，fork一个子进程，关闭未使用的管道端，执行一个shell运行命令，然后等待命令终止。

```
#include <stdio.h>
FILE *popen(const char *cmdstring,const char *type);
int pclose(FILE *fp);
```

函数popen先执行fork，然后调用exec执行cmdstring，并且返回一个标准I/O文件指针。

如果type是“r”，则文件指针连接到cmdstring的标准输出

<img src=".\img_apue\15-2.png" style="zoom:50%;" />

 如果type是“w”，则文件指针连接到cmdstring的标准输入 

<img src=".\img_apue\15-3.png" style="zoom:50%;" />

pclose函数关闭标准I/O流，等待命令终止，然后返回shell的终止状态。

下面程序演示使用popen向分页程序传送文件

```
#include "apue.h"
#include <sys/wait.h>

#define    PAGER    "${PAGER:-more}" /* environment variable, or default */

int
main(int argc, char *argv[])
{
    char    line[MAXLINE];
    FILE    *fpin, *fpout;

    if (argc != 2)
        err_quit("usage: a.out <pathname>");
    if ((fpin = fopen(argv[1], "r")) == NULL)
        err_sys("can't open %s", argv[1]);

    if ((fpout = popen(PAGER, "w")) == NULL)
        err_sys("popen error");

    /* copy argv[1] to pager */
    while (fgets(line, MAXLINE, fpin) != NULL) {
        if (fputs(line, fpout) == EOF)
            err_sys("fputs error to pipe");
    }
    if (ferror(fpin))
        err_sys("fgets error");
    if (pclose(fpout) == -1)
        err_sys("pclose error");

    exit(0);
}
```

 下面是程序运行的结果 

<img src=".\img_apue\15-4.png" style="zoom:50%;" />

 考虑下面一个应用程序：它向标准输出写一个提示，然后从标准输入读一行，使用popen在应用程序和输入之间插入一个程序以便对输入进行变换 

<img src=".\img_apue\15-5.png" style="zoom:50%;" />

 下面程序将演示这个过滤程序，它将输入的大写字符转换成小写字符 

```
#include "apue.h"
#include <ctype.h>

int
main(void)
{
    int        c;

    while ((c = getchar()) != EOF) {
        if (isupper(c))
            c = tolower(c);
        if (putchar(c) == EOF)
            err_sys("output error");
        if (c == '\n')
            fflush(stdout);
    }
    exit(0);
}
```

 将这个过滤程序编译成可执行文件，然后下面程序用popen调用它 

```
#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
    char    line[MAXLINE];
    FILE    *fpin;

    if ((fpin = popen("myuclc", "r")) == NULL)
        err_sys("popen error");
    for ( ; ; ) {
        fputs("prompt> ", stdout);
        fflush(stdout);
        if (fgets(line, MAXLINE, fpin) == NULL)    /* read from pipe */
            break;
        if (fputs(line, stdout) == EOF)
            err_sys("fputs error to pipe");
    }
    if (pclose(fpin) == -1)
        err_sys("pclose error");
    putchar('\n');
    exit(0);
}
```

#### **协同进程**

UNIX系统过滤程序从标准输入读取数据，向标准输出写数据。

当一个过滤程序既产生某个过滤程序的输入，又读取该过滤程序的输出时，它就变成了协同进程。

我们通过实例来观察协同进程。进程创建两个管道：一个是协同进程的标准输入，一个是协同进程的标准输出。

<img src=".\img_apue\15-6.png" style="zoom:50%;" />

 下面程序是一个简单的协同进程，它从其标准输入读取两个数，计算它们的和，然后将和写至其标准输出。 

```
#include "apue.h"

int
main(void)
{
    int        n, int1, int2;
    char    line[MAXLINE];

    while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
        line[n] = 0;        /* null terminate */
        if (sscanf(line, "%d%d", &int1, &int2) == 2) {
            sprintf(line, "%d\n", int1 + int2);
            n = strlen(line);
            if (write(STDOUT_FILENO, line, n) != n)
                err_sys("write error");
        } else {
            if (write(STDOUT_FILENO, "invalid args\n", 13) != 13)
                err_sys("write error");
        }
    }
    exit(0);
}
```

对此程序进行编译，将其可执行文件目标代码存入名为add2的文件。

使用下面程序来调用add2协同进程，并将协同进程送来的值写到其标准输出。

```
#include "apue.h"

static void    sig_pipe(int);        /* our signal handler */

int
main(void)
{
    int        n, fd1[2], fd2[2];
    pid_t    pid;
    char    line[MAXLINE];

    if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
        err_sys("signal error");

    if (pipe(fd1) < 0 || pipe(fd2) < 0)
        err_sys("pipe error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {                            /* parent */
        close(fd1[0]);
        close(fd2[1]);

        while (fgets(line, MAXLINE, stdin) != NULL) {
            n = strlen(line);
            if (write(fd1[1], line, n) != n)
                err_sys("write error to pipe");
            if ((n = read(fd2[0], line, MAXLINE)) < 0)
                err_sys("read error from pipe");
            if (n == 0) {
                err_msg("child closed pipe");
                break;
            }
            line[n] = 0;    /* null terminate */
            if (fputs(line, stdout) == EOF)
                err_sys("fputs error");
        }

        if (ferror(stdin))
            err_sys("fgets error on stdin");
        exit(0);
    } else {                                    /* child */
        close(fd1[1]);
        close(fd2[0]);
        if (fd1[0] != STDIN_FILENO) {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
                err_sys("dup2 error to stdin");
            close(fd1[0]);
        }

        if (fd2[1] != STDOUT_FILENO) {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
                err_sys("dup2 error to stdout");
            close(fd2[1]);
        }
        if (execl("./add2", "add2", (char *)0) < 0)
            err_sys("execl error");
    }
    exit(0);
}

static void
sig_pipe(int signo)
{
    printf("SIGPIPE caught\n");
    exit(1);
}
```

#### **FIFO**

FIFO有时被称为命名管道。与未命名管道不一样：通过FIFO，不相关的进程也能交换数据。

FIFO是一种文件类型，创建FIFO类似于创建文件

```
#include <sys/stat.h>
int mkfifo(const char *path,mode_t mode);
int mkfifoat(int fd,const char *path,mode_t mode);
```

考虑这样一个过程，它需要对一个经过过滤的输出流进行两次处理

<img src=".\img_apue\15-7.png" style="zoom:50%;" />

使用FIFO和UNIX程序tee(1)就可以实现这样的过程而无需使用临时文件。

tee程序将其标准输入同时复制到其标准输出以及其命令行中命名的文件中。

```
mkfifo fifo1
prog3 < fifo1 &
prog1 < infile | tee fifo1 | prog2
```

创建FIFO，然后在后台启动prog3，从FIFO读数据。然后启动prog1，用tee将其输出发送到FIFO和prog2

<img src=".\img_apue\15-8.png" style="zoom:50%;" />

FIFO的另一个用途是在客户进程和服务器进程之间传送数据。

如果有一个服务器进程，它与很多客户进程有关，每个客户进程都可将其请求写到一个该服务器进程创建的总所周知的FIFO中。

<img src=".\img_apue\15-9.png" alt="15-8" style="zoom:50%;" />

 服务器可以使用下面的安排来响应客户进程（为每一个客户进程创建一个FIFO用来响应） 

<img src=".\img_apue\15-10.png" style="zoom:50%;" />

#### **XSI IPC**

有3中成果XSI IPC的IPC：消息队列、信号量以及共享存储器。

它们有如下相类似的特征：

1.标识符和键

2.权限结构

3.结构限制

#### **消息队列**

消息队列是消息的链接表，存储在内核中，由消息队列标识符标识。

#### **信号量**

信号量是一个计数器，用于为多个进程提供对共享数据对象的访问。

#### **共享存储**

共享存储允许两个或多个进程共享一个给定的存储区。

## 第十六章 网络IPC:套接字

 本章将考察不同计算机（通过网络连接）上的进程相互通信的机制：网络进程间通信。 

#### **套接字描述符**

正如使用文件描述符访问文件，应用程序用套接字描述符访问套接字。

许多处理文件描述符函数（如read和write）可以用于处理套接字描述符。调用socket函数创建一个套接字

```
#include <sys/socket.h>
int socket(int domain,int type,int protocol);
```

参数domain（域）确定通信的特性，包括地址格式。下图总结了POSIX.1指定的各个域，每个域都有自己表示地址的格式

<img src=".\img_apue\16-1.png" style="zoom:50%;" />

 参数type确定套接字的类型，下图总结了POSIX.1定义的套接字类型。 

<img src=".\img_apue\16-2.png" style="zoom:50%;" />

参数protocol通常是0，表示为给定的域和套接字类型选择默认协议。

调用socket与调用open相类似。可以调用close来关闭对文件或套接字的访问，并且释放该描述符以便重新使用。

下图总结了到目前为止所讨论的大多数以文件描述符为参数的函数使用套接字描述符时的行为。

<img src=".\img_apue\16-3.png" style="zoom:50%;" />

套接字通信是双向的。可以采用shutdown函数来禁止一个套接字的I/O

```
#include <sys/socket.h>
int shutdown(int sockfd,int how);
```

如果how是SHUT_RD（关闭读端），那么无法从套接字读取数据。

如果how是SHUT_WR（关闭写端），那么无法使用套接字发送数据。

如果how是SHUT_RDWR，则既无法读取数据，又无法发送数据。

使用close函数直到关闭了最后一个引用它的文件描述符才会释放这个套接字，而shutdown允许使用一个套接字处于不活动状态，和引用它的文件描述符数目无关。

#### **字节序**

有4个用来处理处理器字节序和网络字节序之间实施转换的函数

```
#include <arpa/inet.h>
uint32_t htonl(unit32_t hostint32);  //返回值：以网络字节序表示的32位整数
uint16_t htons(unit16_t hostint16); //返回值：以网络字节序表示的16位整数
uint32_t ntohl(unit32_t netint32);  //返回值：以主机字节序表示的32位整数
uint32_t ntons(unit16_t netint16);  //返回值：以主机字节序表示的16位整数
```

#### **地址格式**

一个地址标识一个特定通信域的套接字端点，地址格式与这个特定的通信域相关。

为使不同的格式地址能够传入到套接字函数，地址会被强制转换成一个通用的地址结构sockaddr(Linux中的定义)：

```
struct sockaddr{
       sa_family_t sa_family;
       char sa_data[14];
};
```

因特网地址定义在<netinet/in.h>头文件中。在IPv4因特网域中，套接字地址用结构sockaddr_in表示：

```
struct in_addr{
       in_addr_t s_addr;
};

struct sockaddr_in{
       sa_family_t sin_family;
       in_port_t sin_port;
       struct in_addr sin_addr;
};
```

  IPv6因特网域套接字地址用结构sockaddr_in6表示： 

```
struct in6_addr{
       uint8_t s6_addr[16];
};

struct sockaddr_in6{
       sa_family_t sin6_family;
       in_port_t sin6_port;
       uint32_t sin6_flowinfo;
       struct in6_addr sin6_addr;
       uint32_t sin6_scope_id;
};
```

下面两个函数用于二进制地址格式与十进制字符表示（a.b.c.d）之间的相互转换：

```
#include <arpa/inet.h>
const char * inet_ntop (int domain, const void * restrict addr, char * restrict str, socklen_t size);
int inet_pton ( int domain, const char * restrict str, void * restrict addr);
```

#### **地址查询**

通过调用gethostend，可以找到给定计算机系统的主机信息

```
#include <netdb.h>
struct hostent *gethostent(void);
void sethostent(int stayopen);
void endhostent(void);
```

如果主机数据库文件没有打开，gethostent会打开它。函数gethostent返回文件中的下一个条目。

函数sethostent会打开文件，如果文件已经被打开，那么将其回绕。

hostent结构如下至少包含以下成员：

```
 struct hostent {
        char  *h_name;            /* official name of host */
        char **h_aliases;         /* alias list */
        int    h_addrtype;        /* host address type */
        int    h_length;          /* length of address */
        char **h_addr_list;       /* list of addresses */
};
```

我们可以用以下函数在协议名字和协议编号之间进行映射：

```
#include <netdb.h>
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
void setprotoent(int stayopen);
void endprotoent(void);
```

POSIX.1定义的protoent结构至少包含以下成员：

```
struct protoent {
        char  *p_name;       /* official protocol name */
        char **p_aliases;    /* alias list */
        int    p_proto;      /* protocol number */
};
```

可以使用函数getservbyname将一个服务名映射到一个端口号，使用函数getservbyport将一个端口号映射到一个服务名，使用函数getservent顺序扫描服务数据库

```
#include <netdb.h>
struct servent *getservbyname(const char *name,const char *proto);
struct servent *getservbyport(int port,const char *proto);

struct servent *getservent(void);
void setservent(int stayopen);
void endservent(void);
```

servent结构至少包含以下成员：

```
struct servent {
        char  *s_name;       /* official service name */
        char **s_aliases;    /* alias list */
        int    s_port;       /* port number */
        char  *s_proto;      /* protocol to use */
};
```

POSIX.1定义了若干新的函数，允许一个应用程序将一个主机名和一个服务器名映射到一个地址

```
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *restrict host,
                const char *restrict service,
                const struct addrinfo *restrict hint,
                struct addrinfo **restrict res);
void freeaddrinfo(struct addrinfo *ai);
```

 需要提供主机名、服务名，或者两者都提供。函数返回一个链表结构addrinfo，可以用freeaddrinfo来释放一个或多个这种结构。

addrinfo结构的定义至少包含以下成员：

```
 struct addrinfo {
        int              ai_flags;
        int              ai_family;
        int              ai_socktype;
        int              ai_protocol;
        socklen_t        ai_addrlen;
        struct sockaddr *ai_addr;
        char            *ai_canonname;
        struct addrinfo *ai_next;
};
```

可以提供一个可选的hint来选择符合特定条件的地址。

如果getaddrinfo失败，需要调用gai_strerror将返回的错误码转换成错误消息

```
#include <netdb.h>
const char *gai_strerror(int error);
```

getnameinfo函数将一个地址转换成一个主机名和一个服务名

```
#include <sys/socket.h>
#include <netdb.h>
int getnameinfo(const struct sockaddr *restrict addr,socklen_t alen,
                char *restrict host,socklen_t hostlen,
                char *restrict service,socklen_t servlen,int flags);
```

套接字地址被翻译为一个主机名和一个服务名。flags参数提供了一些控制翻译的方式，下图总结了支持的标志

<img src=".\img_apue\16-4.png" style="zoom:50%;" />

 下面程序说明了getaddrinfo函数的使用方法 

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

void print_family(struct addrinfo *aip)
{
    printf("family ");
    switch(aip->ai_family)
    {
    case AF_INET:
        printf("inet ");
        break;
    case AF_INET6:
        printf("inet6 ");
        break;
    case AF_UNIX:
        printf("unix ");
        break;
    case AF_UNSPEC:
        printf("unspecified ");
        break;
    default:
        printf("unkown ");
    }
}

void print_type(struct addrinfo *aip)
{
    printf("type ");
    switch(aip->ai_socktype)
    {
    case SOCK_STREAM:
        printf("stream ");
        break;
    case SOCK_DGRAM:
        printf("datagram");
        break;
    case SOCK_SEQPACKET:
        printf("seqpacket ");
        break;
    case SOCK_RAW:
        printf("raw ");
        break;
    default:
        printf("unknown (%d)",aip->ai_socktype);
    }
}

void print_protocol(struct addrinfo *aip)
{
    printf(" protocol ");
    switch(aip->ai_protocol)
    {
    case 0:
        printf("default ");
        break;
    case IPPROTO_TCP:
        printf("TCP ");
        break;
    case IPPROTO_UDP:
        printf("UDP ");
        break;
    case IPPROTO_RAW:
        printf("raw ");
        break;
    default:
        printf("unknown (%d)",aip->ai_protocol);
    }
}

void print_flags(struct addrinfo *aip)
{
    printf("flags");
    if(aip->ai_flags == 0)
        printf("0");
    else
    {
        if(aip->ai_flags & AI_PASSIVE)
            printf(" passive ");
        if(aip->ai_flags & AI_CANONNAME)
            printf(" canon ");
        if(aip->ai_flags & AI_NUMERICHOST)
            printf(" numhost ");
    }
}

int main()
{
    struct addrinfo *ailist,*aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if(getaddrinfo("localhost",NULL,&hint,&ailist) != 0)
    {
        printf("getaddrinfo error: %s",gai_strerror(err));
        exit(-1);
    }
    for(aip = ailist;aip != NULL;aip = aip->ai_next)
    {
        print_flags(aip);
        print_family(aip);
        print_type(aip);
        print_protocol(aip);
        printf("\n\thost %s",aip->ai_canonname ?aip->ai_canonname : "-");
        if(aip->ai_family == AF_INET)
        {
            sinp = (struct sockaddr_in *)aip->ai_addr;
            addr = inet_ntop(AF_INET,&sinp->sin_addr,abuf,INET_ADDRSTRLEN);
            printf(" address %s ",addr?addr:"unknown");
            printf(" port %d ",ntohs(sinp->sin_port));
        }
        printf("\n");
    }
    exit(0);
}
```

 下面是程序运行结果 

![]()

<img src=".\img_apue\16-5.png" alt="16-5" style="zoom:50%;" />

#### **将套接字与地址关联**

使用bind函数来关联地址和套接字

```
#include <sys/socket.h>
int bind(int sockfd,const struct sockaddr *addr,socklen_t len);
```

可以调用getsockname函数来发现绑定到套接字上的地址

```
#include <sys/socket.h>
int getsockname(int sockfd,struct sockaddr *restrict addr,socklen_t *restrict alenp);
```

如果套接字已经和对等方连接，可以调用getpeername函数找到对方的地址

```
#include <sys/socket.h>
int getpeername(int sockfd,struct sockaddr *restrict addr,socklen_t *restrict alenp);
```

 

#### **建立连接**

如果处理一个面向连接的网络服务，那么开始交换数据之前，需要使用connect函数来建立连接

```
#include <sys/socket.h>
int connect(int sockfd,const struct sockaddr *addr,socklen_t len);
```

connect中指定的地址是我们想与之通信的服务器地址。

服务器调用listen函数来宣告它愿意接受连接请求

```
#include <sys/socket.h>
int listen(int sockfd,int backlog);
```

参数backlog提供了一个提示，提示系统改进程所要入队的未完成连接请求数量。一旦队列满了，系统就会拒绝多余的连接请求。

一旦服务器调用了listen，所用的套接字就恩能够接受连接请求。使用accept函数获得连接请求并建立连接。

```
#include <sys/socket.h>
int accept(int fd,struct sockaddr *restrict addr,socklen_t *restrict len); 
```

函数返回的是一个连接到调用connect的客户端的套接字描述符

#### **数据传输**

在建立了连接之后，就可以使用read和write来通过套接字通信。

除此之外，有6个位数据传递而设计的套接字函数（3个用于发送数据，3个用于接收数据）。

最简单的是send，它和write很像，但是可以指定标志来改变处理传输数据的方式

```
#include <sys/socket.h>
ssize_t send(int sockfd,const void *buf,size_t nbytes,int flags);
```

下面总结了flags可能的取值

<img src=".\img_apue\16-6.png" style="zoom:50%;" />

函数sendto和send很类似，区别在于sendto可以在无连接的套接字上指定一个目标地址

```
#include <sys/socket.h>
ssize_t sendto(int sockfd,const void *buf,size_t nbytes,int flags,
               const struct sockaddr *destaddr,socklen_t destlen);
```

对于面向连接的套接字，目标地址是被忽略的。

通过套接字发送数据时，还有一个选择。可以调用带有msghdr结构的sendmsg来指定多重缓冲区传输数据。

```
#include <sys/socket.h>
ssize_t sendmsg(int sockfd,const struct msghdr *msg,int flags);
```

POSIX.1定义了msghdr结构，它至少有以下成员：

```
struct msghdr {
        void         *msg_name;       /* optional address */
        socklen_t     msg_namelen;    /* size of address */
        struct iovec *msg_iov;        /* scatter/gather array */
        size_t        msg_iovlen;     /* # elements in msg_iov */
        void         *msg_control;    /* ancillary data, see below */
        size_t        msg_controllen; /* ancillary data buffer len */
        int           msg_flags;      /* flags on received message */
};
```

函数recv和read相似，但是recv可以指定标志来控制如何接收数据

```
#include <sys/socket.h>
ssize_t recv(int sockfd,void *buf,size_t nbytes,int flags);
```

下图总结了这些标志

<img src=".\img_apue\16-8.png" style="zoom:50%;" />

如果有兴趣定位发送者，可以使用recvfrom来得到数据发送者的源地址

```
#include <sys/socket.h>
ssize_t recvfrom(int sockfd,void *restrict buf,size_t len,int flags
                 struct sockaddr *restrict addr,
                 socklen_t *restrict addrlen);
```

如果addr非空，它将包含数据发送者的套接字端点地址。通常用于无连接的套接字。

为了将接收到的数据送入多个缓冲区，或者想接收辅助数据，可以使用recvmsg

```
#include <sys/socket.h>
ssize_t recvmsg(int sockfd,struct msghdr *msg,int flags);
```

返回时，msghdr结构中的msg_flags字段都被设为所接收数据的特征。

 

#### **面向连接实例**

下面程序显示了一个与服务器通信的客户端从系统的uptime命令获得输出。

```
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN        128

extern int connect_retry(int, int, int, const struct sockaddr *,
    socklen_t);

void
print_uptime(int sockfd)
{
    int        n;
    char    buf[BUFLEN];

    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        write(STDOUT_FILENO, buf, n);
    if (n < 0)
        err_sys("recv error");
}

int
main(int argc, char *argv[])
{
    struct addrinfo    *ailist, *aip;
    struct addrinfo    hint;
    int                sockfd, err;

    if (argc != 2)
        err_quit("usage: ruptime hostname");
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(err));
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0,
          aip->ai_addr, aip->ai_addrlen)) < 0) {
            err = errno;
        } else {
            print_uptime(sockfd);
            exit(0);
        }
    }
    err_exit(err, "can't connect to %s", argv[1]);
}
```

这个程序连接服务器，读取服务器发送过来的字符串并将其打印到标准输出。

下面展示了服务器程序，用来提供uptime命令的输出到上面的客户端程序

```
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>

#define BUFLEN    128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
    int        clfd;
    FILE    *fp;
    char    buf[BUFLEN];

    set_cloexec(sockfd);
    for (;;) {
        if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
            syslog(LOG_ERR, "ruptimed: accept error: %s",
              strerror(errno));
            exit(1);
        }
        set_cloexec(clfd);
        if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
            sprintf(buf, "error: %s\n", strerror(errno));
            send(clfd, buf, strlen(buf), 0);
        } else {
            while (fgets(buf, BUFLEN, fp) != NULL)
                send(clfd, buf, strlen(buf), 0);
            pclose(fp);
        }
        close(clfd);
    }
}

int
main(int argc, char *argv[])
{
    struct addrinfo    *ailist, *aip;
    struct addrinfo    hint;
    int                sockfd, err, n;
    char            *host;

    if (argc != 1)
        err_quit("usage: ruptimed");
    if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX;    /* best guess */
    if ((host = malloc(n)) == NULL)
        err_sys("malloc error");
    if (gethostname(host, n) < 0)
        err_sys("gethostname error");
    daemonize("ruptimed");
    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
          gai_strerror(err));
        exit(1);
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
          aip->ai_addrlen, QLEN)) >= 0) {
            serve(sockfd);
            exit(0);
        }
    }
    exit(1);
}
```

 另一个版本的服务器程序 

```
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
    int        clfd, status;
    pid_t    pid;

    set_cloexec(sockfd);
    for (;;) {
        if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
            syslog(LOG_ERR, "ruptimed: accept error: %s",
              strerror(errno));
            exit(1);
        }
        if ((pid = fork()) < 0) {
            syslog(LOG_ERR, "ruptimed: fork error: %s",
              strerror(errno));
            exit(1);
        } else if (pid == 0) {    /* child */
            /*
             * The parent called daemonize ({Prog daemoninit}), so
             * STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO
             * are already open to /dev/null.  Thus, the call to
             * close doesn't need to be protected by checks that
             * clfd isn't already equal to one of these values.
             */
            if (dup2(clfd, STDOUT_FILENO) != STDOUT_FILENO ||
              dup2(clfd, STDERR_FILENO) != STDERR_FILENO) {
                syslog(LOG_ERR, "ruptimed: unexpected error");
                exit(1);
            }
            close(clfd);
            execl("/usr/bin/uptime", "uptime", (char *)0);
            syslog(LOG_ERR, "ruptimed: unexpected return from exec: %s",
              strerror(errno));
        } else {        /* parent */
            close(clfd);
            waitpid(pid, &status, 0);
        }
    }
}

int
main(int argc, char *argv[])
{
    struct addrinfo    *ailist, *aip;
    struct addrinfo    hint;
    int                sockfd, err, n;
    char            *host;

    if (argc != 1)
        err_quit("usage: ruptimed");
    if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX;    /* best guess */
    if ((host = malloc(n)) == NULL)
        err_sys("malloc error");
    if (gethostname(host, n) < 0)
        err_sys("gethostname error");
    daemonize("ruptimed");
    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
          gai_strerror(err));
        exit(1);
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
          aip->ai_addrlen, QLEN)) >= 0) {
            serve(sockfd);
            exit(0);
        }
    }
    exit(1);
}
```

#### **无连接的实例**

下面程序采用数据报套接字接口的uptime客户端命令版本

```
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN        128
#define TIMEOUT        20

void
sigalrm(int signo)
{
}

void
print_uptime(int sockfd, struct addrinfo *aip)
{
    int        n;
    char    buf[BUFLEN];

    buf[0] = 0;
    if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
        err_sys("sendto error");
    alarm(TIMEOUT);
    if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
        if (errno != EINTR)
            alarm(0);
        err_sys("recv error");
    }
    alarm(0);
    write(STDOUT_FILENO, buf, n);
}

int
main(int argc, char *argv[])
{
    struct addrinfo        *ailist, *aip;
    struct addrinfo        hint;
    int                    sockfd, err;
    struct sigaction    sa;

    if (argc != 2)
        err_quit("usage: ruptime hostname");
    sa.sa_handler = sigalrm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) < 0)
        err_sys("sigaction error");
    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
        err_quit("getaddrinfo error: %s", gai_strerror(err));

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0) {
            err = errno;
        } else {
            print_uptime(sockfd, aip);
            exit(0);
        }
    }

    fprintf(stderr, "can't contact %s: %s\n", argv[1], strerror(err));
    exit(1);
}
```

 下面是对应的服务器版本 

```
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>

#define BUFLEN        128
#define MAXADDRLEN    256

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
    int                n;
    socklen_t        alen;
    FILE            *fp;
    char            buf[BUFLEN];
    char            abuf[MAXADDRLEN];
    struct sockaddr    *addr = (struct sockaddr *)abuf;

    set_cloexec(sockfd);
    for (;;) {
        alen = MAXADDRLEN;
        if ((n = recvfrom(sockfd, buf, BUFLEN, 0, addr, &alen)) < 0) {
            syslog(LOG_ERR, "ruptimed: recvfrom error: %s",
              strerror(errno));
            exit(1);
        }
        if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
            sprintf(buf, "error: %s\n", strerror(errno));
            sendto(sockfd, buf, strlen(buf), 0, addr, alen);
        } else {
            if (fgets(buf, BUFLEN, fp) != NULL)
                sendto(sockfd, buf, strlen(buf), 0, addr, alen);
            pclose(fp);
        }
    }
}

int
main(int argc, char *argv[])
{
    struct addrinfo    *ailist, *aip;
    struct addrinfo    hint;
    int                sockfd, err, n;
    char            *host;

    if (argc != 1)
        err_quit("usage: ruptimed");
    if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX;    /* best guess */
    if ((host = malloc(n)) == NULL)
        err_sys("malloc error");
    if (gethostname(host, n) < 0)
        err_sys("gethostname error");
    daemonize("ruptimed");
    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
          gai_strerror(err));
        exit(1);
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_DGRAM, aip->ai_addr,
          aip->ai_addrlen, 0)) >= 0) {
            serve(sockfd);
            exit(0);
        }
    }
    exit(1);
}
```

#### **套接字选项**

套接字机制提供了设置跟查询套接字选项的接口。可以获取或设置以下3种选项

1.通用选项，工作在所有套接字类型上

2.在套接字层次管理的选项，但是依赖于下层协议的支持

3.特定于某协议的选项，每个协议独有的

可以使用setsockopt函数来设置套接字选项

```
#include <sys/socket.h>
int setsockopt(int sockfd,int level,int option,const void *val,socklen_t len);
```

level标识了选项应用的协议：

如果选项是通用的套接字层次选项，则level设置为SOL_SOCKET，否则，level设置成控制这个选项的协议编号。对于TCP选项，level是IPPROTO_TCP，对于IP，level是IPPROTO_IP。

下面总结了Single UNIX Specification中定义的通用套接字层次选项

<img src=".\img_apue\16-9.png" style="zoom:50%;" />

可以使用getsockopt函数来查看选项的当前值

```
#include <sys/socket.h>
int getsockopt(int sockfd,int level,int option,void *restrict val,socklen_t restrict lenp);
```

#### **带外数据**

带外数据是一些通信协议所支持的可选特征，允许更加高级的数据比普通数据优先传输。

TCP将带外数据称为紧急数据。TCP仅支持一个字节的紧急数据，但是允许紧急数据在普通数据传递机制数据流之外传输。

为了产生紧急数据，在三个send函数中任何一个指定标志MSG_OOB。如果带MSG_OOB标志传输字节超过一个时，最后一个字节被作为紧急数据字节。

如果安排发生套接字信号，当接收到紧急数据时，那么发送信号SIGURG信号。可以通过调用以下函数安排进程接收套接字的信号：

```
fcntl(sockfd,F_SETTOWN,pid);
```

F_GETOWN命令可以用来获取当前套接字所有权

```
owner=fcntl(sockfd,F_GETOWN,0);
```

为帮助判断是否已经达到紧急标记，可以使用函数sockatmark

```
#include <sys/socket.h>
int sockatmark(int sockfd);
```

当下一个要读取的字节在紧急标志处时，sockatmark返回1。

#### **非阻塞和异步I/O**

在基于套接字异步I/O中，当能够从套接字中读取数据，或者套接字写队列中的空间变得可用时，可以安排发送信号SIGIO。通过两个步骤来使用异步I/O：

1） 建立套接字拥有者关系，信号可以被传送到合适的进程。

2） 通知套接字当I/O操作不会阻塞时发信号告知。

可以使用三种方式来完成第一个步骤：

A、 在fcntl使用F_SETOWN命令

B、 在ioctl中作用FIOSETOWN命令

C、 在ioctl中使用SIOCSPGRP命令。

   要完成第二个步骤，有两个选择：

A、 在fcntl中使用F_SETFL命令并且启用文件标志O_ASYNC。

B、 在ioctl中使用FIOASYNC

## 第十七章 高级进程间通信

 本章介绍一种高级IPC---UNIX域套接字机制，并说明它的应用方法 

**UNIX域套接字**

UNIX域套接字用于在同一台计算机上运行的进程（无关进程）之间的（全双工）通信。相比于因特网套接字，UNIX域套接字效率更高。

UNIX域套接字仅仅复制数据，不执行协议处理，不需要添加或删除网络报头，无需计算校验和，不要产生顺序号，无需发送确认报文。

UNIX域套接字提供流和数据报两种接口，**就像是套接字和管道的混合（双向的通信）**，使用socketpair函数创建一对无命名的、相互连接的UNIX域套接字

```
#include <sys/socket.h>
int socketpair(int domain,int type,int protocol,int sockfd[2]);
```

也可以使用它们面向网络的域套接字接口（socket）建立该套接字。

一对相互连接的UNIX域套接字可以起到全双工管道的作用：两端对读和写开放。

<img src=".\img_apue\17-1.png" style="zoom:50%;" />

**命名UNIX域套接字**

上一章学习了然后将一个地址绑定到一个因特网域套接字上。跟因特网域套接字一样，可以命名UNIX域套接字，并可将其用于告示服务。

UNIX域套接字的地址由sockaddr_un结构表示，在Linux 3.2.0和Solaris 10中定义如下

```
struct sockaddr_un 
{
　　　　sa_family_t sun_family;   /* AF_UNIX */
　　　　char sun_path[UNIX_PATH_MAX];   /* pathname */
};
```

sun_path成员包含一个路径名。当我们将一个地址绑定到一个UNIX域套接字时，系统会用该路径名创建一个S_IFSOCK类型的文件。

该文件仅用于向用户进程告示套接字名字。该文件无法打开，也不能由应用程序用于通信。

下面程序是一个将地址绑定到UNIX域套接字的例子。

```
#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>

int
main(void)
{
    int fd, size;
    struct sockaddr_un un;

    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, "foo.socket");
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        err_sys("socket failed");
    size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
    if (bind(fd, (struct sockaddr *)&un, size) < 0)
        err_sys("bind failed");
    printf("UNIX domain socket bound\n");
    exit(0);
}
```

 下面是程序的运行情况 

<img src=".\img_apue\17-2.png" style="zoom:50%;" />

**唯一连接**

服务器进程可以使用标准的bind、listen和accept函数，为客户进程安排一个唯一UNIX域连接。

客户进程使用connect与服务器进程联系，在服务器进程接受了connect请求后，在服务器进程和客户进程之间就存在了唯一连接。

<img src=".\img_apue\17-3.png" style="zoom:50%;" />

<img src=".\img_apue\17-4.png" style="zoom:50%;" />

