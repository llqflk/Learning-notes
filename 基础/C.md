# C

# 1.类型，运算符与表达式

## 1.1.常量

整数常量：十进制、八进制或十六进制的整数类型数字。

字符常量： int 类型，适于转换到字符类型、char16_t、char32_t (C11起)或 wchar_t 类型的单个字符。

浮点常量： float 、 double 或 long double 类型值。

字符串常量：类型为 char[] 、 char16_t[] 、 char32_t[] (C11 起)或 wchar_t[] 的一系列字符，表示空终止字符串。

复合常量：直接嵌入程序代码的结构体、联合体或数组类型的值。

## 1.2. 变量

### 1.2.1. 固定类型

| TYPE          | 32BIT | 64BIT |
| ------------- | ----- | ----- |
| char          | 1     | 1     |
| short         | 2     | 2     |
| int           | 4     | 4     |
| unsigned int  | 4     | 4     |
| float         | 4     | 4     |
| double        | 8     | 8     |
| long          | 4     | 8     |
| long long     | 8     | 8     |
| unsigned long | 4     | 8     |

### 1.2.2. 动态类型

同类型

数组

字符串

不同类型

结构体



## 1.3. 数据类型转换

### 1.3.1. 隐式类型转换



### 1.3.2. 显式类型转换

（类型名）表达式



## 1.4 运算符

### 1.4.1 算数运算符

`\+ - * / %`



### 1.4.2 关系运算符

`\> >= < <= == !=`



### 1.4.3 逻辑运算符

`&& ||`



### 1.4.4 自增运算符与自减运算符

`++ --`



### 1.4.5 位运算符

`& | ^ << >> -`



### 1.4.6 赋值运算符

`= += -= *= /=`



# 2.控制流

## 2.1 选择语句

### 2.1.1 if-else

`if(表达式)`

​	`语句;`

`else`

​	`语句;`



### 2.1.2 switch

`switch(表达式)`

`{`

`case 常量表达式:语句序列`

`case 常量表达式:语句序列`

`default: 语句序列`

`}`



## 2.2 循环

### 2.2.1 while循环

`while(表达式)`

`{`

`语句`

`表达式;`

`}`



### 2.2.2 for循环

`for(表达式;表达式;表达式)`

`{`

​	`语句`

`}`



# 3.函数与程序结构

## 3.1声明



## 3.2 实现







# 4.标准库

```c
<assert.h>											
<float.h>											
<math.h>												
<stdarg.h>											
<stdlib.h>												
<ctype.h>											
<limits.h>											
<setjmp.h>												
<stddef.h>												
<string.h>												
<errno.h>												
<locale.h>												
<signal.h>												
<stdio.h>												
<time.h>
```

​											



## 4.1 <stdio.h> 输入与输出

### 4.1.1 文件操作

```c
FILE *fopen(const char *filename, const char *mode);`			
//返回stream，出错返回EOF	
```

```c
FILE *freopen(const char *filename, const char *mode, FILE *stream);`										
//一般用于改变与stdin, stdout和stderr相关联的文件
```

```c
int fflush(FILE *stream);`										
//将已写入缓冲区但尚未写入文件的所有数据写入到文件中`	
```

```c
int fclose(FILE *stream);										
//将未写入到数据写入stream中，丢弃缓冲区中的所有未读输入数据，并释放自动分配的全部缓冲区	
```

```c
int remove(const char *filename);								
//删除filename指定的文件，后续试图打开该文件的操作将失败。如果删除操作失败，则返回一个非0值							
```

```c
int rename(const char *oldname, const char *newname);					
//修改文件名字，如果操作失败，返回一个非0值								
```



### 4.1.2 格式化输出

```c
int fprintf(FILE *stream, const char *format,...);		
int printf(const char *format, ...);			
int sprintf(char *s, const char *format, ...);	
```

### 4.1.3 格式化输入

```c
int fscanf(FILE *stream, const char *format, …);	
int scanf(const char *format, ...)		
int sscanf(const char *s, const char *format, ...)			
```

### 4.1.4 字符输入/输出函数

```c
int fget(FILE *stream);										
//返回stream流的下一个字符，返回类型为unsigned char（被转换为int），失败返回EOF
```

```c
char *fgets(char *s, int n, FILE *stream)							
//最多将n-1个字符读入到数字s中，遇到换行符时，把换行符读入到数组s中，读取过程终止。数组s以'\0'结尾。fgets函数返回	
```

```c
int fputc(int c, FILE *stream);									
//把字符c输出到流stream中，返回写入的字符，失败返回EOF		
```

```c
int fputs(const char *s, FILE *stream)
//把字符串s输出到流stream中，它返回一个非负值
```

```c
int getc(FILE *stream);
```

```c
int getchar();
```

```c
char *gets(char *s);
```

```c
int putc(int c, FILE *stream);
```

```c
int putchar();
```

```c
int puts(const char *s);
int ugetc(int c, FILE *stream);
```

### 4.1.5 直接输入/输出函数

```c
size_t fread(void *ptr, size_t size, size_t nobj, FILE *stream);
```

```c
size_t fwrite(const void *ptr, size_t size, size_t nobj, FILE *stream);
```



### 4.1.6 文件定位函数

```c
int fseek(FILE *stream, long offset, int origin);
long ftell(FILE *stream);
void rewind(FILE *stream);
int fgetpos(FILE *stream, fpos_t *ptr);
int fsetpos(FILE *stream, const fpos_t *ptr);
```



### 4.1.7 错误处理函数

```c
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *s);
fprintf(stderr, “%s: %s\n”, s, “error_message”);
```



## 4.2 字符类别测试<ctype.h>

```c
isalnum(c)
isalpha(c)
iscntrl(c)
isdigit(c)
isgraph(c)
islower(c)
isprint(c)
ispunct(c)
isspace(c)
isupper(c)
isxdigit(c)
```



## 4.3 字符串函数<string.h>

```c
char *strcpy(s, ct);
char *strncpy(s, ct, n);
char *strcat(s, ct);
char *strncat(s, ct, n);
int strcmp(cs, ct);
int strncmp(cs, ct, n);
char *strchr(cs, c);
char *strrchr(cs, c);
size_t strspn(cs, ct);
size_t strcspn(cs, ct);
char *strpbrk(cs, ct);
char *strstr(cs, ct);
size_t strlen(cs);
char *strerror(n);
char *strtok(s, ct);
```

```c
void *memcpy(s, ct, n);
void *memmove(s, ct, n);
int memcmp(cs, ct, n);
void *memchr(cs, c, n);
void *memset(s, c, n);
```

## 4.4 数学函数<math.h>

```c
sin(x)
cos(x)
tan(x)
asin(x)
acos(x)
atan(x)
atan2(x)
sinh(x)
cosh(x)
tanh(x)
exp(x)
log(x)
log10(x)
pow(x)
sqrt(x)
ceil(x)
floor(x)
fabs(x)
ldexp(x)
frexp(x)
modf(x)
fmod(x)
```



## 4.5 实用函数<stdlib.h>

```c
double atof(const char *s);
int atoi(const char *s);
long atol(const char *s);
double strtod(const char *s, char **endp);
long strtol(const char *s, char **endp, int base);
unsigned long strtoul(const char *s, char **endp, int base);
int rand();
void srand(unsigned int seed);
void *calloc(size_t nobj, size_t size);
void *malloc(size_t size);
void *realloc(void *p, size_t size);
void free(void *p);
void abort();
void exit(int status);
int atexit(void (*fcn)());
int system(const char *s);
char *getenv(const char *name);
void *bsearch(const void *key, const void *base, size_t n, size_t size, 		int (*cmp)(const void *keyval, const void *datum));
void qsort(void *base, size_t n, size_t size, int (*cmp)(const void *keyval, const void *))
int abs(int n);
long labs(long n);
div_t div(int num, int denom);
ldiv_t ldiv(long num, long denom);
```



## 4.6 诊断<assert.h>

```c
void assert(int 	表达式)
assert(表达式)
```



## 4.7 可变参数表<stdarg.h>

```c
va_list ap;
va_start(va_list ap, lastarg);
void va_end(va_list ap);
```



## 4.8 非局部跳转<setjmp.h>

```c
int setjmp(jmp_buf env);
```



## 4.9 信号<signal.h>

```c
int raise(int sig);
```



## 4.10 日期和时间函数<time.h>

```c
clock_t clock();
time_t time(time_t *tp);
double difftime(time_t time2, time_t time1);
time_t mktime(struct tm *tp);
char *asctime(const struct tm *tp);
char *ctime(const time_t *tp);
asctime(localtime(tp));
struct tm *gmtime(const time_t *tp);
struct tm *localtime(const time_t *tp);
size_t strftime(char *s, size_t smax, const char *fmt, const struct tm *tp)
```



## 4.11 与具体实现相关的限制<limits.h>和<flost.h>