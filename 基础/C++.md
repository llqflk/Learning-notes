# 1.变量和基本类型

## 1.1 常量

整型：

浮点型：

字符：

字符串：

转义序列：

布尔：true false

指针：nullptr

## 1.2 变量

### 1.2.1变量定义

内置类型

整型：

bool

char

int 

wchar_t

char16_t

char32_t

short

int

long

long long

浮点型：

float

double

long double





复合类型

引用

指针

string 

​	初始化

​		string s1 = “hiya”; //拷贝初始化

​		string s2{“hiya”} //直接初始化







类型转换



初始化

1’列表初始化

int units_sold = 0;

int units_sold = {0};

int units_sold{0};

int units_sold(0);

使用列表初始化且存在丢失信息的风险，则编译器将报错

long double d = 3.1415

int a{ld}, b = {ld};//错误

int c(ld), d = ld;//正确，且确实丢失了部分价值



2’默认初始化

函数之外的变量被初始化为0

函数体内部的变量将不被初始化









1.2.2标识符

1.2.3作用域

1.2.4const限定符

只读

const int bufsize = 512







# 2.表达式

## 2.1运算符

算术运算符

\+ = * / %

逻辑和关系运算符

! < <= > >= == != && ||

赋值运算符

=

递增和递减运算符

++ —

成员访问运算符

ptr->mem

(*ptr).mem

条件运算符

cond?expr1:expr2

位运算符

～ 

<<

\>>

&

^

|



sizeof运算符



# 3.语句

## 3.1条件语句



## 3.2循环语句

# 4.函数

## 4.1函数基础



## 4.2参数传递



## 4.3返回类型



## 4.4函数重载



# 5.面向对象程序设计

# 6.模板与泛型编程