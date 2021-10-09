## 第一章 简介

#### **环境搭建**

1.下载解压unpv13e.tar.gz

2.进入目录执行

```
./configurecd lib   //进入lib目录make     //执行make命令
```

3.将生成的libunp.a静态库复制到/usr/lib/和/usr/lib64/中  

```
cd ..   //返回unpv13e目录sudo cp libunp.a /usr/lib  
sudo cp libunp.a /usr/lib64  
```

4.修改unp.h并将其和config.h拷贝到/usr/include中，为了以后include方便

```
vi lib/unp.h   
//将unp.h中#include "../config.h"修改为#include "config.h"  
sudo cp lib/unp.h /usr/include  
sudo cp config.h /usr/include  
```

5.编译源代码

```
cd ./intro  
gcc -o daytimetcpcli daytimetcpcli.c -lunp
```

跟普通的编译不一样的是要在最后加上刚才那个链接库，-l参数加上刚才那个libunp.a去掉lib和后面的.a。最后得到参数-lunp。

 

 

下面是socket网络编程的实例，代码将在后面章节进行详细说明

#### **一个简单的时间获取客户程序**

```
#include    "unp.h"

int
main(int argc, char **argv)
{
    int                    sockfd, n;
    char                recvline[MAXLINE + 1];
    struct sockaddr_in    servaddr;

    if (argc != 2)
        err_quit("usage: a.out <IPaddress>");

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(13);    /* daytime server */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_quit("inet_pton error for %s", argv[1]);

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");

    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;    /* null terminate */
        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }
    if (n < 0)
        err_sys("read error");

    exit(0);
}
```

上面是TCP当前时间查询客户程序的一个实现。该客户与服务器建立一个TCP连接后，服务器以直观可读格式简单地送回当前时间和日期。

 

 

#### **协议无关性**

上面程序与IPv4协议相关。修改该程序，让它能够在IPv6运行

```
#include    "unp.h"

int
main(int argc, char **argv)
{
    int                    sockfd, n;
    struct sockaddr_in6    servaddr;
    char                recvline[MAXLINE + 1];

    if (argc != 2)
        err_quit("usage: a.out <IPaddress>");

    if ( (sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        err_sys("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port   = htons(13);    /* daytime server */
    if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) <= 0)
        err_quit("inet_pton error for %s", argv[1]);

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");

    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;    /* null terminate */
        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }
    if (n < 0)
        err_sys("read error");

    exit(0);
}
```

**包裹函数**

本书例子通过包裹函数来进行错误处理：约定包裹函数名是实际函数名的首字母大写形式，例如Socket函数是socket的包裹函数，下面代码将演示这个包裹函数

```
int
Socket(int family, int type, int protocol)
{
    int n;
    if( (n = socket(family, type, protocol)) < 0)
        err_sys("socket error");
    return (n);
}
```

 **一个简单的时间获取服务器程序** 

```
#include    "unp.h"
#include    <time.h>

int
main(int argc, char **argv)
{
    int                    listenfd, connfd;
    struct sockaddr_in    servaddr;
    char                buff[MAXLINE];
    time_t                ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(13);    /* daytime server */

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for ( ; ; ) {
        connfd = Accept(listenfd, (SA *) NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

        Close(connfd);
    }
}
```

**测试程序**

使用root用户使服务器程序从后台运行

<img src=".\img_unp\1-1.png" style="zoom:50%;" />

 运行第一个客户端程序 
 <img src=".\img_unp\1-2.png" style="zoom:50%;" />

 127.0.0.1是环回地址。本例子中客户端程序连接到服务器程序，服务器程序将时间跟日期发送给客户端，然后客户端将它打印到终端中。 

## 第二章 传输层

本章的焦点是传输层，包括TCP、UDP和SCTP。

绝大多数客户/服务器网络应用使用TCP或UDP。SCTP是一个较新的协议。

UDP是一个简单的、不可靠的数据报协议。而TCP是一个复杂、可靠的字节流协议。

这里重点放在TCP中。

#### **用户数据报协议（UNP）**

应用程序往UDP套接字写入一个消息，该消息随后被封装到一个UDP数据报，该UDP数据报进而又被封装到一个IP数据报，然后发送到目的地（面向非连接的）。

UDP不保证UDP数据报会达到最终目的地，不保证各个数据报的先后顺序跨网路后保持不变，也不保证每个数据报只到达一次。


#### **传输控制协议（TCP）**

相对于UDP，TCP客户先于某个给定的服务器建立一个连接，再跨该连接与那个服务器交换数据，然后终止连接（面向连接的）。

其次，TCP还提供了可靠性：当TCP向另一端发送数据时，它要求对端返回一个确认。如果没有收到确认，TCP就自动重传数据并等待更长的时间。在数次重传失败后，TCP才放弃。

 

 

 

#### **TCP连接的建立和终止**

三次握手，建立一个TCP连接时会发生下述情况

![img](.\img_unp\1-3.png)

 

1.服务器必须准备好接受外来连接。这通常通过调用socket、bind和listen这3个函数来完成，我们称之为被动打开。

2.客户通过调用connect发起主动打开。这导致客户TCP发送一个SYN（同步）分节，它告诉服务器将在（待建立）连接中发送的数据的初始序列号（J）。

3.服务器必须确认（ACK）客户的SYN，同时自己也得发送一个SYN分节，它含有服务器将在同一连接中发送数据的初始序列号（K）。

4.客户必须确认服务器的SYN。

图中客户的初始序列号是J，服务器的初始序列号是K。ACK中的确认号是发送这个ACK的一段所期待的下一个序列号。


#### **TCP连接终止**

TCP终止一个连接需要4个分节

![img](.\img_unp\1-4.png)

1.某个应用进程首先调用close，我们称该端执行主动关闭。该端的TCP于是发送一个FIN分节，表示数据发送完毕。

2.接受到这个FIN的对端执行被动关闭。这个FIN由TCP确认。它的接收也作为一个文件结束符传递给接收端应用程序。

3.一段时间后，接收到这个文件结束符的应用进程将调用close关闭它的套接字。这导致它的TCP也发送一个FIN。

4.接收这个最终FIN的原发送端TCP确认这个FIN。

 

 

 

#### **TCP状态转换图**

![img](.\img_unp\1-5.png)

 

 

 

#### **观察分组**

下面展示了一个完整的TCP连接所发生的实际分组交换情况。

![img](.\img_unp\1-6.png)

 

 

#### **TIME_WAIT状态**

TIME_WAIT状态有两个存在的理由：

1.可靠地实现TCP全双工连接的终止；

2.允许老的重复分节在网络中消逝。

## 第三章 套接字编程简介

本章开始讲解套接字API。

#### **套接字地址结构**

###### **IPv4套接字地址结构**

它以sockaddr_in命名，下面给出它的POSIX定义

```c
struct in_addr
{
     in_addr_t s_addr;                    
};

stuct sockaddr_in
{
     uint8_t         sin_len;                      
     sa_family_t     sin_family;    /* AF_INET */
     in_port_t       sin_port;       
     struct in_addr  sin_addr;
     char            sin_zero[8];
};
```

POSIX规范只需要这个结构中的3个字段：sin_family、sin_port和sin_addr。

下图列出了POSIX定义的关于socket编程的数据类型

![img](.\img_unp\3-1.png)

 

###### **通用的套接字地址结构**

为了解决任意套接字地址结构都可以传递进使用套接字地址的套接字函数中，定义了一个通用的套接字地址结构。

任何套接字地址结构的指针都通过类型强制转换（变成通用套接字地址结构的指针）来做为函数参数。

通用的套接字地址结构如下所示

```c
struct sockaddr
{
    uint8_t        sa_len;
    sa_family_t    sa_family;              
    char           sa_data[14];            
};
```

######  **IPv6套接字地址结构** 

```
struct in_addr
{
     in_addr_t     s_addr;                  
};

struct sockaddr_in
{
     uint8_t            sin_len;        
     sa_family_t        sin_family;       /* AF_INET6 */
     in_port_t          sin_port;          
     struct in_addr     sin_addr;          
     char               sin_zero[8];        
};
```

#### **字节排序函数**

网络字节序采用大端字节序，而不同主机使用不同的字节序。

下面4个函数用于主机字节序和网络字节序之间的相互转换

```c
#include <netinet/in.h>
uint16_t htons(uint16_t host16bitvalue);
uint32_t htonl(uint32_t host32bitvalue);
                                           //均返回：网络字节序的值；
uint16_t ntohs(uint16_t net16bitvalue);
uint32_t ntohl(uint32_t net32bitvalue);
                                           //均返回：主机字节序的值；
```

 h代表host，n代表network，s代表short，l代表long。 

#### **字节操纵函数**

下面函数用于字节的设置、复制和比较。

```c
#include <strings.h>
void bzero(void *dest, size_t nbytes);
void bcopy(const void *src, void *dest, size_t nbytes);
void bcmp(const void *ptr1, const void *ptr2, size_t nbytes);
                                     //返回：相等则为0，否则为非0
```

```c
#include <string.h>
void *memset(void *dest, int c, size_t nbytes);
void *memcpy(void *dest, const void *src, size_t nbytes);
int   memcmp(const void *s1, const void *s2, size_t nbytes);
                                    //返回：相等则为0，否则为非0
```

#### **函数inet_aton、inet_addr和inet_ntoa**

这几个函数用于在点分十进制数串（如“206.168.112.96”）于它长度为32位的网络字节序二进制值间转换IPv4地址。

```c
#include <arpa/inet.h>
int_addr_t inet_addr(const char *strptr);
                //已被舍弃，使用inet_aton函数代替
int inet_aton(const char *strptr, struct in_addr *addrptr);
                //返回：字符串有效返回1，否则为0
char *inet_ntoa(struct in_addr inaddr);
                //返回：指向一个点分十进制的字符串的指针
```

#### **函数inet_pton和inet_ntop**

相对于上面的几个函数，这两个函数对于IPv4地址和IPv6地址都适用

```c
#include <arpa/inet.h>
int inet_pton(int family, const char *strptr, void *addrptr);
                                      //返回：若成功返回1，输入不是有效表达格式返回0，若出错返回-1
const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
                                       //返回：若成功则返回指向结果的指针，出错则为NULL
```

p代表表达（presentation）  n代表数值（numeric）

family参数既可以是AF_INET，也可以是AF_INET6。

inet_pton尝试转换由strptr指针指向的字符串，并通过addrptr指针存放二进制结果。

inet_ntop进行相反的转换，len参数是目标储存单元的大小，以免该函数一处其调用者的缓冲区。对此，<netinet/in.h>头文件中由如下定义：

```
#define INET_ADDRSTRLEN            16              
#define INET6_ADDRSTRLEN           46        
```

 

 

#### **UNP中定义的几个函数**

inet_ntop函数的一个基本问题是：要求调用者必须知道这个结构的格式和地址族

我们可以使用自己定义的sock_ntop函数来解决这个问题

```
#include "unp.h"
char *sock_ntop(const struct sodkaddr *sockaddr, socklen_t addrlen);
```

该函数内部使用sockaddr指针来确定地址族，然后调用inet_ntop。

同样的，我们为操作套接字地址结构定义了如下几个函数

![img](.\img_unp\3-2.png)

#### **readn、writen和readline函数**

字节流套接字（例如TCP套接字）上read和write函数所表现的行为不同于通常的文件

字节流套接字上调用read或write输入或输出的字节数可能比请求的数量少，然而这不是出错的状态，此时所需的是调用者再次调用read或write函数，以输入或输出剩余的字节。

我们提供一下3个函数是每当我们读或写一个字节流套接字时总要使用的函数

```c
#include "unp.h"
ssize_t readn(int filedes,void *buff,size_t nbytes);
ssize_t writen(int filedes,const void *buff,size_t nbytes);
ssize_t readline(int filedes,void *buff,size_t nbytes); 
                         //返回值：读或写的字节数，若出错则为-1
```

 readn实现代码 

```c
#include "unp.h"
ssize_t readn(int fd,void *vptr,size_t n)
{
    size_t  nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;
    while(nleft > 0)
    {
        if((nread = read(fd,ptr,nleft)) < 0 )
        {
            if(errno == EINTR)
            {
                nread = 0;
            }
            else
            {
                return -1;
            }
        }
        else if(nread == 0)
        {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (n-nleft);
}
```

 writen实现代码 

```
#include "unp.h"
ssize_t written(int fd,void *vptr,size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = n;
    while(nlfet >  0)
    {
        if((nwritten = writen(fd,ptr,nleft)) <= 0)
        {
            if(nwritten < 0 && errno ==EINTR)
            {
                nwritten =0;
            }
            else
            {
                return -1;
            }
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}
```

 readline实现代码 

```
#include "unp.h"
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
     ssize_t n,rc;
     char    c,*ptr;

     ptr = vptr;
     for(n = 1; n < maxlen; n++)
     {
       if( (rc = read(fd, &c)) == 1)
       {
         *ptr ++ = c;
         if(c == '\n')
           break;
       }
       else if (r c== 0)
       {
          *ptr = 0;
          return (n-1);
       }else
          return (-1);
     }

     *ptr = 0;
     return (n);
}
```

## 第四章 基本TCP套接字

本章讲解编写一个完整的TCP客户/服务器程序所需要的基本套接字函数。

 

 

#### **socket函数**

```
#include <sys/socket.h>
int socket(int family,int type,int protocol);
            //返回：成功则为非负描述符，若出错则为-1
```

family参数指明协议族，它是如下某个常值

![img](.\img_unp\4-1.png)

type参数指明套接字类型，它是如下某个常值

![img](.\img_unp\4-2.png)

protocol参数为下面某个协议类型常值，或者设为0，以选择所给定family和type组合的系统默认值

![img](.\img_unp\4-3.png)

下图展示了基本TCP客户/服务器程序的套接字函数

![img](.\img_unp\4-4.png)

 

 

 

#### **connect函数**

TCP客户用connect函数来建立与TCP服务器的连接

```
#include <sys/socket.h>
int connect(int sockfd,const struct sockaddr *servaddr,socklen_t addrlen)
                                           //返回：若成功则为0，若出错则为-1
```

sockfd是由socket函数返回的套接字描述符，第二个、第三个参数分别是一个指向套接字地址结构的指针和该结构的大小。

 

 

 

#### **bind函数**

bind函数把本地协议地址赋予一个套接字

```
#include <sys/socket.h>
int bind(int sockfd,const struct sockaddr *myaddr,socklen_t addrlen);
                                        //返回：若成功则为0，若出错则为-1
```

 

 

#### **listen函数**

listen函数仅由TCP服务器调用，它做两件事情。

1.当socket函数创建一个套接字时，它被假设为一个主动套接字（将调用connect发起连接的客户端套接字）

  listen函数把未连接的套接字转换成一个被动套接字，指示内核应接受该套接字的连接请求。

2.本函数的第二个参数规定了内核应该为相应套接字排队的最大连接个数。

```c
#include <sys/socket.h>
int listen(int sockfd,int backlog);
             //返回：若成功则为0，若出错则为-1
```

本函数通常应该在调用socket和bind这两个函数之后，并在调用accept函数之前调用。

 

 

 

#### **accept函数**

accept函数由TCP服务器调用，用于从已完成连接队列队头返回下一个已完成连接。如果已完成连接队列为空，那么进程被投入睡眠。

```c
#include <sys/socket.h>
int accept(int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);
                              //返回：若成功则为非负描述符，若出错则为-1
```

参数cliaddr和addrlen用来返回已连接的对端进程（客户）的协议地址。

如果accept成功，那么其返回值是由内核自动生成的一个全新描述符，代表与所返回客户的TCP连接。

在讨论accept函数时，我们称它的第一个参数为监听套接字（由socket创建，随后用作bind和listen的第一个参数的描述符），称它的返回值为已连接套接字描述符。

下面程序演示了上面全部函数的用法，这个服务器是一个迭代服务器。是一个显示客户IP地址和端口号的时间获取服务器程序。

```c
#include    "unp.h"
#include    <time.h>

int
main(int argc, char **argv)
{
    int                    listenfd, connfd;
    socklen_t            len;
    struct sockaddr_in    servaddr, cliaddr;
    char                buff[MAXLINE];
    time_t                ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(13);    /* daytime server */

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for ( ; ; ) {
        len = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *) &cliaddr, &len);
        printf("connection from %s, port %d\n",
               Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
               ntohs(cliaddr.sin_port));

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

        Close(connfd);
    }
}
```

**fork和exec函数**

fork函数是UNIX中派生新进程的唯一方法

```
#include <unistd.h>
pid_t fork(void);
         //返回：在子进程中为0，在父进程中为子进程ID，若出错则为-1
```

存放在硬盘上的可执行程序文件能够被UNIX执行的唯一方法是现有调用6个exec函数的某一个

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


exec把当前进程映像替换成新的程序文件，而且该新程序通常从main函数开始执行。

关于fork和exec函数可以查看之前写的apue的文章：http://www.cnblogs.com/runnyu/p/4638913.html

**并发服务器**

上面有一个迭代服务器的例子，对于像时间获取这样的简单服务器来说，这就够了。

然后当服务一个客户请求可能花费较长时间时，我们并不希望整个服务器被单个客户长期占用，而是希望同时服务多个客户。

UNIX编写并发服务器程序最简单的办法就是fork一个子进程来服务每个客户，下面给出了一个典型的并发服务器程序的轮廓。

```
pid_t pid;
int listenfd,connfd;
listenfd=Socket(...);
        /* fill in sockaddr_in{} with server'swell-known port */
Bind(listenfd,...);
Listen(listenfd,LISTENQ);
for(;;)
{
      connfd=Accept(listenfd,...);  /* probably blocks */
      if((pid=Fork())==0)
      {
             Close(listenfd);   /* child closes listening socket */
             doit(connfd);      /* process the request */
             Close(connfd);    /* done with this client */
             exit(0);              /* child terminates */
      }
      Close(connfd)            /* parent closes connected socket */
}
```

当一个连接建立时，accept返回，服务器接着调用fork，然后由子进程服务客户（通过已连接套接字connfd），父进程等待另一个连接（通过监听套接字listenfd） 

#### **close函数**

通常的UNIX close函数也可用来关闭套接字。

```
#include <unistd.h>
int close(int sockfd);
         //返回：若成功则为0，若出错则为-1
```

描述符引用计数

在上面并发服务器框架中，父进程关闭已连接套接字只是导致相应描述符的应用计数值减1。

既然引用计数值大于0，这个close调用并不引发TCP的四分组连接终止序列，知道子进程使用close关闭该套接字（引用次数为0）才终止TCP连接。

#### **getsockname和getpeername函数**

getsockname返回某个套接字关联的本地协议地址，getpeername函数返回与某个套接字关联的外地协议地址。

```
#include <sys/socket.h>
int getsockname(int sockfd,struct sockaddr *localaddr,socklen_t *addrlen);
int getpeername(int sockfd,struct sockaddr *peeraddr,socklen_t *addrlen);
                                           //均返回：若成功则为0，若出错则为-1
```

## 第五章 TCP客户/服务程序示例

我们将在本章使用前一章中介绍的基本函数编写一个完整的TCP客户/服务器程序实例

这个简单得例子是执行如下步骤的一个回射服务器：

<img src=".\img_unp\5-1.png" style="zoom:50%;" />

####  **TCP回射服务器程序** 

```
#include    "unp.h"

int
main(int argc, char **argv)
{
    int                    listenfd, connfd;
    pid_t                childpid;
    socklen_t            clilen;
    struct sockaddr_in    cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

        if ( (childpid = Fork()) == 0) {    /* child process */
            Close(listenfd);    /* close listening socket */
            str_echo(connfd);    /* process the request */
            exit(0);
        }
        Close(connfd);            /* parent closes connected socket */
    }
}
```

 str_echo函数 

```
#include    "unp.h"

void
str_echo(int sockfd)
{
    ssize_t        n;
    char        buf[MAXLINE];

again:
    while ( (n = read(sockfd, buf, MAXLINE)) > 0)
        Writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("str_echo: read error");
}
```

####  **TCP回射客户程序** 

```c
#include    "unp.h"

int
main(int argc, char **argv)
{
    int                    sockfd;
    struct sockaddr_in    servaddr;

    if (argc != 2)
        err_quit("usage: tcpcli <IPaddress>");

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);        /* do it all */

    exit(0);
}
```

 str_cli函数 

```c
#include    "unp.h"

void
str_cli(FILE *fp, int sockfd)
{
    char    sendline[MAXLINE], recvline[MAXLINE];

    while (Fgets(sendline, MAXLINE, fp) != NULL) 
    {

        Writen(sockfd, sendline, strlen(sendline));

        if (Readline(sockfd, recvline, MAXLINE) == 0)
            err_quit("str_cli: server terminated prematurely");

        Fputs(recvline, stdout);
    }
}
```

**正常启动**

在后台启动服务器

<img src=".\img_unp\5-2.png" style="zoom:50%;" />

服务器启动后，它将阻塞于accept调用。运行netstat程序来检查服务器监听套接字状态

![](.\img_unp\5-3.png)

端口9877是我们服务器使用的端口，netstat用星号“*”表示一个为0的IP地址（INADDR_ANY 通配地址）或为0的端口号

使用环回地址启动客户端

![](.\img_unp\5-4.png)

客户端程序调用socket和connest，将阻塞带fgets调用。

服务器中的accept返回，然后调用fork，子进程调用str_echo，阻塞于read函数。父进程再次调用accept并阻塞。

此时，我们有3个都在睡眠（即已阻塞）：客户进程、服务器父进程和服务器子进程

使用nestat给出对应所建立TCP连接。

![](.\img_unp\5-5.png)

![](.\img_unp\5-6.png)

第一个是服务器父进程，第二个是客户进程，第三个是服务器子进程。

正常终止程序

<img src=".\img_unp\5-7.png" alt="5-7" style="zoom: 67%;" />

我们输入两行数据，每行都得到回射。我们接着键入终端EOF字符Ctrl+D以终止客户（导致fgets返回一个空指针）

**POSIX信号处理**

关于信号我们可以查看以前写的apue学习笔记 http://www.cnblogs.com/runnyu/p/4641346.html

关于进程可以查看  http://www.cnblogs.com/runnyu/p/4638913.html

这是后面章节的基本知识（例如signal、wait函数）

#### **处理SIGCHLD信号**

设置僵死状态的目的是维护子进程的信息，以便父进程在以后某个时候获取。

**处理僵死进程**

在一个进程终止或者停止时，将SIGCHLD信号发送给其父进程。按系统默认将忽略此信号。

我们可以在listen调用之后捕获SIGCHLD信号用来处理僵死进程

