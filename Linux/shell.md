# Shell编程
## 变量

与编译型语言不同，shell脚本是一种解释型语言。

执行这类程序时，解释器(interpreter)需要读取我们编写的源代码(source code)，并将其转换成目标代码(object code)，再由计算机运行。

Linux默认的脚本解析器是bash。

#### **第一个Shell脚本**
下面演示我们的第一个shell脚本，使用文本编辑器（如vi），键入如下代码并保存为hello.sh

```shell
#!/bin/bash
# 打印hello world
echo "Hello World !"
```

 然后给该脚本赋予执行权限 

```shell
chmod +x ./hello.sh
```

然后执行该脚本，终端就会显示Hello World!

其中脚本第一行`#!/bin/bash` 指明了该脚本的解释器

sh里没有多行注释，只能每一行加一个#号

#### **read命令**

下面的脚本使用`read`命令从 `stdin` 获取输入并赋值给 `PERSON` 变量，最后在 `stdout`上输出 

```c
#!/bin/bash
echo "What is your name?"
read PERSON
echo "Hello, $PERSON"
```

#### **Shell变量**

定义变量

```shell
variableName="value"
```

使用一个定义过的变量，只要在变量名前面加美元符号（$）即可，如：

```shell
name="runnyu"
echo ${name}
```

花括号可加可不加，加花括号是为了帮助解释器识别变量的边界。使用readonly+变量名可以将变量定义为只读变量，unset+变量名可以删除变量

```shell
readonly name
unset name
```

 #### **特殊变量**

![](.\img_shell\1_1.png)

 编写special.sh测试 

```shell
#!/bin/sh
echo "pid=$$"
echo "$0"
echo "$1"
echo "$2"
echo "$#"
echo "$*"
```

##  替换

#### **转义字符**

```shell
#!/bin/bash
a=10
echo -e "Value of a is $a \n"
```

使用-e表示对转义字符进行替换，默认情况是不转义的

#### **命令替换**

命令替换的语法，注意是反引号，不是单引号，这个键位于 Esc 键下方。

```shell
`command`
```

可以将命令执行的结果保存在变量中，在需要的时候使用

```shell
#!/bin/bash
DATE=`date`
echo "now is ${DATE}"
```

#### **变量替换**

变量替换可以根据变量的状态（是否为空、是否定义等）来改变它的值

可以使用的变量替换形式：

![](.\img_shell\2_1.png)

下面是变量替换的一个例子

```shell
#!/bin/bash

echo ${var:-"Variable is not set"}
echo "1 - Value of var is ${var}"

echo ${var:="Variable is not set"}
echo "2 - Value of var is ${var}"

unset var
echo ${var:+"This is default value"}
echo "3 - Value of var is $var"

var="Prefix"
echo ${var:+"This is default value"}
echo "4 - Value of var is $var"

echo ${var:?"Print this message"}
echo "5 - Value f var is ${var}"
```

## 运算符

#### **算术运算符**

原生bash不支持简单的数学运算，但是可以通过其他命令来实现，例如 awk 和 expr，expr 最常用。

expr 是一款表达式计算工具，使用它能完成表达式的求值操作。

例如：实现两个数相加

```
#!/bin/bash
val=`expr 2 + 2`
echo "Total value : ${val}"
```

**表达式和运算符之间要有空格**，可以把每个表达式和运算符都看做是一个参数传递到expr

下面是算术运算符列表，其中乘号(*)前边必须加反斜杠(\)才能实现乘法运算

<img src="D:\learn\linuxlearn\5_Linux\img_shell\3_1.png" style="zoom:67%;" />

**关系运算符**

关系运算符只支持数字，不支持字符串，除非字符串的值是数字。

下面是关系运算符列表

<img src="D:\learn\linuxlearn\5_Linux\img_shell\3_2.png" style="zoom:75%;" />

 下面给出一个关系运算符的例子 

```
#!/bin/sh
a=10
b=20
if [ $a -eq $b ]
then
   echo "$a -eq $b : a is equal to b"
else
   echo "$a -eq $b: a is not equal to b"
```

 注意：条件表达式要放在方括号之间，并且**要有空格** 

**布尔运算符**

下面是一个布尔运算符的例子。其中**-o是或运算，-a是与运算**

```
#!/bin/sh

a=10
b=20

if [ $a != $b ]
then
   echo "$a != $b : a is not equal to b"
fi

if [ $a -lt 100 -a $b -gt 15 ]
then
   echo "$a -lt 100 -a $b -gt 15 : returns true"
fi

if [ $a -lt 100 -o $b -gt 100 ]
then
   echo "$a -lt 100 -o $b -gt 100 : returns true"
fi
```

**字符串运算符**

下面是字符串运算符列表

![3_3](D:\learn\linuxlearn\5_Linux\img_shell\3_3.png)

 下面是一个字符串运算符的例子 

```
#!/bin/sh
a="abc"
b="efg"
if [ $a = $b ]
then
   echo "$a = $b : a is equal to b"
else
   echo "$a = $b: a is not equal to b"

if [ -z $a ]
then
   echo "-z $a : string length is zero"
else
   echo "-z $a : string length is not zero"
fi

if [ -n $a ]
then
   echo "-n $a : string length is not zero"
else
   echo "-n $a : string length is zero"
fi

if [ $a ]
then
   echo "$a : string is not empty"
else
   echo "$a : string is empty"
fi
```

#### **文件测试运算符**

下面是文件测试运算符列表

<img src="D:\learn\linuxlearn\5_Linux\img_shell\3_4.png" style="zoom:75%;" />

```
#!/bin/bash
file="/home/runnyu/demo.c"
if [ -r $file ]
then
   echo "File has read access"
else
   echo "File does not have read acces"
fi
```

## 字符串

字符串可以用单引号，也可以用双引号，也可以不用引号。

#### **单引号**

```
str='this is a string'
```

单引号字符串的限制：

1.单引号里的任何字符都会原样输出，单引号字符串中的变量是无效的；

2.单引号字串中不能出现单引号（对单引号使用转义符后也不行）。

#### **双引号**

```
name='runnyu'
str="Hello, ${name} \n"
```

双引号的优点：

1.双引号里可以有变量

2.双引号里可以出现转义字符

#### **获取字符串长度**

```
string="abcd"
echo ${#string} #输出 4
```

#### **提取字字符串**

```
string="hello world"
echo ${string:1:4} #  ${string:index:length}
```

#### **查找子字符串**

```
string="hello world"
echo `expr index "$string" world`   #   `expr index "$string" substring`
```

## 数组

 bash支持一维数组，并且没有限定数组的大小，数组元素的下标由0开始编号。 

#### **定义数组**

在Shell中，用括号来表示数组，数组元素用“空格”符号分割开。定义数组的一般形式为：

```shell
array_name=(value0 value1 value2 value3)
```

也可以单独定义每个数组元素

```shell
array_name[0]=value0
array_name[1]=value1
array_name[2]=value2
```

#### **读取数组**

读取数组元素值的一般格式是：

```shell
${array_name[index]}
```

使用@或*可以获取数组中的所有元素:

```shell
${array_name[*]}
${array_name[@]}
```

#### **获取数组的长度**

获取数组长度的方法与获取字符串长度的方法相同

```shell
# 取得数组元素的个数
length=${#array_name[*]}
# 取得数组单个元素的长度
lengthn=${#array_name[n]}
```

## if-else

Shell 有三种 if ... else 语句：

1.if ... fi 语句；

2.if ... else ... fi 语句；

3.if ... elif ... else ... fi 语句。

#### **if ... fi 语句**

if ... else 语句的语法：

```
if [ expression ]
then
   Statement(s) to be executed if expression is true
fi
```

 注意：expression 和方括号([ ])之间必须有空格，否则会有语法错误。 

#### **if ... else ... fi 语句**

if ... else ... fi 语句的语法

```
if [ expression ]
then
   Statement(s) to be executed if expression is true
else
   Statement(s) to be executed if expression is not true
fi
```

#### **if ... elif ... else ... fi 语句**

if ... elif ... else ... fi 语句的语法

```
if [ expression 1 ]
then
   Statement(s) to be executed if expression 1 is true
elif [ expression 2 ]
then
   Statement(s) to be executed if expression 2 is true
elif [ expression 3 ]
then
   Statement(s) to be executed if expression 3 is true
else
   Statement(s) to be executed if no expression is true
fi
```

#### **配合test的if-else语句**

if ... else 语句也经常与 test 命令结合使用，如下所示：

```
num1=1
num2=2
if test ${num1} -eq ${num2}
then
    echo 'The two numbers are equal!'
else
    echo 'The two numbers are not equal!'
fi
```

## case

 case ... esac 与其他语言中的 switch ... case 语句类似，是一种多分枝选择结构。 

####  **case语句的语法** 

```
case 值 in
模式1)
    command1
    command2
    command3
    ;;
模式2）
    command1
    command2
    command3
    ;;
*)
    command1
    command2
    command3
    ;;
esac
```

取值后面必须为关键字 in，每一模式必须以右括号结束。其中;;与其他语言中的 break 类似

下面是case的一个例子

```
#!/bin/bashecho 'Input a number between 1 to 4'
echo -e 'Your number is:\c'
read aNum
case $aNum in
    1)  echo 'You select 1'
    ;;
    2)  echo 'You select 2'
    ;;
    3)  echo 'You select 3'
    ;;
    4)  echo 'You select 4'
    ;;
    *)  echo 'You do not select a number between 1 to 4'
    ;;
esac
```

## for

与其他编程语言类似，Shell支持for循环。

for循环一般格式为：

```
for 变量 in 列表
do
    command1
    command2
    ...
    commandN
done
```

列表是一组值（数字、字符串等）组成的序列，每个值通过空格分隔。每循环一次，就将列表中的下一个值赋给变量。
in 列表是可选的，如果不用它，for 循环使用命令行的位置参数。

 

下面演示几个使用for的shell脚本例子

1.顺序输出当前列表中的数字：

```
#!/bin/bashfor loop in 1 2 3 4 5
do
    echo "The value is: $loop"
done
```

2.顺序输出字符串中的字符：

```
#!/bin/bashfor str in This is a string
do
    echo ${str}
done
```

3.显示主目录下以 .bash 开头的文件：

```
#!/bin/bash
for FILE in $HOME/.bash*
do
   echo $FILE
done
```

## while

while循环用于不断执行一系列命令，也用于从输入文件中读取数据；命令通常为测试条件。其格式为：

```shell
while command
do
   Statement(s) to be executed if command is true
done
```

 

下面是一个基本的while循环的例子

```shell
#!/bin/bash
COUNTER=0
while [ $COUNTER -lt 5 ]
do
    COUNTER=`expr $COUNTER + 1`
    echo $COUNTER
done
```

 while循环也可用于读取键盘信息 

```shell
#!/bin/bash
echo 'type <CTRL-D> to terminate'
echo -n 'enter something: '
while read FILM
do
    echo "you enter $FILM"
    echo -n 'enter something: '
done
```

## 退出循环

 Shell也使用 break 和 continue 来跳出循环。 

#### **break命令**

下面的例子中，脚本进入死循环直至用户输入数字大于5，使用break跳出这个循环。

```shell
#!/bin/bash
while :
do
    echo -n "Input a number between 1 to 5: "
    read aNum
    case $aNum in
        1|2|3|4|5) echo "Your number is $aNum!"
        ;;
        *) echo "You do not select a number between 1 to 5, game is over!"
            break
        ;;
    esac
done
```

在嵌套循环中，break 命令后面还可以跟一个整数，表示跳出第几层循环。

```shell
break n
```

下面是一个嵌套循环的例子，如果 var1 等于 2，并且 var2 等于 0，就跳出循环：

```shell
#!/bin/bash
for var1 in 1 2 3
do
   for var2 in 0 5
   do
      if [ $var1 -eq 2 -a $var2 -eq 0 ]
      then
         break 2
      else
         echo "$var1 $var2"
      fi
   done
done
```

#### **continue命令**

continue命令与break命令类似，只有一点差别，它不会跳出所有循环，仅仅跳出当前循环。

同样，continue 后面也可以跟一个数字，表示跳出第几层循环。

## 函数

 Shell 也支持函数。Shell函数必须先定义后使用。 

#### **函数的定义与调用**

Shell 函数的定义格式如下：

```
function_name () {
    list of commands
    [ return value ]
}
```

函数返回值，可以显式增加return语句；如果不加，会将最后一条命令运行结果作为返回值。

调用函数只需要给出函数名，不需要加括号。下面给出Shell函数定义和调用的一个简单例子：

```
#!/bin/bash
# Define your function here
Hello () {
   echo "hello world"
}
# Invoke your function
Hello
```

 再来看一个带有return语句的函数： 

```
#!/bin/bash
funWithReturn(){
    echo "The function is to get the sum of two numbers..."
    echo -n "Input first number: "
    read aNum
    echo -n "Input another number: "
    read anotherNum
    echo "The two numbers are $aNum and $anotherNum !"
    return $(($aNum+$anotherNum))
}
funWithReturn
# Capture value returnd by last command
ret=$?
echo "The sum of two numbers is $ret !"
```

 像删除变量一样，删除函数也可以使用 unset 命令，不过要加上 .f 选项 

#### **函数参数**

在Shell中，调用函数时可以向其传递参数。在函数体内部，通过 $n 的形式来获取参数的值

```
#!/bin/bash
funWithParam(){
    echo "The value of the first parameter is $1 !"
    echo "The value of the second parameter is $2 !"
    echo "The value of the tenth parameter is $10 !"
    echo "The value of the tenth parameter is ${10} !"
    echo "The value of the eleventh parameter is ${11} !"
    echo "The amount of the parameters is $# !"  # 参数个数
    echo "The string of the parameters is $* !"  # 传递给函数的所有参数
}
funWithParam 1 2 3 4 5 6 7 8 9 34 73
```

 注意，$10不能获取第十个参数，获取第十个参数需要${10}。当n>=10时，需要使用${n}来获取参数。 