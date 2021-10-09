## C++及其标准程序库简介

 本章重点是介绍与C++标准程序库相关的几个最重要的语言新特性 

####  **template(模板)** 

程序库中几乎所有东西都被设计成template形式。所谓templates，是针对“一个或多个尚未明确的型别”所撰写的函数或类别。

下面是一个典型例子

```c++
 template<class T>
 inline const T& max(const T& a,const T& b)
 {
      return a < b ? b : a;
 {
```

 template并非一次编译便产生出适合所有型别的代码，而是针对被使用的某个(或某组)型别进行编译。因此你必须先提供它的某个实作品才能调用。 

#### **Nontype Templates参数(非型别模板参数)**

型别(type)可作为template参数，非型别(nontype)也可以作为template参数。例如可以把标准类别bitset<>的bits数量以template参数指定。下面定义两个由bits构成的容器，分别为32个bits空间和50个bits空间。

```c++
bitset<32> flags32;
bitset<50> flags50;
```

####  **Default Template Parameters(缺省模板参数)** 

Template classes可以有缺省参数。例如以下声明，允许你使用一个或两个template参数来声明MyClass对象。

```c++
template <class T,class container = vector<T>>
class MyClass;
```

#### **关键字typename**

关键字typename被用来作为型别之前的标识符号。考虑下面例子：

```
template <class T>
class MyClass
{
       typename T::SubType *ptr;
       ...
};
```

这里，typename指出SubType是class T中定义的一个型别，因此ptr是一个指向T::SubType型别的指针。

如果没有关键字typename,SubType会被当成一个static成员，于是T::SubType *ptr会被解释为型别T内的数值SubType与ptr的乘积。

typename还可以在template声明中用来替代关键字class:

```c++
template <typename T>
class MyClass;
```

**Member Template(成员模板)**

class member function可以是个template，但这样的member template既不能是virtual，也不能有缺省参数，例如：

```c++
class MyClass
{
      ...
      template <class T>
      void f(T);
};
```

这里的MyClass::f声明了一个成员函数集，使用任何型别参数。这个特性通常用来为template classes中的成员提供自动型别转换。

如果使用类模板，其型别必须和调用端所提供的对象的型别完全吻合：

```c++
template <class T>
class MyClass
{
    private:
        T value;
    public:
        void assign(const MyClass<T>& x) {value=x.value;}
    ...  
};

void f()
{
     MyClass<double> d;
     MyClass<int> i;
     d.assign(d)            //OK
     d.assign(i)            //ERROR
}
```

而member template function可以放宽“必须精确吻合”这条规则。只要型别可被赋值，就可以被当做其函数参数。 

因为assign()参数x和*this的型别并不相同，所以不能直接存取MyClass&lt;&gt;的private成员和protected成员，因此要提供类似getValue()之类的东西。

```c++
template <class T>
class MyClass{
     private:
         T value;
     public:
         template <class X>
         void assign(const MyClass<X>& x) {value=x.getValue();}
         T getValue () const {return value;}
};

void f()
{
     MyClass<double> d;
     MyClass<int> i;
     d.assign(d);   //OK
     d.assign(i);    //OK
}
```

 因为assign()参数x和*this的型别并不相同，所以不能直接存取MyClass<>的private成员和protected成员，因此要提供类似getValue()之类的东西。 

 **template constructor**是member template的一种特殊形式。template constructor并不遮蔽implicit copy constructor。如果型别完全吻合，implicit copy constructor就会被产生出来并被调用。 

```c++
template <class T>
class MyClass{
   public:
       template <class U>
       MyClass(const MyClass<U>& x);
       ...
};

void f()
{ 
     MyClass<double> xd;
     MyClass<double> xd2(xd)  //calls built-in copy constructor
     MyClass<int> xi(xd)           //calls template constructor
}
```

**基本型别的显示初始化(Explicit Initialization)**

如果采用不含参数的、明确的constructor调用语法，基本型别会被初始化为零:

```c++
int i1;            //undefined value
int i2=int();      //initialized with zero
```

这个特性可以确保我们在撰写template程序代码时，任何型别都有一个确切的初值：

```c++
template <class T>
void f()
{
    T x=T();
    ...
}
```

 **关键字explicit** 

通过关键字explicit的作用，我们可以禁止“单参数构造函数”被用于自动型别转换。

```c++
class Stack{
    explicit Stack(int size);
    ...
};
```

如果没有explicit，这个构造函数有能力将一个int自动转换成Stack。在这种情况下，可以给Stack指派一个整数值而不会引起任何问题：

```c++
Stack s;
...
s=40  //create a new Stack for 40 elements and assigns it to s
```

而使用了explicit的构造函数则会使上述赋值操作导致编译错误。

## 通用工具

 本章讲解C++标准程序库中的通用工具。它们是由短小精干的类和函数构成。 

#### **Pairs(对组)**

class pair可以将两个值视为一个单元。STL内多处使用了pair。尤其容器map和multimap，就是使用pairs来管理key/value的成对元素。

struct pair定义与<utility>:

```c++
namespace std{
    template <class T1,class T2>
    struct pair{
        //type names for the values
        typedef T1 first_type;
        typedef T2 second_type;

        //member     
        T1 first;
        T2 second;
 
        //default constructor
        pair():first(T1()),second(T2()) {}

        //constructor for two values
        pair(cosnt T1&a,const T2& b):first(a),second(b) {}
      
        //copy constructor with implicit conversions
        template<class U,class V>
        pair(const pair<U,V>& p):first(p.first),second(p.second) {}
    };
    //comparisons
    template<class T1,class T2>
    bool operator==(const pair<T1,T2>&,const pair<T1,T2>&);
    template<class T1,class T2>
    bool operator<(const pair<T1,T2>&,const pair<T1,T2>&);
    ...
    //convenience function to create a pair
    template<class T1,class T2>
    pair<T1,T2> make_pair(const T1&,const T2&);
}
```

####  **make_pair()** 

 template函数可以让你无需写出型别，就可以生成一个pair对象: 

```c++
namespace std{
    template<class T1,class T2>
    pair<T1,T2> make_pair(const T1& x,const T2& y){
        return pair<T1,T2>(x,y);
    }
}
```

因此我们可以这样使用make_pair()

```c++
std::make_pair(42,'@')
```

而不必费力地这么写

```c++
std::pair<int,char>(42,'@')
```

#### **Class auto_ptr**

auto_ptr是一种智能型指针(smart pointer)，帮助程序员防止“被异常抛出时发生资源泄漏”。

auto_ptr是这样的一种指针：它是“它所指向的随想”的拥有者(owner)。所以，当身为对象拥有者的auto_ptr销毁时，该对象也将遭到销毁。auto_ptr要求一个对象只能有一个拥有者。

**这个智能型指针应该保证，无论在何种情形下，只要自己被摧毁，就一定连带释放其所指资源。**

####  **auto_ptr拥有权的转移** 

auto_ptr的copy构造函数和assignment操作符负责将拥有权交出去。试看下例copy构造函数的运用：

```
std::auto_ptr<ClassA> ptr1(new ClassA);
std::auto_ptr<ClassA> ptr2(ptr1);
```

一开始ptr1拥有那个new出来的对象，在第二条语句中，拥有权由ptr1转交给ptr2.ptr2就拥有了那个new出来的对象，而ptr1不再拥有它。这样，对象就只会被delete一次--在ptr2被销毁的时候。

赋值动作也差不多

```
std::auto_ptr<ClassA> prt1(new ClassA);
std::auto_ptr<ClassA> ptr2;
ptr2=ptr1;
```

如果ptr2被赋值之前正拥有另一个对象，赋值动作发生时会调用delete，将该对象删除。

拥有权的转移，使得auto_ptr产生一种特殊用法：**某个函数可以利用auto_ptr将拥有权转交给另一个函数**。这种事情可以在两种情形下出现：

1.某函数是数据的终点。如果auto_ptr以by value方式呗当做一个参数传递给某函数。此时被调用端的参数获得了这个auto_ptr的拥有权，如果函数不再将它传递出去，它指向的对象就会在函数退出时被删除:

```
void sink(std::auto_ptr<ClassA>);
```

2.某函数是数据的起点。当一个auto_ptr被返回，其拥有权便转交给调用端

```c++
std::auto_ptr<ClassA> f()
{
    std::auto_ptr<ClassA> ptr(new ClassA);
    ...
    return ptr;
}

void g()
{
    std::auto_ptr<ClassA> p;
    p=f();
}
```

**数值极限(numeric Limits)**

一般说来，数值型别的极值是一个与平台相关的特性。C++标准程序库通过template numeric_limits提供这些极值。

下面是numeric_limits<>的使用范例

```c++
#include <iostream>
#include <limits>
#include <string>
using namespace std;

int main()
{
    cout<<"max(short):"<<numeric_limits<short>::max()<<endl;
    cout<<"max(int):"<<numeric_limits<int>::max()<<endl;
}
```

 下表给出了class numeric_limits<>的所有成员 

<img src="./STL_img\2_1.png" style="zoom:75%;" />

**辅助函数**

1.挑选较小值和较大值

```
namespace std{
    template<class T>
    inline const T& min(const T& a,const T& b) {return b < a ? b : a;}
    template<class T>
    inline const T& max(const T& a,const T& b) {return a < b ? b : a;}
}
```

 上述两个函数还有另一个版本，接收一个额外的template参数作为“比较准则”： 

```c++
namespace std
{
    template<class T,class Compare>
    inline const T& min(const T& a,const T& b,Compared comp) 
    {
        return comp(b,a) ? b : a;
    }
    template<class T,class Compare>
    inline const T& min(const T& a,const T& b,Compared comp) 
    {
        return comp(a,b) ? b : a;
    }
}
```

 下面这个例子示范了如何传入特定的比较函数作为操作，以此方式来运用max(): 

```c++
#include <algorithm>
using namespace std;

bool int_ptr_less(int *a,int* b)
{
    return *a < *b;
}

int main()
{
    int x=17;
    int y=42;
    int* px=&x;
    int* py=&y;
    pmax=max(px,py,int_ptr_less);
}
```

2.两值互换

函数swap()用来交换两对象的值。

```c++
namespace std
{
    template<class T>
    inline void swap(T& a,T& b)
    {
        T tmp(a);
        a=b;
        b=tmp;
    }
}
```

## STL组件

若干精心勾画的组件共同合作，构筑起STL的基础。这些组件最关键的是容器、迭代器和算法。

下图演示了STL组件之间的合作

![](./STL_img\3_1.png)

#### **容器(Containers)**

容器类别(简称容器)用来管理一组元素。为了适应不同需求，STL提供了不同类型的容器。

![](./STL_img\3_2.png)

总的来说，容器可分为两类：

1.序列式容器Sequence containers，此乃可序群集，其中每个元素均有固定位置(取决于插入时机和地点，和元素值无关)。

  STL提供三个定义好的序列式容器：vector、deque和list。

2.关联式容器Associative containers，此乃已序群集，元素位置取决于特定的排序准则。

  STL提供了四个关联式容器：set、multiset、map和multimap。

关于容器的具体操作将在第六章演示。

#### **迭代器(Iterator)**

迭代器是一个“可遍历STL容器内全部或部分元素”的对象。一个迭代器用来指出容器中的一个特定位置。基本操作如下：

Operator *

返回当前位置上的元素值。如果该元素拥有成员，可以直接以operator->取用它们。

Operator ++

将迭代器前进至下一元素。大多数迭代器还可使用operator--退回到前一个元素。

Operator ==和Operator !=

判断两个迭代器是否指向同一位置。

Operator =

为迭代器赋值(将其所指元素的**位置**赋值进去)。

所有容器类别都提供有一些成员函数，使我们得以获得迭代器并以之遍历访问所有元素。这些函数中最重要的是：

begin()

返回一个迭代器，指向容器起始点，也就是第一个元素(如果有的话)的位置。

end()

返回一个迭代器，指向容器借宿点。结束点在最后一个元素之后。

![](./STL_img\3_3.png)

#### **迭代器分类(Iterator Categories)**

STL预先定义好的所有容器，其迭代器均属于以下两种类型：

1.双向迭代器(Bidirectional iterator)

 双向迭代器可以双向行进：以递增运算前进或以递减运算后退。list、set、multiset、map和multimap这些容器锁提供的迭代器都属此类。

2.随机存储迭代器(Random access iterator)

  随机存取迭代器不但具备双向迭代器的所有属性，还具备随机访问能力。即可以对该迭代器增加或减少一个偏移量、处理迭代器之间的距离、使用<和>之泪的操作符来比较两个迭代器。

  vector、deque和strings锁提供的迭代器都属此类。

 

#### **算法(Algorithm)**

为了处理容器内的元素，STL提供了一些标准算法，包括搜寻、排序、拷贝、重新排序、修改、数值运算等十分基本而普遍的算法。

算法并非容器类别的成员函数，而是一种搭配迭代器使用的全局函数。


#### **仿函数(Functors)**

仿函数(functor)，就是使一个类的使用看上去象一个函数。其实现就是类中实现一个operator()，这个类就有了类似函数的行为，就是一个仿函数类了。

例如我们定义一个类：

```c++
class X
{
    public:
        return-value operator()(arguments) const;
        ...  
};
```

然后就可以把这个类别的对象当做函数调用

```c++
X fo;
...
fo(arg1,arg2)  //等价于fo.operator()(arg1,arg2);
```

## 序列式容器

#### **Vector**

Vector是一个动态数组。

**1.Vector的操作函数**

 构造、拷贝和析构

```c++
vector<Elem> c           //产生一个空vector ，其中没有任何元素
vector<Elem> c1(c2)      //产生另一个同型vector的副本，(所有元素都被拷贝)
vector<Elem> c(n)        //利用元素的default构造函数生成一个大小为n的vector
vector<Elem> c(n,elem)   //产生一个大小为n的vector，每个元素值都是elem
vector<Elem> c(beg,end)  //产生一个vector,以区间[beg;end)作为元素初值
c.~vector<Elem>()        //销毁所有元素，并释放内存
```

非变动性操作

```c++
c.size()      //返回当前的元素数量
c.empty()     //判断大小是否为零。等同于size()==0
c.max_size()  //返回可容纳元素的最大数量
capacity()    //返回重新分配空间前所能容纳的元素的最大数量
reserve()     //扩大容量
c1 (op) c2    //判断两个vector大小，其中op可以为==、!=、<、>、<=和>=
```

赋值

```c++
c1=c2             //c2的全部元素赋值给c1
c.assign(n,elem)  //复制n个elem，赋值给c
c.assign(beg,end) //将区间[beg;end)内的元素赋值给c
c1.swap(c2)       //c1和c2元素交换
swap(c1,c2)       //同上，此为全局函数
```

元素存取(如果索引越界，at()会抛出out_of_range异常，而其他函数不做检查，会引发未定义行为)

```c++
c.at(idx)  //返回索引idx所标志的元素。如果idx越界，抛出out_of_range
c[idx]     //返回索引idx所标志的元素。不进行范围检查
c.front()  //返回第一个元素。不检查第一个元素是否存在
c.back()   //返回最后一个元素。不检查最后一个元素是否存在
```

迭代器相关函数

```c++
c.begin()  //返回一个随机存取迭代器，指向第一个元素
c.end()    //返回一个随机存取迭代器，指向最后元素的下一位置
c.rbegin() //返回一个逆向迭代器，指向逆向迭代的第一个元素
c.rend()   //返回一个逆向迭代器，指向逆向迭代的最后元素的下一位置
```

vector迭代器持续有效，除非发生两种情况：

1.使用者在一个较小索引位置上安插或移除元素，
2.由于容量变化而引起内存重新分配。

安插和移除元素

```c++
c.insert(pos,elem)   //在pos位置上插入一个elem副本，并返回新元素位置
c.insert(pos,n,elem) //在pos位置上插入n个elem副本。无回传值
c.insert(pos,beg,end)//在pos位置上插入区间[beg;end)内的所有元素的副本。无回传值
c.push_back(elem)    //在尾部添加一个elem副本
c.pop_back()         //移除最后一个元素(但不回传)
c.erase(pos)         //移除pos位置上的元素，返回下一元素的位置
e.resize(num)        //将元素数量改为num(如果size()变大了，多出来的新元素都是elem的副本)
c.clear()   
```

安插元素和移除元素都会使“作用点”之后的个元素的references、pointers、iterators实效。

如果安插操作引发内存重新分配，那么该容器身上的所有reference、pointers、iterators都会实效。

**2.`Vector`运用实例**

```c++
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
using namespace std;

int main()
{
    vector<string> sentence;
    sentence.push_back("hello,");
    sentence.push_back("how");
    sentence.push_back("are");
    sentence.push_back("you");
    sentence.push_back("?");
    copy(sentence.begin(),sentence.end(),ostream_iterator<string>(cout," "));
    cout<<endl;
    cout<<" max_size(): "<<sentence.max_size()<<endl;
    cout<<" size(): "<<sentence.size()<<endl;
    cout<<" capacity(): "<<sentence.capacity()<<endl;
    swap(sentence[1],sentence[3]);
    sentence.insert(find(sentence.begin(),sentence.end(),"?"),"always");
    sentence.back()="!";
    copy(sentence.begin(),sentence.end(),ostream_iterator<string>(cout," "));
    cout<<endl;
    cout<<" max_size(): "<<sentence.max_size()<<endl;
    cout<<" size(): "<<sentence.size()<<endl;
    cout<<" capacity(): "<<sentence.capacity()<<endl;
}
```

#### **Deque**

**1.`deque`的操作**

容器`deque`和`vector`非常相似。不同的是`deque`的动态数组头尾都开放，因此能在头尾连孤单进行快速安插和删除。

`deque`和`vector`的接口几乎一样，一方面不同的是`deque`具有操作头部元素的能力：

```c++
c.push_front(elem)  //在头部插入elem的一个副本
c.pop_front()       //移除头部元素(但不回传)
```

另一方面，`deque`不提供容量操作(`capacity()`和`reserve()`)。

元素的插入和删除可能导致内存重新分配，所以任何插入和删除动作都会使锁有指向`deque`元素的`pointers`、`references`和`iterators`实效。

唯一例外是在头部或尾部插入元素，动作之后，`pointers`和`refercences`依然有效。

 **2.`Deque`运用实例** 

```c++
#include <iostream>
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
using namespace std;
int main()
{
    deque<string> coll;
    coll.assign(3,string("string"));
    coll.push_back("last string");
    coll.push_front("first string");
    copy(coll.begin(),coll.end(),ostream_iterator<string>(cout,"\n"));
    cout<<endl;
    coll.pop_front();
    coll.pop_back();
    for(int i=1;i<coll.size();++i)
        coll[i]="another "+coll[i];
    coll.resize(4,"resized string");
    copy(coll.begin(),coll.end(),ostream_iterator<string>(cout,"\n"));
}
```

#### **List**

List使用一个doubly linked list(双向链表)来管理元素。

List的内部结构和vector或deque截然不同，主要有以下区别：

1.Lists不指出随机存储

2.任何位置上执行元素的安插和移除都是在常数时间内完成，因为无需移动任何其他元素。

3.安插和删除动作并不会造成指向其他元素的各个pointers、references、iterators实效。

4.Lists并为提供容量、空间分配等函数。

**1.List的操作函数**

list的生成、复制和销毁动作和所有序列式容器相同。

元素存取

list不支持随机存取，只有front()和back()能够直接存取元素

元素的安插和移除

lists提供deques的所有功能。为了移除元素，lists特别配备了remove()算法的特别版本。

```c++
c.remove(val)   //移除所有其值为val的元素
c.remove_if(op) //移除所有”造成op(elem)结果为true“的元素
```

List的特殊变动性操作

```c++
c.unique()                     //如果存在若干相邻而数值相等的元素，就移除重复元素，只留下一个
c.unique(op)                   //如果存在若干相邻元素，都使op()结果为true，则移除重复元素，只留下一个
c1.splice(pos,c2)              //将c2内所有元素转移到c1之内、迭代器pos之前
c1.splice(pos,c2,c2pos)        //将c2内c2pos所指元素移到c1内的pos所指位置上(c1和c2可相同)
c1.splice(pos,c2,c2beg,c2end)  //将c2内的[c2beg;c2end]区间内所有元素转移到c1内的pos之前(c1和c2可相同)
c.sort()                       //以operator<为准则，对所有元素排序
c1.merge(c2)                   //假设c1和c2容器都包含已序元素，将c2的全部元素转移到c1，并保证合并后的list仍为已序
c1.merge(c2,op)                //假设c1和c2容器都包含op()原则下的已序元素，将c2的全部元素转移到c1，并保证合并后list在op()原则下仍为已序
c.reverse() 
```

 **2.List运用实例** 

```c++
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
using namespace std;

void printLists(const list<int>& l1,const list<int>& l2)
{
    cout<<"list1: ";
    copy(l1.begin(),l1.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    cout<<"list2: ";
    copy(l2.begin(),l2.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}

int main()
{
    list<int> list1,list2;
    for(int i=0;i<6;++i)
    {
        list1.push_back(i);
        list2.push_front(i);
    }
    printLists(list1,list2);
    list2.splice(find(list2.begin(),list2.end(),3),list1);
    printLists(list1,list2);
    list2.splice(list2.end(),list2,list2.begin());
    printLists(list1,list2);
    list2.sort();
    list1=list2;
    list2.unique();
    printLists(list1,list2);
    list1.merge(list2);
    printLists(list1,list2);
}
```

## 关联式容器

#### **Set和Multisets**

set和multiset会根据特定的排序准则，自动将元素排序。两者不同在于multisets允许元素重复而set不允许。

![](./STL_img\4_1.png)

###### **1.set和multiset的操作函数**

生成、复制和销毁

```c++
set c              //产生一个空的set/multiset，其中不含任何元素
set c(op)          //以op为排序准则，产生一个空的set/mulitset
set c1(c2)         //产生某个set/multiset的副本，所有元素均被复制
set c(beg,end)     //以区间[beg;end)内的元素产生一个set/multiset
set c(beg,end,op)  //以op为排序准则，利用[beg;end]内的元素生成一个set/multiset
c.~set()           //销毁所有元素，释放内存
```

其中set可为下列形式：

```c++
set<Elem>         //一个set，以less<>(operator<)为排序准则
set<Elem,op>      //一个set，以op为排序准则
multiset<Elem>    //一个multiset，以less<>(operator<)为排序准则
multiset<Elem,op> //一个multiset，以op为排序准则
```

特殊的搜寻函数

```c++
count(elem)       //返回“元素值为elem”的元素个数
find(elem)        //返回"元素值为elem"的第一个元素的位置，如果找不到就返回end()
lower_bound(elem) //返回elem的第一个可安插位置，也就是“元素值>=elem”的第一个元素位置
upper_bound(elem) //返回elem的最后一个可安插位置，也就是“元素值>elem”的第一个元素位置
equal_range(elem) //返回elem可安插的第一个位置和最后一个位置，也就是“元素值==elem”的元素区间
```

下面是后面三个函数的一个例子

```c++
#include <iostream>
#include <set>
using namespace std;

int main()
{
    set<int> c;
    c.insert(1);
    c.insert(2);
    c.insert(4);
    c.insert(5);
    c.insert(6);
    cout<<"lower_bound(3): "<<*c.lower_bound(3)<<endl;
    cout<<"upper_bound(3): "<<*c.upper_bound(3)<<endl;
    cout<<"equal_range(3): "<<*c.equal_range(3).first<<" "<<*c.equal_range(3).second<<endl;
    cout<<endl;
    cout<<"lower_bound(5): "<<*c.lower_bound(5)<<endl;
    cout<<"upper_bound(5): "<<*c.upper_bound(5)<<endl;
    cout<<"equal_range(5): "<<*c.equal_range(5).first<<" "<<*c.equal_range(5).second<<endl;
}
```

赋值

set和multiset只提供所有容器都提供的基本赋值操作

这些操作函数中，赋值操作的两端容器必须具有相容型别，而“比较准则”本身可以不同。如果准则不同，准则本身也会被赋值或交换

```c++
c1=c2        //将c2中所有元素赋值给c1
c1.swap(c2)  //将c1和c2的元素交换
swap(c1,c2)  //同上，此为全局函数
```

 元素的安插和移除 

```c++
c.insert(elem)      //安插一份elem副本，返回新元素位置
c.insert(pos,elem)  //安插一份elem副本，返回新元素位置(pos只是个提示)
c.insert(beg,end)   //将区间[beg;end)内所有元素的副本安插到c(无返回值)
c.erase(elem)       //移除“与elem相等”的所有元素，返回被移除的元素个数
c.erase(pos)        //移除迭代器pos锁指位置上的元素，无返回值
c.erase(beg,end)    //移除区间[beg;end)内所有元素，无返回值
c.clear()           //移除全部元素，将整个容器清空
```

 因为set的元素不可以重复，因此安插动作可能会失败。为此set提供了如下接口来判断安插动作是否成功:

```c++
pair<iterator,bool> insert(const value_type& elem);
```

其中pair结构中second成员表示安插是否成功，first成员返回新元素的位置。

因为对迭代器操作而言，set和multiset所有元素都被视为常数，因此无法对其元素调用任何变动性算法。例如:remove()(因为remove()算法实际上是以一个参数值覆盖被移除的元素)。

如果要移除元素，只能使用它们所提供的成员函数。

###### **2.set和multiset运用实例** 

```c++
#include <iostream>
#include <set>
#include <iterator>
using namespace std;

int main()
{
    typedef set<int,greater<int> > IntSet;
    IntSet coll1;
    coll1.insert(4);
    coll1.insert(3);
    coll1.insert(5);
    coll1.insert(1);
    coll1.insert(6);
    coll1.insert(2);
    coll1.insert(5);
    IntSet::iterator pos;
    for(pos=coll1.begin();pos!=coll1.end();++pos)
        cout<<*pos<<' ';
    cout<<endl;
    pair<IntSet::iterator,bool> status=coll1.insert(4);
    if(status.second)
        cout<<"4 inserted as element "<<distance(coll1.begin(),status.first)+1<<endl;
    else
        cout<<"4 already exists"<<endl;
    set<int> coll2(coll1.begin(),coll1.end());
    copy(coll2.begin(),coll2.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    coll2.erase(coll2.begin(),coll2.find(3));
    int num;
    num=coll2.erase(5);
    cout<<num<<" element(s) removed"<<endl;
    copy(coll2.begin(),coll2.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}
```



 

#### **Map和Multimap**

Map和Multimap将key/value当做元素，进行管理，他们可根据key的排序准则自动将元素排序。multimap允许重复元素(key值)，maps不允许。



![](./STL_img\4_2.png)

###### **1.map和multimap的操作函数**

生成、复制和销毁(跟set一样)

```
map c              //产生一个空的map/multimap，其中不含任何元素
map c(op)          //以op为排序准则，产生一个空的map/mulitmap
map c1(c2)         //产生某个map/multimap的副本，所有元素均被复制
map c(beg,end)     //以区间[beg;end)内的元素产生一个map/multimap
map c(beg,end,op)  //以op为排序准则，利用[beg;end)内的元素生成一个map/multimap
c.~map()           //销毁所有元素，释放内存
```

其中，map可为下列形式：

```
map<key,Elem>          //一个map，以less<>(operator<)为排序准则
map<key,Elem,op>       //一个map，以op为排序准则
multimap<key,Elem>     //一个multimap，以less<>(operator<)为排序准则
multimap<key,Elem,op>  //一个multimap，以op为排序准则
```

map和multimap也有类似于set和multiset的特殊搜寻操作函数

```
count(key)       //返回“键值为key”的元素个数
find(key)        //返回"键值为key"的第一个元素的位置，如果找不到就返回end()
lower_bound(key) //返回"键值为key"的元素的第一个可安插位置，也就是“键值>=key”的第一个元素位置
upper_bound(key) //返回“键值为key”的元素的最后一个可安插位置，也就是“键值>key”的第一个元素位置
equal_range(key) //返回“键值为key”的元素可安插的第一个位置和最后一个位置，也就是“键值==key”的元素区间
```

与set和multiset一样，在map和multimap中(实际型别是pair<const key,T>)，所有元素的key都被视为常数，因此不能对其调用任何变动性算法，也不能直接修改key(但是可以修改value)。

map和multimap的元素安插和移除函数跟set和multiset一样。不同的是map跟multimap安插和删除的是key/value值。

将map视为关联式数组

map提供下标操作符，支持元素的直接存取

```
m[key]  //返回一个reference，指向键值为key的元素。如果该元素尚未存在，就安插该元素
```

###### **2.map和multimap的运用实例**

```c
#include <iostream>
#include <map>
#include <string>
using namespace std;

int main()
{
    typedef map<string,float> StringFloatMap;
    StringFloatMap stocks;
    stocks["BASF"]=369.50;
    stocks["VM"]=413.50;
    stocks["Daimler"]=819.00;
    stocks["BMW"]=834.00;
    stocks["Siemens"]=842.20;
    StringFloatMap::iterator pos;
    for(pos=stocks.begin();pos!=stocks.end();++pos)
    {
        cout<<"stock: "<<pos->first<<"\t"
            <<"price: "<<pos->second<<endl;
    }
    cout<<endl;
    for(pos=stocks.begin();pos!=stocks.end();++pos)
        pos->second*=2;
    for(pos=stocks.begin();pos!=stocks.end();++pos)
    {
        cout<<"stock: "<<pos->first<<"\t"
            <<"price: "<<pos->second<<endl;
    }
    cout<<endl;
    stocks["Volkswagen"]=stocks["VM"];
    stocks.erase("VM");
    for(pos=stocks.begin();pos!=stocks.end();++pos)
    {
        cout<<"stock: "<<pos->first<<"\t"
            <<"price: "<<pos->second<<endl;
    }
    cout<<endl;
}
```

 下面例子展示如何将multimap当做一个字典来使用 

```c
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
using namespace std;

int main()
{
    typedef multimap<string,string> StrStrMap;
    StrStrMap dict;
    dict.insert(make_pair("day","Tat"));
    dict.insert(make_pair("strange","fremd"));
    dict.insert(make_pair("car","Auto"));
    dict.insert(make_pair("smart","elegant"));
    dict.insert(make_pair("trait","Merkmak"));
    dict.insert(make_pair("strange","seltsam"));
    dict.insert(make_pair("smart","raffiniert"));
    dict.insert(make_pair("smart","klug"));
    dict.insert(make_pair("clever","raffiniert"));
    StrStrMap::iterator pos;
    cout.setf(ios::left,ios::adjustfield);
    cout<<' '<<setw(10)<<"english "<<"german "<<endl;
    cout<<setfill('-')<<setw(20)<<""<<setfill(' ')<<endl;
    for(pos=dict.begin();pos!=dict.end();++pos)
    {
        cout<<' '<<setw(10)<<pos->first.c_str()
            <<pos->second<<endl;
    }
    cout<<endl;
    string word("smart");
    cout<<word<<":"<<endl;
    for(pos=dict.lower_bound(word);pos!=dict.upper_bound(word);++pos)
        cout<<"     "<<pos->second<<endl;
    word=("raffiniert");
    cout<<word<<":"<<endl;
    for(pos=dict.begin();pos!=dict.end();++pos)
    {
        if(pos->second==word)
            cout<<"     "<<pos->first<<endl;
    }
}
```

## **迭代器类型**

迭代器是一种“能够遍历某个序列内的所有元素”的对象。它可以透过与一般指针一致的接口来完成自己的工作。

不同的迭代器具有不同的”能力“(行进和存取能力)

![](./STL_img\6_1.png)

#### **Input迭代器**

Input迭代器只能一次一个向前读取元素，按此顺序一个个传回元素值。

几乎所有迭代器都具备Input迭代器的能力，而且通常更强。纯粹Input迭代器的一个典型例子就是“从标准输入装置读取数据”的迭代器。

下表列出了Input迭代器的各种操作行为

![](./STL_img\6_2.png)

#### **Output迭代器**

Output迭代器和Input迭代器相反，其作用是将元素值一个个写入。

下表列出Output迭代器的各种操作行为

![](./STL_img\6_3.png)

#### **Forward(前向)迭代器**

Forward迭代器是Input迭代器和Output迭代器的结合，具有Input迭代器的全部功能和Output迭代器的大部分功能，下表总结了Forward迭代器的所有操作

![](./STL_img\6_4.png)

#### **Bidirectional(双向)迭代器**

Bidirectional迭代器在Forward迭代器的基础上增加了回头遍历的能力。换言之，它支持递减操作符，用以进行一步一步的后退操作

![](./STL_img\6_5.png)

#### **Random Access(随机存取)迭代器**

Random Access迭代器在Bidirectional迭代器基础之上再增加随机存取能力。因此它必须提供“迭代器算术运算”(加减某个偏移量、能处理距离问题)。

下面列出Random Access迭代器的新增操作

![](./STL_img\6_6.png)

## 迭代器相关辅助函数

#### **advance()可令迭代器前进**

```c++
#include <iterator>
void advance(InputIterator& pos,Dist n);
```

面对Random Access(随机存取)迭代器，该函数只是简单的调用pos+=n。而对于其他任何类型的迭代器则调用++pos(--pos)n次。

#### **distance()可以处理迭代器之间的距离**

```c++
#include <iterator>
Dist distance(InputIterator pos1,InputIterator pos2);
```

面对Random Access迭代器，该函数仅仅传回pos2-pos1。而对于其他迭代器，则会不断递增pos1，知道抵达pos2为止，然后传回递增次数。

#### **iter_swap()可交换两个迭代器所指内容**

```c++
#include <iterator>
void iter_swap(ForwardIterator pos1,ForwardIterator pos2);
```

迭代器型别不必相同，但所指的两个值必须可以相互赋值。

#### **Vector迭代器的递增**

考虑下面例子：

```c++
std::vector<int> coll;
...
if(coll.size()>1){
    sort(++coll.begin(),coll.end())
}
```

通常编译sort()会失败。因为vector一般实作为一般指针，而C++不允许修改任何基本型别(包括指针)的暂时值。而对于struct和class则允许。

如果vector实作为一般指针，则会编译失败，如果vector实作为class，则可以通过编译。

## 迭代器配接器

#### **Reverse(逆向)迭代器**

Reverse迭代器是一种配接器。 重新定义递增运算和递减运算。使其行为正好倒置。

如果你使用这类迭代器，算法将以逆向次序处理元素。所有标准容器都允许使用Reverse迭代器来遍历元素。下面是个例子：

```
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

void print(int elem)
{
    cout<<elem<<' ';
}

int main()
{
    list<int> coll;
    for(int i=1;i<=9;++i)
        coll.push_back(i);
    for_each(coll.begin(),coll.end(),print);
    cout<<endl;
    for_each(coll.rbegin(),coll.rend(),print);
    cout<<endl;
}
```

#### **Insert(安插型)迭代器**

通过这种迭代器，算法可以执行安插行为而非覆盖行为。它提供以下操作

![](./STL_img\8_1.png)

C++标准程序库提供三种Insert迭代器：back Inserters、front inserters、general Inserters。它们之间的区别在于插入位置。

事实上它们各自调用所属容器中不同的成员函数。

![](./STL_img\8_2.png)

显然，容器本身必须支持Insert迭代器所调用的函数，否则该种Insert迭代器就不可用。

下面展示了back inserters的用法

```
#include <iostream>
#include <vector>
#include <algorithm>
#include "print.cpp"
using namespace std;

int main()
{
    vector<int> coll;
    back_insert_iterator<vector<int> > iter(coll);
    *iter=1;
    iter++;
    *iter=2;
    iter++;
    *iter=3;
    PRINT_ELEMENTS(coll);
    back_inserter(coll)=44;
    back_inserter(coll)=55;
    PRINT_ELEMENTS(coll);
    coll.reserve(2*coll.size());
    copy(coll.begin(),coll.end(),back_inserter(coll));
    PRINT_ELEMENTS(coll);
}
```

#### **Stream(流)迭代器**

我们可以通过Stream迭代器把stream当成算法的原点和起点。

一个istream迭代器可用来从input stream中读取元素，而一个ostream迭代器可以用来对output stream写入元素。

###### **1.Ostream迭代器**

ostream迭代器可以将被赋予的值写入output stream中。如此一来算法就可以使用一般的迭代器接口直接对stream执行涂写动作。下面列出ostream迭代器的各个操作函数。

![](./STL_img\8_3.png)

 下面演示ostream迭代器的用法 

```
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;

int main()
{
    ostream_iterator<int> intWriter(cout,"\n");
    *intWriter=42;
    intWriter++;
    *intWriter=77;
    intWriter++;
    *intWriter=-5;
    vector<int> coll;
    for(int i=1;i<=9;++i)
        coll.push_back(i);
    copy(coll.begin(),coll.end(),ostream_iterator<int>(cout));
    cout<<endl;
    copy(coll.begin(),coll.end(),ostream_iterator<int>(cout," < "));
    cout<<endl;
}
```

###### 2.istream迭代器

istream迭代器用来从input stream读取元素。透过istream迭代器，算法可以从stream中直接读取数据。

下面是istream迭代器的各项操作函数

![](./STL_img\8_4.png)

 下面展示istream迭代器的各项操作 

```
#include <iostream>
#include <iterator>
using namespace std;

int main()
{
    istream_iterator<int> intReader(cin);
    istream_iterator<int> intReaderEOF;
    while(intReader!=intReaderEOF)
    {
        cout<<"once: "<<*intReader<<endl;
        cout<<"once again: "<<*intReader<<endl;
        ++intReader;
    }
}
```

## 仿函数

仿函数(functor)，就是使一个类的使用看上去象一个函数。其实现就是类中实现一个operator()，这个类就有了类似函数的行为，就是一个仿函数类了。

例如我们定义一个类：

```
class X{
    public:
        return-value operator()(arguments) const;
        ...  
};
```

然后就可以把这个类别的对象当做函数调用

```
X fo;
...
fo(arg1,arg2)  //等价于fo.operator()(arg1,arg2);
```

 显然，这种定义形式更为复杂，却又三大妙处：

1.仿函数比一般函数更灵巧，因为它可以拥有状态。

2.每个仿函数都有其型别。因此可以将仿函数的型别当做template参数传递。

3.执行速度上，仿函数通常比函数指针更快。

####  **仿函数可当做排序准则** 

```
#include <iostream>
#include <string>
#include <set>
#include <algorithm>
using namespace std;

class Person{
    public:
        string firstname() const;
        string lastname() const;
        ...
};

class PersonSortCriterion{
    public:
        bool operator()(const Person&p1,const Person& p2) const {
            return p1.lastname()<p2.lastname()||
                    (!(p2.lastname()<p1.lastname())&&
                    p1.firstname()<p2.firstname());
        }
};

int main()
{
    typedef set<Person,PersonSortCriterion> PersonSet;
    PersonSet coll;
    PersonSet::iterator pos;
    for(pos=coll.begin();pos!=coll.end();++pos){
        ...
    }
    ...
}
```

 这里的coll适用了特殊排序准则PersonSortCritersion，而它是一个仿函数类别。所以可以当做set的template参数，而一般函数则无法做到这一点。 

#### **拥有内部状态的仿函数**

下面例子展示仿函数如何模拟函数在同一时刻下拥有多个状态

```c++
#include <iostream>
#include <list>
#include <algorithm>
#include "print.cpp"
using namespace std;

class IntSequence
{
private:
    int value;
public:
    IntSequence(int initialValue):value(initialValue){}
    int operator() ()
    {
        return value++;
    }
};

int main()
{
    list<int> coll;
    generate_n(back_inserter(coll),9,IntSequence(1));
    PRINT_ELEMENTS(coll);
    generate(++coll.begin(),--coll.end(),IntSequence(42));
    PRINT_ELEMENTS(coll);
}
```

#### **for_each()的返回值**

使用for_each()可以返回其仿函数。下面将演示这一点

```
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class MeanValue
{
private:
    long num;
    long sum;
public:
    MeanValue():num(0),sum(0){}
    void operator() (int elem)
    {
        num++;
        sum+=elem;
    }
    double value()
    {
        return static_cast<double>(sum)/static_cast<double>(num);
    }
};

int main()
{
    vector<int> coll;
    for(int i=1;i<=8;++i)
    {
        coll.push_back(i);
    }
    MeanValue mv=for_each(coll.begin(),coll.end(),MeanValue());
    cout<<"mean value:"<<mv.value()<<endl;
}
```

#### **预定义的仿函数**

C++标准程序库提供了许多预定义的仿函数。下面列出了所有这些仿函数

![](./STL_img\8_5.png)

 对对象排序或进行比较时，一般都以less<>为预设排序准则。要使用这些仿函数，必须包含头文件<functional>。 

#### **函数配接器(Function Adapters)**

所谓“函数配接器”是指能够将仿函数和另一个仿函数(或某个值，或一般函数)结合起来的仿函数。函数配接器也声明与<functional>中。

例如以下语句：

```c++
find_if(coll.begin(),coll.end(),bind2nd(greater<int>()),42)
```

其中bind2nd是将一个二元仿函数(greater<>)转换成一元仿函数。它通常将第二参数传给“由第一参数指出”的二元仿函数，作为后者的第二参数。

下面列出了预定义的函数配接器

![](./STL_img\8_6.png)

## 算法概述

#### **算法头文件**

要运用C++标准程序库的算法，首先必须包含头文件<algorithm>

使用STL算法时，经常需要用到仿函数以及函数配接器。它们定义域<functional>头文件中。

 

 

#### **算法的分类**

可以按以下分类方式描述各个STL算法：

非变动性算法(nonmodifying algorithms)

变动性算法(modifying algorithms)

移除性算法(removing algorithms)

变序性算法(mutating algorithms)

排序算法(sorting algorithms)

已序区间算法(sorted range algorithms)

数值算法(numeric algorighms)

 

#### **非变动性算法**

非变动性算法既不改变元素次序，也不改变元素值。它们透过input迭代器和forward迭代器完成工作，因此可作用于所有标准容器身上。

下表展示C++彼岸准程序库涵盖的所有非变动性算法:

![](./STL_img\10_1.png)

最重要的算法之一便是for_each()，它将调用者提供的操作施加于每一个元素身上。例如可以用for_each()来打印区间内的每个元素。

for_each()也可以用来变动元素(如果传给它的操作行为会变动元素值的话)，所以也可以说是变动性算法。

#### **变动性算法**

变动性算法，要不直接改变元素值，要不就是在复制在另一区间的过程中改变元素值(原区间不会发生变化)。

关联式容器的元素被视为常数，在变动元素的时候可能会违反容器的排序准则。因此变动性算法的目标区间不能是关联式容器。

下标列出了C++标准程序库涵盖的变动性算法。

![](./STL_img\10_2.png)

最基本的变动性算法时for_each()和transform()。两者都可以变动序列中的所有元素值。他们的行为有以下不同点：

for_each()接受一项操作，该操作可变动其参数。因此该参数必须以by reference方式传递。例如：

```c++
void square(int& elem)        //call by reference
{
    elem = elem * elem;
}
...
for_each(coll.begin(),coll.end(),   //range         square);                   //operation
```

`transform()`运用某项操作，该操作返回被改动后的参数。所以可以被用来将结果复制给原元素，例如： 

```c++
int square(int elem)   //call by value
{
    return elem * elem;
}
...  
transform(coll.begin(),coll.end()   //source range
                coll.begin(),                 //destination range
                square);                      //operation
```

#### **移除性算法**

移除性算法是一种特殊的变动性算法。他们可以移除某区间内的元素，也可以在赋值过程中执行移除动作。

与变动性算法类似，移除性算法的目标区间也不能使关联式容器。下表列出C++标准程序库涵盖的所有移除性算法：

![](./STL_img\10_3.png)

移除算法只是在逻辑上移除元素。使用的手段是：将不需将移除的元素往前覆盖应被移除的元素。

因此它并不改变操作区间内的元素个数，而是返回逻辑上的新终点位置。

 

**变序性算法**

所谓变序性算法，是透过元素值的赋值和交换，改变元素顺序(但不改变元素值)。和变动性算法一样，变序性算法也不能以关联式容易作为目标。

下表列出了C++标准程序库涵盖的所有变序性算法：

![](./STL_img\10_4.png)

**排序算法**

排序算法是一种特殊的变序性算法。下表列出C++标准程序库中所有的排序算法：

![](./STL_img\10_5.png)

**已序区间算法**

所谓已序区间算法，是指其所作用的区间在某种排序准则下已序。下表列出C++标准程序库中涵盖的所有已序区间算法。

![](./STL_img\10_6.png)

**数值算法**

数值算法以不同方式组合数值元素。下表列出C++标准程序涵盖的所有数值算法：

![](./STL_img\10_7.png)

## 非变动性算法

 本节跟以后几节将对所有STL算法逐一详细讨论。为了简化这些例子，我们使用了一些辅助函数，分别用于对容器进行输出跟插入操作。 

```c++
#ifndef ALGOSTUFF_HPP
#define ALGOSTUFF_HPP
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>

template <class T>
inline void PRINT_ELEMENTS(const T& coll,const char* optcstr="")
{
    typename T::const_iterator pos;
    std::cout<<optcstr;
    for(pos=coll.begin();pos!=coll.end();++pos)
        std::cout<<*pos<<" ";
    std::cout<<std::endl;
}

template <class T>
inline void INSERT_ELEMENTS(T& coll,int first,int last)
{
    for(int i=first;i<=last;++i)
        coll.insert(coll.end(),i);
}
#endif
```

**for_each()算法**

for_each()算法非常灵活，它可以以不同的方式存取、处理、修改每一个元素

UnaryProc

for_each(InputIterator beg,InputIterator end,UnaryProc op);

 

1.对与区间[beg,end)中的每一个元素调用:op(elem)

2.返回op(已在算法内部被变动过)的一个副本

3.op的任何返回值都会被忽略

 下面例子将print()传给for_each()，使得for_each()对每一个元素调用print()，从而打印所有元素： 

```c++
#include "algostuff.hpp"
using namespace std;

void print(int elem)
{
    cout<<elem<<' ';
}

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    for_each(coll.begin(),coll.end(),print);
    cout<<endl;
}
```

 下面例子展示如何利用for_each()的返回值，利用这个特性可以求出平均值： 

```c++
#include "algostuff.hpp"
using namespace std;

class MeanValue
{
private:
    long num;
    long sum;
public:
    MeanValue():num(0),sum(0){}
    void operator() (int elem)
    {
        num++;
        sum+=elem;
    }
    operator double()
    {
        return static_cast<double>(sum)/static_cast<double>(num);
    }
};

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,8);
    double mv=for_each(coll.begin(),coll.end(),MeanValue());
    cout<<"mean value: "<<mv<<endl;
}
```

#### **1.元素计数**

difference_type

**count**(InputIterator beg,InputIterator end,const T& value)

difference_type

**count_if**(InputIterator beg,InputIterator end,UnarPredicate op)

1.第一种形式会计算区间[beg,end)中元素值等于value的元素个数

2.第二种形式会计算区间[beg,end)中令以下一元判断式结果为true的元素个数:op(elem)

3.返回值型别difference_type是表现迭代器区间的型别

以下范例根据不同的准则对元素进行计数：

```c++
#include "algostuff.hpp"
using namespace std;

bool isEven(int elem)
{
    return elem%2==0;
}

int main()
{
    vector<int> coll;
    int num;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    num=count(coll.begin(),coll.end(),4);
    cout<<"number of elements equal to 4: "<<num<<endl;
    num=count_if(coll.begin(),coll.end(),isEven);
    cout<<"number of elements with even value: "<<num<<endl;
    num=count_if(coll.begin(),coll.end(),bind2nd(greater<int>(),4));
    cout<<"number of elements greater than 4: "<<num<<endl;
}
```

#### **2.最小值和最大值**

InputIterator

**min_element**(InputIterator beg,InputIterator end)

InputIterator

**min_element**(InputIterator beg,InputIterator end,CompFunc op)

InputIterator

**max_element**(InputIterator beg,InputIterator end)

InputIterator

**max_element**(InputIterator beg,InputIterator end,CompFunc op)

1.所有这些算法都返回区间[beg,end)中最小或最大元素的位置。

2.上述无op参数的版本，以operator<进行元素比较

3.op用来比较两个元素：op(elem1,elem2)。如果第一个元素小于第二个元素，应当返回true。

4.如果存在多个最小值或最大值，上述算法返回找到的第一个最小或最大值。

 

以下程序打印coll之中的最小元素和最大元素，并通过absLess()打印绝对值最大跟最小的元素：

```c++
#include <cstdlib>
#include "algostuff.hpp"
using namespace std;

bool absLess(int elem1,int elem2)
{
    return abs(elem1)<abs(elem2);
}

int main()
{
    deque<int> coll;
    INSERT_ELEMENTS(coll,2,8);
    INSERT_ELEMENTS(coll,-3,5);
    PRINT_ELEMENTS(coll);
    cout<<"minimum: "
        <<*min_element(coll.begin(),coll.end())
        <<endl;
    cout<<"maximum: "
        <<*max_element(coll.begin(),coll.end())
        <<endl;
    cout<<"minimum of absolute values: "
        <<*min_element(coll.begin(),coll.end(),absLess)
        <<endl;
    cout<<"maximum of absolute values: "
        <<*max_element(coll.begin(),coll.end(),absLess)
        <<endl;
}
```

#### **3.搜寻元素**

###### **搜索第一个元素**

InputIterator

**find**(InputIterator beg,InputIterator end,const T& value)

InputIterator

**find_if**(InputIterator beg,InputIterator end,UnaryPredicate op)

1.第一形式返回区间[beg,end)中第一个“元素值等于value”的元素位置

2.第二形式返回区间[beg,end)中令以下一元判断式结果为true的第一个元素位置:op(elem)

3.如果没有找到匹配元素，两种形式都返回end。

下面这个例子展示如何运用find()搜寻一个子区间：以元素值为4的第一个元素开始，以元素值为4的第二个元素结束

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    list<int>::iterator pos1;
    pos1=find(coll.begin(),coll.end(),4);
    list<int>::iterator pos2;
    if(pos1!=coll.end())
        pos2=find(++pos1,coll.end(),4);
    if(pos1!=coll.end()&&pos2!=coll.end())
        copy(--pos1,++pos2,ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

 下面这个程序展示find_if()的用法 

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    vector<int>::iterator pos;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    pos=find_if(coll.begin(),coll.end(),bind2nd(greater<int>(),3));
    cout<<"the "
        <<distance(coll.begin(),pos)+1
        <<". element is the first greater than 3"<<endl;
    pos=find_if(coll.begin(),coll.end(),not1(bind2nd(modulus<int>(),3)));
    cout<<"the "
        <<distance(coll.begin(),pos)+1
        <<". element is the first divisible by 3"<<endl;
}
```

###### **搜索前n个连续匹配值**

InputIterator

**search_n**(InputIterator beg,InputIterator end,

​       Size count,const T& value)

InputIterator

**search_n**(InputIterator beg,InputIterator end,

​       Size count,const T& value,BinaryPredicate op)

1.第一形式返回区间[beg,end)中第一组“连续count个元素全等于value”的元素位置

2.第二形式返回区间[beg,end)中第一组“连续count个元素造成以下二元判断式结果为true”的元素位置：op(elem,value)

3.如果没有找到匹配元素，两种形式都返回end。

下面这个例子演示了这两个函数的用法

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll);
    deque<int>::iterator pos;
    pos=search_n(coll.begin(),coll.end(),4,3);
    if(pos!=coll.end())
        cout<<"four consecutive elements with value 3 "
            <<"start with "<<distance(coll.begin(),pos)+1
            <<". element"<<endl;
    else
        cout<<"no four consecutive elements with value 3 found"<<endl;
    pos=search_n(coll.begin(),coll.end(),4,3,greater<int>());
    if(pos!=coll.end())
        cout<<"four consecutive elements with value > 3 "
            <<"start with "<<distance(coll.begin(),pos)+1
            <<". element"<<endl;
    else
        cout<<"no four consecutive elements with value > 3 found"<<endl;

}
```

###### **搜寻第一个子区间**

ForwardIterator1

**search**(ForwardIterator1 beg,ForwardIterator1 end,

​      ForwardIterator2 searchBeg,ForwardIterator2 searchEnd)

ForwardIterator1

**search**(ForwardIterator1 beg,ForwardIterator2 end,

​      ForwardIterator2 searchBeg,ForwardIterator2 end,

​      BinaryPredicate op)

1.两种形式都返回区间[beg,end)内和区间[searchBeg,searchEnd)完全吻合的第一个子区间的第一个元素位置

2.第一种形式中，子区间的元素必须完全等于[searchBeg,searchEnd)的元素

3.第二种形式中，子区间的元素和[searchBeg,searchEnd)的对应元素必须造成以下二元判断式的结果为true：op(elem,searchElem)

4.如果没有找到符合条件的子区间，两种形式都返回end

下面这个例子展示如何在另一个序列中搜寻一个子序列

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll;
    list<int> subcoll;
    INSERT_ELEMENTS(coll,1,7);
    INSERT_ELEMENTS(coll,1,7);
    INSERT_ELEMENTS(subcoll,3,6);
    PRINT_ELEMENTS(coll,"coll: ");
    PRINT_ELEMENTS(subcoll,"subcoll: ");
    deque<int>::iterator pos;
    pos=search(coll.begin(),coll.end(),subcoll.begin(),subcoll.end());
    while(pos!=coll.end())
    {
        cout<<"subcoll found starting with element "
            <<distance(coll.begin(),pos)+1
            <<endl;
        ++pos;
        pos=search(pos,coll.end(),subcoll.begin(),subcoll.end());
    }
}
```

 下面这个例子展示了search的第二种形式的用法 

```c++
#include "algostuff.hpp"
using namespace std;

bool checkEven(int elem,bool even)
{
    if(even)
    {
        return elem%2==0;
    }
    else
    {
        return elem%2==1;
    }
}

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    bool checkEvenArgs[3]={true,false,true};
    vector<int>::iterator pos;
    pos=search(coll.begin(),coll.end(),checkEvenArgs,checkEvenArgs+3,checkEven);
    while(pos!=coll.end())
    {
        cout<<"subrange found starting with element "
            <<distance(coll.begin(),pos)+1
            <<endl;
        pos=search(++pos,coll.end(),checkEvenArgs,checkEvenArgs+3,checkEven);
    }
}
```

###### **搜寻最后一个子区间**

ForwardIterator

**find_end**(ForwardIterator beg,ForwardIterator end,

​       ForwardIterator searchBeg,ForwardIterator searchEnd)

ForwardIterator

**find_end**(ForwardIterator beg,ForwardIterator end,

​       ForwardIterator searchBeg,ForwardIterator searchEnd,

​       BinaryPredicate op)

find_end()与search()用法大同小异。下面例子展示如何在一个序列中搜寻“与某序列相等”的最后一个子序列

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll;
    list<int> subcoll;
    INSERT_ELEMENTS(coll,1,7);
    INSERT_ELEMENTS(coll,1,7);
    INSERT_ELEMENTS(subcoll,3,6);
    PRINT_ELEMENTS(coll,"coll: ");
    PRINT_ELEMENTS(subcoll,"subcoll: ");
    deque<int>::iterator pos;
    pos=find_end(coll.begin(),coll.end(),subcoll.begin(),subcoll.end());
    deque<int>::iterator end(coll.end());
    while(pos!=end)
    {
        cout<<"subcoll found starting with element "
            <<distance(coll.begin(),pos)+1
            <<endl;
        end=pos;
        pos=find_end(coll.begin(),end,subcoll.begin(),subcoll.end());
    }
}
```

###### **搜寻某些元素的第一次出现地点**

FordwardIterator

**find_first_of**(ForwardIterator1 beg,ForwardIterator1 end,

​           ForwardIterator 2 searchBeg,ForwardIterator2 searchEnd)

FordwardIterator

**find_first_of**(ForwardIterator1 beg,ForwardIterator1 end,

​           ForwardIterator 2 searchBeg,ForwardIterator2 searchEnd,

​           BinaryPredicate op)

1.第一形式返回第一个“既在区间[beg,end)中出现，也在区间[searchBeg,searchEnd)中出现”的元素的位置

2.第二形式返回区间[beg,end)中第一个这样的元素：它和区间[searchBeg,searchEnd)内某一个元素进行以下动作的结果是true: op(elem,searchElem)

3.如果没有找到吻合元素，两种形式都返回end

 

下面这个例子展示find_first_of()的用法：

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    list<int> searchcoll;
    INSERT_ELEMENTS(coll,1,11);
    INSERT_ELEMENTS(searchcoll,3,5);
    PRINT_ELEMENTS(coll,"coll: ");
    PRINT_ELEMENTS(searchcoll,"searchcoll: ");
    vector<int>::iterator pos;
    pos=find_first_of(coll.begin(),coll.end(),searchcoll.begin(),searchcoll.end());
    cout<<"first elements of searchcoll in coll is element "<<distance(coll.begin(),pos)+1<<endl;
    vector<int>::reverse_iterator rpos;
    rpos=find_first_of(coll.rbegin(),coll.rend(),searchcoll.rbegin(),searchcoll.rend());
    cout<<"last element of searchcoll in coll is element "<<distance(coll.begin(),rpos.base())<<endl;
}
```

###### **搜寻两个连续且相等的元素**

InputIterator

**adjacent_find**(InputIteator beg,InputIterator end)

InputIterator

**adjacent_find**(InputIteator beg,InputIterator end,

​           BinaryPredicate op)

1.第一形式返回区间[beg,end)中第一对“连续两个相等元素”之中第一个元素位置

2.第二形式返回区间[beg,end)中第一对“连续两个元素均使以下二元判断式的结果为true”的其中第一元素位置：op(elem,nextelem)

3.如果没有找到吻合元素，两者都返回end

 

下面展示adjacent_find()两种形式的用法：

```c++
#include "algostuff.hpp"
using namespace std;

bool doubled(int elem1,int elem2)
{
    return elem1*2==elem2;
}

int main()
{
    vector<int> coll;
    coll.push_back(1);
    coll.push_back(3);
    coll.push_back(2);
    coll.push_back(4);
    coll.push_back(5);
    coll.push_back(5);
    coll.push_back(0);
    PRINT_ELEMENTS(coll,"coll: ");
    vector<int>::iterator pos;
    pos=adjacent_find(coll.begin(),coll.end());
    if(pos!=coll.end())
        cout<<"first two elements with equal value have position "
            <<distance(coll.begin(),pos)+1
            <<endl;
    pos=adjacent_find(coll.begin(),coll.end(),doubled);
    if(pos!=coll.end())
        cout<<"first two elements with second value twice the first have pos. "
            <<distance(coll.begin(),pos)+1
            <<endl;
}
```

#### **区间的比较**

###### **检验相等性**

bool

**equal**(InputIterator1 beg,InputIterator1 end,

​     InputIterator2 cmpBeg)

bool

**equal**(InputIterator1 beg,InputIterator1 end,

​     InputIterator2 cmpBeg,BinaryPredicate op)

1.第一形式判断区间[beg,end)内的元素是否都和“以cmpBeg开头的区间”内的元素相等

2.第二形式判断区间[beg,end)内的元素和“以cmpBeg开头的区间内的对应元素“是否都能使以下二元判断式为true： op(elem,cmpElem)

3.调用者必须确保”以cmpBeg开头的区间“内含足够元素

 

下面是equal()两种形式的用法

```c++
#include "algostuff.hpp"
using namespace std;

bool bothEvenOrOdd(int elem1,int elem2)
{
    return elem1%2==elem2%2;
}

int main()
{
    vector<int> coll1;
    list<int> coll2;
    INSERT_ELEMENTS(coll1,1,7);
    INSERT_ELEMENTS(coll2,3,9);
    PRINT_ELEMENTS(coll1,"coll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    if(equal(coll1.begin(),coll1.end(),coll2.begin()))
        cout<<"coll1==coll2"<<endl;
    else
        cout<<"coll1!=coll2"<<endl;
    if(equal(coll1.begin(),coll1.end(),coll2.begin(),bothEvenOrOdd))
        cout<<"even and odd elements correspond"<<endl;
    else
        cout<<"even and odd elements do not correspond"<<endl;
}
```

###### **搜寻第一处不同点**

pair<InputIterator1,InputIterator2>

**mismatch**(InputIterator1 beg,InputIterator1 end,

​        InputIterator2 cmpBeg)

pair<InputIterator1,InputIterator2>

**mismatch**(InputIterator1 beg,InputIterator1 end,

​        InputIterator2 cmpBeg,

​        BinaryPredicate op)

1.第一形式返回区间[beg,end)和”以cmpBeg开头的区间“之中第一组两两相异的对应元素

2.第二形式返回区间[beg,end)和”以cmpBeg开头的区间“之中第一组”使以下二元判断式获得false“的对应元素: op(elem,cmpElem)

3.如果没有找到相异点，就返回一个pair，以end和第二序列的对应元素组成。

 

下面例子展示mismatch()两种形式的用法

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll1;
    list<int> coll2;
    INSERT_ELEMENTS(coll1,1,6);
    for(int i=1;i<=16;i*=2)
        coll2.push_back(i);
    coll2.push_back(3);
    PRINT_ELEMENTS(coll1,"coll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    pair<vector<int>::iterator,list<int>::iterator> values;
    values=mismatch(coll1.begin(),coll1.end(),coll2.begin());
    if(values.first==coll1.end())
        cout<<"no mismatch"<<endl;
    else
        cout<<"first mismatch: "
            <<*values.first<<" and "
            <<*values.second<<endl;
    values=mismatch(coll1.begin(),coll1.end(),coll2.begin(),less_equal<int>());
    if(values.first==coll1.end())
        cout<<"always less-or-equal"<<endl;
    else
        cout<<"not less-or-euqal: "
            <<*values.first<<" and "
            <<*values.second<<endl;

}
```

###### **检验“小于”**

bool

**lexicographical_compare**(InputIterator1 beg1,InputIterator1 end1,

​                    InputIterator2 beg2,InputIterator2 end2)

bool

**lexicographical_compare**(InputIterator1 beg1,InputIterator1 end1,

​                    InputIterator2 beg2,InputIterator2 end2,

​                    CompFunc op)

1.两种形式都用来判断区间[beg1,end1)的元素是否小于区间[beg2,end2)的元素

2.第一形式以operator<来比较元素

3.第二形式以二元判断式op(elem1,elem2)比较元素。如果elem1<elem2，则判断式应当返回true

 

下面这个例子展示如何利用这个算法对群集完成”字典次序“的排序

```c++
#include "algostuff.hpp"
using namespace std;

void printCollection(const list<int>& l)
{
    PRINT_ELEMENTS(l);
}

bool lessForCollection(const list<int>& l1,const list<int>& l2)
{
    return lexicographical_compare(l1.begin(),l1.end(),l2.begin(),l2.end());
}

int main()
{
    list<int> c1,c2,c3,c4;
    INSERT_ELEMENTS(c1,1,5);
    c4=c3=c2=c1;
    c1.push_back(7);
    c3.push_back(2);
    c3.push_back(0);
    c4.push_back(2);
    vector<list<int> > cc;
    cc.push_back(c1);
    cc.push_back(c2);
    cc.push_back(c3);
    cc.push_back(c4);
    cc.push_back(c3);
    cc.push_back(c1);
    cc.push_back(c4);
    cc.push_back(c2);
    for_each(cc.begin(),cc.end(),printCollection);
    cout<<endl;
    sort(cc.begin(),cc.end(),lessForCollection);
    for_each(cc.begin(),cc.end(),printCollection);
}
```

## 变动性算法

本节描述的算法会变动区间内的元素内容。有两种方法可以变动元素内容：

1.运用迭代器遍历序列的过程中，直接加以变动

2.将元素从源区间赋值到目标区间的过程中加以变动

#### **复制(copy)元素**

OutputIterator

**copy**(InputIterator sourceBeg,

​    InputIterator sourceEnd,

​    OutputIterator destBeg)

BiderectionalIterator

**copy_backward**(BidirectionalIterator sourceBeg,

​             BidirectionalIterator sourceEnd,

​             BidirectionalIterator destEnd)

1.这两个算法都将源区间[sourceBeg,sourceEnd)中的所有元素赋值到以destBeg为起点或以destEnd为终点的目标区间去

2.返回目标区间内最后一个被赋值元素的下一位置，也就是第一个违背覆盖的元素的位置

3.destBeg或destEnd不可处于[sourceBeg,sourceEnd)区间内

 

下面的例子展示copy()的一些简单用法

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll1;
    list<int> coll2;
    INSERT_ELEMENTS(coll1,1,9);
    copy(coll1.begin(),coll1.end(),back_inserter(coll2));
    copy(coll2.begin(),coll2.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    copy(coll1.rbegin(),coll1.rend(),coll2.begin());
    copy(coll2.begin(),coll2.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

#### **转换(Transforming)和结合(Combining)元素**

算法transform()提供一下两种能力：

###### **1.转换元素**

OutputIterator

**transform**(InputIterator sourceBeg,InputIterator sourceEnd,

​        OutputIterator destBeg,UnaryFunc op)

针对源区间[sourceBeg,sourceEnd)中的每一个元素调用 op(elem)，并将结果写到以destBeg起始的目标区间内。

下面这个例子展示了`transform()`的转换元素功能

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll1;
    list<int> coll2;
    INSERT_ELEMENTS(coll1,1,9);
    PRINT_ELEMENTS(coll1,"coll1: ");
    transform(coll1.begin(),coll1.end(),coll1.begin(),negate<int>());
    PRINT_ELEMENTS(coll1,"negated: ");
    transform(coll1.begin(),coll1.end(),back_inserter(coll2),bind2nd(multiplies<int>(),10));
    PRINT_ELEMENTS(coll2,"coll2: ");
    transform(coll2.rbegin(),coll2.rend(),ostream_iterator<int>(cout," "),negate<int>());
    cout<<endl;
}
```

###### **2.将两序列的元素加以结合**

OutputIterator

**transform**(InputIterator source1Beg,InputIterator source1End,

​         InputIterator source2Beg,

​         OutputIterator destBeg,

​         BinaryFunc op)

针对第一源区间[source1Beg,source1End)以及“从source2Beg开始的第二个源区间“对应的元素，调用：

op(source1Elem,source2Elem) 并将结果以destBeg起始的目标区间内。

下面这个例子展示以上所说的transform()用法：

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll1;
    list<int> coll2;
    INSERT_ELEMENTS(coll1,1,9);
    PRINT_ELEMENTS(coll1,"coll1: ");
    transform(coll1.begin(),coll1.end(),coll1.begin(),coll1.begin(),multiplies<int>());
    PRINT_ELEMENTS(coll1,"squared: ");
    transform(coll1.begin(),coll1.end(),coll1.rbegin(),back_inserter(coll2),plus<int>());
    PRINT_ELEMENTS(coll2,"coll2: ");
    cout<<"diff: ";
    transform(coll1.begin(),coll1.end(),coll2.begin(),ostream_iterator<int>(cout," "),minus<int>());
    cout<<endl;
}
```

#### **互换(Swapping)元素内容**

ForwardIterator

**swap_ranges**(ForwardIterator beg1,ForwardIterator end1,

​           ForwardIterator beg2)

1.强区间[beg1,end1)内的元素和”从beg2开始的区间“内的对应元素互换

2.返回第二区间中”最后一个被交换元素“的下一位置

 

下面这个例子展示swap_ranges()的用法

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll1;
    deque<int> coll2;
    INSERT_ELEMENTS(coll1,1,9);
    INSERT_ELEMENTS(coll2,11,23);
    PRINT_ELEMENTS(coll1,"coll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    deque<int>::iterator pos;
    pos=swap_ranges(coll1.begin(),coll1.end(),coll2.begin());
    PRINT_ELEMENTS(coll1,"\ncoll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    if(pos!=coll2.end())
        cout<<"first element not modified: "<<*pos<<endl;
    swap_ranges(coll2.begin(),coll2.begin()+3,coll2.rbegin());
    PRINT_ELEMENTS(coll2,"\ncoll2:");
}
```

#### **赋予(Assigning)新值**

###### **1.赋予完全相同的数值**

void **fill**(ForwardIterator beg,ForwardIterator end, const T& newValue)

void

**fill**(ForwardIterator beg,Size num,

   const T& newValue)

1.fill()将区间[beg,end)内的每一个元素都赋予新值newValue

2.fill_n()将”从beg开始的前num个元素“赋予新值newValue

3.调用者必须确保目标区间有足够空间，要不就得用插入行迭代器

 

以下程序展示fill()和fill_n()的用法

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    fill_n(ostream_iterator<float>(cout," "),10,7.7);
    cout<<endl;
    list<string> coll;
    fill_n(back_inserter(coll),9,"hello");
    PRINT_ELEMENTS(coll,"coll: ");
    fill(coll.begin(),coll.end(),"again");
    PRINT_ELEMENTS(coll,"coll: ");
    fill_n(coll.begin(),coll.size()-2,"hi");
    PRINT_ELEMENTS(coll,"coll: ");
    list<string>::iterator pos1,pos2;
    pos1=coll.begin();
    pos2=coll.end();
    fill(++pos1,--pos2,"hmmm");
    PRINT_ELEMENTS(coll,"coll: ");
}
```

###### **2.赋予新产生的数值**

void 

**generate**(ForwardIterator beg,ForwardIterator end,

​        Func op)

void

**generate_n**(OutputIterator beg,Size num,

​          Func op)

1.generate()会调用以下动作：op() ，并赋值给区间[beg,end)内的每个元素

2.generate_n()会调用以下动作: op()，并赋值给”以beg起始的区间“内的前num个元素

 

以下程序展示如何利用generate()和generatr_n()安插和赋值一些随机数

```
#include <cstdlib>
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    generate_n(back_inserter(coll),5,rand);
    PRINT_ELEMENTS(coll);
    generate(coll.begin(),coll.end(),rand);
    PRINT_ELEMENTS(coll);
}
```

#### **替换(Replacing)元素**

###### **1.替换序列内的元素**

void

**replace**(ForwardIterator beg,ForwardIterator end,

​       const T& oldValue,const T& newValue)

void

**replace_if**(ForwardIterator beg,ForwardIterator end,

​         UnaryPredicate op,const T& newValue)

1.replace()将区间[beg,end)之内每一个”与oldValue相等“的元素替换成newValue

2.replace_if()将区间[beg,end)之内每一个以下一元判断式:op(elem)返回true的元素替换成newValue

 

以下程序示范replace()和replace_if()的用法

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    INSERT_ELEMENTS(coll,2,7);
    INSERT_ELEMENTS(coll,4,9);
    PRINT_ELEMENTS(coll,"coll: ");
    replace(coll.begin(),coll.end(),6,42);
    PRINT_ELEMENTS(coll,"coll: ");
    replace_if(coll.begin(),coll.end(),bind2nd(less<int>(),5),0);
    PRINT_ELEMENTS(coll,"coll: ");
}
```

**2.复制并替换元素**

OutputIterator

**replace_copy**(InputIterator sourceBeg,InputIterator sourceEnd,

​           OutputIterator destBeg,

​           const T& oldValud,const T& newValue)

OutputIterator

**replace_copy_if**(InputIterator sourceBeg,InputIterator sourceEnd,

​             OutputIterator destBeg,

​             UnaryPredicate op,const T& newValue)

1.replace_copy()是copy()和replace()的组合。他将源区间[beg,end)中的元素赋值到”以destBeg为起点“的目标区间

  同时将其中”与oldValue相等“的所有元素替换为newValue

2.replace_copy()是copy()和replace_if()的组合。

 

以下程序示范如何使用replace_copy()和replace_copy_if()

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    INSERT_ELEMENTS(coll,2,6);
    INSERT_ELEMENTS(coll,4,9);
    PRINT_ELEMENTS(coll);
    replace_copy(coll.begin(),coll.end(),ostream_iterator<int>(cout," "),5,55);
    cout<<endl;
    replace_copy_if(coll.begin(),coll.end(),ostream_iterator<int>(cout," "),bind2nd(less<int>(),5),0);
    cout<<endl;
}
```

## 移除性算法

本节所列的算法是根据元素值或某一准则，在一个区间内移除某些元素。

这些算法并不能改变元素的数量，它们只是将原本置于后面的“不移除元素”向前移动，覆盖那些被移除的元素。

**这些算法都返回逻辑上的新终点**

 

**移除某些特定元素**

**1.移除某序列内的元素**

ForwardIterator

**remove**(ForwardIterator beg,ForwardIterator end,

​       const T& value)

ForwardIterator

**remove_if**(ForwardIterator beg,ForwardIterator end,

​         UnaryPredicate op)

1.remove()会移除区间[beg,end)中每一个“与value相等”的元素

2.remove_if()会移除区间[beg,end)中每一个“令以下一元判断式”: op(elem) 为true的元素。

 

下面程序示范remove()和remove_if()的用法：

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,2,6);
    INSERT_ELEMENTS(coll,4,9);
    INSERT_ELEMENTS(coll,1,7);
    PRINT_ELEMENTS(coll,"coll:     ");
    vector<int>::iterator pos;
    pos=remove(coll.begin(),coll.end(),5);
    PRINT_ELEMENTS(coll,"size not changed: ");
    coll.erase(pos,coll.end());
    PRINT_ELEMENTS(coll,"size changed: ");
    coll.erase(remove_if(coll.begin(),coll.end(),
                         bind2nd(less<int>(),4)),coll.end());
    PRINT_ELEMENTS(coll,"< 4 removed: ");
}
```

**2.复制时一并移除元素**

OutputIterator

**remove_copy**(InputIterator sourceBeg,InputIterator sourceEnd,

​           OutputIterator destBeg,

​           const T& value)

OutputIterator

**remove_copy_if**(InputIterator sourceBeg,InputIterator sourceEnd,

​             OutputIterator destBeg,

​             UnaryPredicate op)

1.remove_copy()是copy()和remove()组合。它将源区间[beg,end)内的所有元素赋值到“以destBeg为起点”的目标区间内。

  并在复制过程中移除“与value相等”的所有元素

2.remove_copy_if是copy()和remove_if()的组合

 

以下程序示范remove_copy()和remove_copy_if的用法

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll1;
    INSERT_ELEMENTS(coll1,1,6);
    INSERT_ELEMENTS(coll1,1,9);
    PRINT_ELEMENTS(coll1);
    remove_copy(coll1.begin(),coll1.end(),ostream_iterator<int>(cout," "),3);
    cout<<endl;
    remove_copy_if(coll1.begin(),coll1.end(),ostream_iterator<int>(cout," "),bind2nd(greater<int>(),4));
    cout<<endl;
    multiset<int> coll2;
    remove_copy_if(coll1.begin(),coll1.end(),inserter(coll2,coll2.end()),bind2nd(less<int>(),4));
    PRINT_ELEMENTS(coll2);
}
```

**移除重复元素**

###### **1.移除连续重复元素**

ForwardIterator

**unique**(ForwardIterator beg,ForwardIterator end)

ForwardIterator

**unique**(ForwardIterator beg,ForwardIterator end

​      BinaryPredicate op)

1.以上两种形式都会移除连续重复元素中的多余元素

2.第一形式将区间[beg,end)内所有“与前一元素相等的元素“移除。

3.第二形式将每一个”位于元素e之后并且造成以下二元判断式:op(elem,e)结果为true”的所有elem元素移除。

  换言之此判断式并非用来将元素和其原本的前一元素比较，而是将它和未被移除的前一元素比较。

 

下面程序示范unique()的用法

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    int source[]={1,4,4,6,1,2,2,3,1,6,6,6,5,7,5,4,4};
    int sourceNum=sizeof(source)/sizeof(source[0]);
    list<int> coll;
    copy(source,source+sourceNum,back_inserter(coll));
    PRINT_ELEMENTS(coll);
    list<int>::iterator pos;
    pos=unique(coll.begin(),coll.end());
    copy(coll.begin(),pos,ostream_iterator<int>(cout," "));
    cout<<endl<<endl;
    copy(source,source+sourceNum,coll.begin());
    PRINT_ELEMENTS(coll);
    coll.erase(unique(coll.begin(),coll.end(),greater<int>()),coll.end());
    PRINT_ELEMENTS(coll);
}
```

###### **2.复制过程中移除重复元素**

OutputIterator

**unique_copy**(InputIterator sourceBeg,InputIterator sourceEnd,

​           OutputIterator destBeg)

OutputIterator

**unique_copy**(InputIterator sourceBeg,InputIterator sourceEnd,

​           OutputIterator destBeg,

​           BinaryPredicate op)

两种形式都是copy()和unique()的组合

 

下面程序示范unique_copy()的用法

```
#include <iterator>
#include "algostuff.hpp"
using namespace std;

bool differenceOne(int elem1,int elem2)
{
    return elem1+1==elem2||elem1-1==elem2;
}

int main()
{
    int source[]={1,4,4,6,1,2,2,3,1,6,6,6,5,7,5,4,4};
    int sourceNum=sizeof(source)/sizeof(source[0]);
    list<int> coll;
    copy(source,source+sourceNum,back_inserter(coll));
    PRINT_ELEMENTS(coll);
    unique_copy(coll.begin(),coll.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    unique_copy(coll.begin(),coll.end(),ostream_iterator<int>(cout," "),differenceOne);
    cout<<endl;
}
```

## 变序性算法

变序性算法改变元素的次序，但不改变元素值。

这些算法不能用于关联式容器，因为在关联式容器中，元素有一定的次序，不能随意变动。

#### **逆转元素次序**

void

**reverse**(BidirectionalIterator beg,BidirectionalIterator end)

OutputIterator

**reverse_copy**(BidirectionalIterator sourceBeg,BidirectionalIterator sourceEnd,

​           OutputIterator destBeg)

1.reverce()会将区间[beg,end)内的元素全部逆序

2.reverse_copy()是reverse()跟copy()的组合

 

下面这个程序展示reverse()和reverse_copy()的用法

```
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    reverse(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"coll: ");
    reverse(coll.begin()+1,coll.end()-1);
    PRINT_ELEMENTS(coll,"coll: ");
    reverse_copy(coll.begin(),coll.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

#### **旋转(Rotating)元素次序**

###### **1.旋转序列内的元素**

void

**rotate**(ForwardIterator beg,ForwardIterator newBeg,

​      ForwardIterator end)

1.将区间[beg,end)内的元素进行旋转，执行后*newBeg成为新的第一元素

2.调用者必须确保newBeg是[beg,end)内的一个有效位置，否则会引发未定义的行为

 

以下程序示范如何使用rotate()

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    rotate(coll.begin(),coll.begin()+1,coll.end());
    PRINT_ELEMENTS(coll,"one left: ");
    rotate(coll.begin(),coll.end()-2,coll.end());
    PRINT_ELEMENTS(coll,"two right: ");
    rotate(coll.begin(),find(coll.begin(),coll.end(),4),coll.end());
    PRINT_ELEMENTS(coll,"4 first: ");
}
```

###### **2.复制并同时旋转元素**

OutputIterator

**rotate_copy**(ForwardIterator sourceBeg,ForwardIterator newBeg,

​          ForwardIterator sourceEnd,

​          OutputIterator destBeg)

这是copy()和rotate()的组合

 

以下程序示范rotate_copy()的用法

```c++
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    set<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll);
    set<int>::iterator pos=coll.begin();
    advance(pos,1);
    rotate_copy(coll.begin(),pos,coll.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    pos=coll.end();
    advance(pos,-2);
    rotate_copy(coll.begin(),pos,coll.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    rotate_copy(coll.begin(),coll.find(4),coll.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

#### **排列元素(Permuting)元素**

bool

**next_permutation**(BidirectionalIterator beg,

​               BidirectionalIterator end)

bool

**prev_permutation**(BidirectionalIterator beg,

​               BidirectionalIterator end)

1.next_permutation()会改变区间[beg,end)内的元素次序，使他们符合“下一个排列次序”

2.prev_permutation()会改变区间[beg,end)内的元素次序，是他们符合“上一个排列次序”

 

下面这个例子展示利用next_permutation()和prev_permutation()将所有元素的所有可能排列的过程

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,3);
    PRINT_ELEMENTS(coll,"on entry: ");
    while(next_permutation(coll.begin(),coll.end()))
        PRINT_ELEMENTS(coll," ");
    PRINT_ELEMENTS(coll,"afterward: ");
    while(prev_permutation(coll.begin(),coll.end()))
        PRINT_ELEMENTS(coll," ");
    PRINT_ELEMENTS(coll,"now: ");
    while(prev_permutation(coll.begin(),coll.end()))
        PRINT_ELEMENTS(coll," ");
    PRINT_ELEMENTS(coll,"afterward: ");
}
```

####  **重排元素(Shuffling,搅乱次序)** 

void

**random_shuffle**(RandomAccessIterator beg,RandomAccessIterator end)

void

**random_shuffle**(RandomAccessIterator beg,RandomAccessIterator end,

​             RandomFunc& op)

1.第一形式使用一个均匀分布随机数产生器来打乱区间[beg,end)内的元素次序

2.第二形式使用op打乱区间[beg,end)内的元素次序。

 

以下程序示范如何调用random_shuffle()来打乱元素次序

```
#include <cstdlib>
#include "algostuff.hpp"
using namespace std;

class MyRandom
{
public:
    ptrdiff_t operator()(ptrdiff_t max)
    {
        double tmp;
        tmp=static_cast<double>(rand())/static_cast<double>(RAND_MAX);
        return static_cast<ptrdiff_t>(tmp * max);
    }
};

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    random_shuffle(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"shuffled: ");
    sort(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"sorted: ");
    MyRandom rd;
    random_shuffle(coll.begin(),coll.end(),rd);
    PRINT_ELEMENTS(coll,"shuffled: ");
}
```

#### **将元素向前搬移**

BidirectionalIterator

**partition**(BidirectionalIterator beg,

​        BidirectionalIterator end,

​        UnaryPredicate op)

BidirectionalIterator

**stable_partition**(BidirectionalIterator beg,

​             BidirectionalIterator end,

​             UnaryPredicate op)

1.这两种算法将区间[beg,end)中造成以下一元判断式：op(elem)结果为true的元素向前端移动

2.stable_partition()会保持元素之间的相对次序。

 

以下程序示范partition()和stable_partition()的用法以及两者的区别

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll1;
    vector<int> coll2;
    INSERT_ELEMENTS(coll1,1,9);
    INSERT_ELEMENTS(coll2,1,9);
    PRINT_ELEMENTS(coll1,"coll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    cout<<endl;
    vector<int>::iterator pos1,pos2;
    pos1=partition(coll1.begin(),coll1.end(),not1(bind2nd(modulus<int>(),2)));
    pos2=stable_partition(coll2.begin(),coll2.end(),not1(bind2nd(modulus<int>(),2)));
    PRINT_ELEMENTS(coll1,"coll1: ");
    cout<<"first odd element: "<<*pos1<<endl;
    PRINT_ELEMENTS(coll2,"coll1: ");
    cout<<"first odd elements: "<<*pos2<<endl;
    PRINT_ELEMENTS(coll2,"coll2: ");
}
```

## 排序算法

STL提供了好几种算法对区间内的元素排序。出来完全排序外，还支持局部排序。

 

#### **对所有元素排序**

void 

**sort**(RandomAccessIterator beg,RandomAccessIterator end)

void 

**sort**(RandomAccessIterator beg,RandomAccessIteratro end,

​    BinaryPredicate op)

 

void

**stable_sort**(RandomAccessIterator beg,RandomAccessIterator end)

void 

**stable_sort**(RandomAccessIterator beg,RandomAccessIterator end,

​          BinaryPredicate op)

1.sort()和stable_sort()的上述第一形式，使用operator<对区间[beg,end)内的所有元素进行排序

2.sort()和stable_sort()的上述第二形式，使用判断式op(elem1,elem2)作为排序准则，对区间[beg,end)内的所有元素进行排序

3.sort()和stable_sort()的区别是，后者保证相等元素的原本相对次序在排序后保持不变。

 

下面这个例子示范sort()的用法

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"on entry: ");
    sort(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"sorted: ");
    sort(coll.begin(),coll.end(),greater<int>());
    PRINT_ELEMENTS(coll,"sorted >:");
}
```

 以下程序示范sort()和stable_sort()两者间的区别 

```
#include "algostuff.hpp"
using namespace std;

bool lessLength(const string& s1,const string& s2)
{
    return s1.length() < s2.length();
}

int main()
{
    vector<string> coll1;
    vector<string> coll2;
    coll1.push_back("1xxx");
    coll1.push_back("2x");
    coll1.push_back("3x");
    coll1.push_back("4x");
    coll1.push_back("5xx");
    coll1.push_back("6xxxx");
    coll1.push_back("7xx");
    coll1.push_back("8xxx");
    coll1.push_back("9xx");
    coll1.push_back("10xxx");
    coll1.push_back("11");
    coll1.push_back("12");
    coll1.push_back("13");
    coll1.push_back("14xx");
    coll1.push_back("15");
    coll1.push_back("16");
    coll1.push_back("17");
    coll2=coll1;
    PRINT_ELEMENTS(coll1,"on entry:\n");
    sort(coll1.begin(),coll1.end(),lessLength);
    stable_sort(coll2.begin(),coll2.end(),lessLength);
    PRINT_ELEMENTS(coll1,"\nwith sort():\n");
    PRINT_ELEMENTS(coll2,"\nwith stable_sort():\n");
}
```

#### **局部排序**

void

**partial_sort**(RandomAccessIterator beg,

​          RandomAccessIterator sortEnd,

​          RandomAccessIterator end)

void

**partial_sort**(RandomAccessIterator beg,

​          RandomAccessIterator sortEnd,

​          RandomAccessIterator end,

​          BinaryPredicate op)

1.以上第一形式，以operator<对区间[beg,end)内的元素进行排序，是区间[beg,end)内的元素处于有序状态。

2.以上第二形式，运用二元判断式: op(elem1,elem2)进行局部排序。

 

以下程序示范partial_sort()的用法

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll;
    INSERT_ELEMENTS(coll,3,7);
    INSERT_ELEMENTS(coll,2,6);
    INSERT_ELEMENTS(coll,1,5);
    PRINT_ELEMENTS(coll);
    partial_sort(coll.begin(),coll.begin()+5,coll.end());
    PRINT_ELEMENTS(coll);
    partial_sort(coll.begin(),coll.begin()+5,coll.end(),greater<int>());
    PRINT_ELEMENTS(coll);
    partial_sort(coll.begin(),coll.end(),coll.end());
    PRINT_ELEMENTS(coll);
}
```

RandomAccessIterator

**partital_sort_copy**(InputIterator sourceBeg,

​               InputIterator sourceEnd,

​               RandomAccessIterator destBeg,

​               RandomAccessIterator destEnd)

RandomAccessIterator

**partial_sort_copy(**InputIterator sourceBeg,

​               InputIterator sourceEnd,

​               RandomAccessIterator destBeg,

​               RandomAccessIterator destEnd,

​               BinaryPredicate op)

1.两者都是copy()和partial_sort()的组合

2.它们将元素从源区间[sourceBeg,sourceEnd)复制到目标区间[destBeg,destEnd)，同时进行排序

3.被排序(被复制)的元素数量是源区间和目标区间两者所含元素数量的较少值

 

以下程序示范partial_sort_copy()的用法

```
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll1;
    vector<int> coll6(6);
    vector<int> coll30(30);
    INSERT_ELEMENTS(coll1,3,7);
    INSERT_ELEMENTS(coll1,2,6);
    INSERT_ELEMENTS(coll1,1,5);
    PRINT_ELEMENTS(coll1);
    vector<int>::iterator pos6;
    pos6=partial_sort_copy(coll1.begin(),coll1.end(),coll6.begin(),coll6.end());
    copy(coll6.begin(),pos6,ostream_iterator<int>(cout," "));
    cout<<endl;
    vector<int>::iterator pos30;
    pos30=partial_sort_copy(coll1.begin(),coll1.end(),coll30.begin(),coll30.end(),greater<int>());
    copy(coll30.begin(),pos30,ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

**根据第n个元素排序**

void

**nth_element**(RandomAccessIterator beg,

​            RandomAccessIterator nth,

​            RandomAccessIterator end)

void 

**nth_element**(RandomAccessIterator beg,

​            RandomAccessIterator nth,

​            RaddomAccessIterator end,

​            BinaryPredicate op)

1.两种形式都对区间[beg,end)内的元素进行排序，使第n个位置上的元素就位。也就是说：

  所有在位置n之前的元素都小于等于它，所有在位置n之后的元素都大于等于它。

 

以下程序示范nth_element()的用法

```
#include "algostuff.hpp"
#include <iterator>
using namespace std;

int main()
{
    deque<int> coll;
    INSERT_ELEMENTS(coll,3,7);
    INSERT_ELEMENTS(coll,2,6);
    INSERT_ELEMENTS(coll,1,5);
    PRINT_ELEMENTS(coll);
    nth_element(coll.begin(),coll.begin()+3,coll.end());
    cout<<"the four lowest elements are: ";
    copy(coll.begin(),coll.begin()+4,ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

#### **Heap算法**

就排序而言，heap是一种特别的元素组织方式，应用于heap排序法(heapsort)。

heap可被视为一个以序列式群集视作而成的二叉树，具有两大性质：

1.第一个元素总是最大

2.总是能够在对数时间内增加或移除一个元素

关于heapsort，可以查看之前的博客：http://www.cnblogs.com/runnyu/p/4677170.html

为了处理heap，STL提供四种算法

1.make_heap()   将某区间内的元素转化成heap

2.push_heap()   对着heap增加一个元素

3.pop_heap()    对着heap取出下一个元素

4.sort_heap()    将heap转化为一个已序群集(此后它就不再是heap了)

 

###### **heap算法细节**

void

**make_heap**(RandomAccessIterator beg,

​          RandomAccessIterator end)

void 

**make_heap**(RandomAccessIterator beg,

​          RandomAccessIterator end,

​          BinaryPredicate op)

两种形式都将区间[beg,end)内的元素转化为heap

 

void

**push_heap**(RandomAccessIterator beg,RandomAccessIterator end)

void 

**push_heap**(RandomAccessIterator beg,RandomAccessIterator end,

​         BinaryPredicate op)

两种形式都将end之前的最后一个元素加入原本就是heap的[beg,end-1)区间内，是区间[beg,end)成为一个heap

 

void

**pop_heap**(RandomAccessIterator beg,RandomAccessIterator end)

void

**pop_heap**(RandomAccessIterator beg,RandomAccessIterator end,

​         BinaryPredicate op)

以上两种形式都将heap[beg,end)内的最高元素，也就是第一元素，移到最后位置，并将剩余区间[beg,end-1)内的元素组织起来成为一个新的heap

 

void

**sort_heap**(RandomAccessIterator beg,RandomAccessIterator end)

void

**sort_heap**(RandomAccessIterator beg,RandomAccessIterator end,

​         BinaryPredicate op)

两种形式都可以将heap[beg,end)转换为一个已序序列

 

以下程序示范如何使用各种heap算法

```c++
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,3,7);
    INSERT_ELEMENTS(coll,5,9);
    INSERT_ELEMENTS(coll,1,4);
    PRINT_ELEMENTS(coll,"on entry: ");
    make_heap(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"after make_heap(): ");
    coll.push_back(17);
    push_heap(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"after push_heap()");
    sort_heap(coll.begin(),coll.end());
    PRINT_ELEMENTS(coll,"after sort_heap(): ");
}
```

## 已序区间算法

针对已序区间执行的算法，执行前提是源区间必须在某个排序准则下已序。

#### **搜寻元素(Searching)**

###### **1.检查某个元素是否存在**

bool

**binary_search**(ForwardIterator beg,ForwardIterator end,

​            const T& value)

bool

**binary_search**(ForwardIterator beg,ForwardIterator end,

​            const T& value,

​            BinaryPredicate op)

以下示范binary_search()的用法

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll);
    if(binary_search(coll.begin(),coll.end(),5))
        cout<<"5 is present"<<endl;
    else
        cout<<"5 is not present"<<endl;
    if(binary_search(coll.begin(),coll.end(),42))
        cout<<"42 is present"<<endl;
    else
        cout<<"42 is not present"<<endl;
}
```

###### **2.检查若干值是否存在**

bool

**includes**(InputIterator beg,

​       InputIterator end,

​       InputIterator searchBeg,

​       InputIterator searchEnd)

bool

**includes**(InputIterator beg,

​       InputIterator end,

​       InputIterator searchBeg,

​       InputIterator searchEnd,

​       BinaryPredicate op)

两种形式都用来判断已序区间[beg,end)是否包含另一已序区间[searchBeg,searchEnd)的全部元素

以下程序示范inlcudes()的用法

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    vector<int> search;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll,"coll: ");
    search.push_back(3);
    search.push_back(4);
    search.push_back(7);
    PRINT_ELEMENTS(search,"search: ");
    if(includes(coll.begin(),coll.end(),search.begin(),search.end()))
        cout<<"all elements of search are also in coll"<<endl;
    else
        cout<<"not all elements of search are also in coll"<<endl;
}
```

###### **3.搜寻第一个或最后一个可能位置**

ForwardIterator

**lower_bound**(ForwardIterator beg,ForwardIterator end,const T& value)

ForwardIterator

**lower_bound**(ForwardIterator beg,ForwardIterator end,const T& value,

​           BinaryPredicate op)

ForwardIterator

**upper_bound**(ForwardIterator beg,ForwardIterator end,const T& value)

ForwardIterator

**upper_bound**(ForwardIterator beg,ForwardIterator end,const T& value,

​           BinaryPredicate op)

1.lower_bound()返回第一个“大于等于value”的元素位置。

2.upper_bound()返回第一个“大于value”元素的位置

 

以下程序示范lower_bound()和upper_bound()的用法

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    INSERT_ELEMENTS(coll,1,9);
    coll.sort();
    PRINT_ELEMENTS(coll);
    list<int>::iterator pos1,pos2;
    pos1=lower_bound(coll.begin(),coll.end(),5);
    pos2=upper_bound(coll.begin(),coll.end(),5);
    cout<<"5 could get position "
        <<distance(coll.begin(),pos1)+1
        <<" up to "
        <<distance(coll.begin(),pos2)+1
        <<" without breaking the sorting"<<endl;
    coll.insert(lower_bound(coll.begin(),coll.end(),3),3);
    coll.insert(upper_bound(coll.begin(),coll.end(),7),7);
    PRINT_ELEMENTS(coll);
}
```

**3.搜寻第一个和最后一个可能位置**

pair<ForwardIterator,ForwardIterator>

**equal_range**(ForwardIterator beg,ForwardIterator end,const T& value)

pair<ForwardIterator,ForwardIterator>

**equal_range**(ForwardIterator beg,ForwardIterator end,const T& value,

​           BinaryPredicate op)

返回值与下式等效: make_pair(lower_bound(...),upper_bound(...))

 

以下程序展示equal_range()的用法

```
#include "algostuff.hpp"
using namespace std;

bool bothEvenOrOdd(int elem1,int elem2)
{
    return elem1%2==elem2%2;
}

int main()
{
    vector<int> coll1;
    list<int> coll2;
    INSERT_ELEMENTS(coll1,1,7);
    INSERT_ELEMENTS(coll2,3,9);
    PRINT_ELEMENTS(coll1,"coll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    if(equal(coll1.begin(),coll1.end(),coll2.begin()))
        cout<<"coll1==coll2"<<endl;
    else
        cout<<"coll1!=coll2"<<endl;
    if(equal(coll1.begin(),coll1.end(),coll2.begin(),bothEvenOrOdd))
        cout<<"even and odd elements correspond"<<endl;
    else
        cout<<"even and odd elements do not correspond"<<endl;
}
```

#### **合并元素(Merging)**

###### **1.两个已序集合的总和**

OutputIterator

**merge**(InputIterator source1Beg,InputIterator source1End,

​      InputIterator source2Beg,InputIterator source2End,

​      OutputIterator destBeg)

OutputIterator

**merge**(InputIterator source1Beg,InputIterator source1End,

​      InputIterator source2Beg,InputIterator source2End,

​      OutputIterator destBeg,BinaryPredicate op)

1.两者都是将两个源区间的元素合并，使得“以destBeg起始的目标区间”内含两个源区间所有元素。

2.目标区间内的所有元素都将按顺序排序

 

下面这个例子展示merge()的用法

```
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll1;
    set<int> coll2;
    INSERT_ELEMENTS(coll1,1,6);
    INSERT_ELEMENTS(coll2,3,8);
    PRINT_ELEMENTS(coll1,"coll1: ");
    PRINT_ELEMENTS(coll2,"coll2: ");
    cout<<"merged: ";
    merge(coll1.begin(),coll1.end(),coll2.begin(),coll2.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
}
```

###### **2.两个已序集合的并集**

OutputIterator

**set_union**(InputIterator source1Beg,InputIterator source1End,

​        InputIterator source2Beg,InputIterator source2End,

​        OutputIterator destBeg)

OutputIterator

**set_union**(InputIterator source1Beg,InputIterator source1End,

​         InputIterator source2Beg,InputIterator source2End,

​         OutputIterator destBeg,BinaryPredicate op)

与merge不一样的是，目标区间的元素要不来自第一源区间，要不就来自第二源区间，或是同时来自两个源区间。例如：

source1:1 2 2 4 6 7 7 9

source2:2 2 2 3 6 6 8 9

dest:1 2 2 2 3 4 6 6 7 7 8 9 

 

###### **3.两个已序集合的交集**

OutputIterator

**set_intersection**(InputIterator source1Beg,InputIterator source1End,

​             InputIterator source2Beg,InputIterator source2End,

​             OutputIterator destBeg)

OutputIterator

**set_intersection**(InputIterator source1Beg,InputIterator source1End,

​             InputIterator source2Beg,InputIterator source2End,

​             OutputIterator destBeg,BinaryPredicate op)

 

###### **4.两个已序集合的差集**

OutputIterator

**set_difference**(InputIterator source1Beg,InputIterator source1End,

​             InputIterator source2Beg,InputIterator source2End,

​             OutputIterator destBeg)

OutputIterator

**set_difference**(InputIterator source1Beg,InputIterator source1End,

​             InputIterator source2Beg,InputIterator source2End,

​             OutputIterator destBeg,BinaryPredicate op)

目标区间的元素只存在于第一源区间，不存在与第二源区间。

## 数值算法

运用数值算法之前必须先加入头文件<numeric>

#### **加工运算后产生结果**

###### **1.对序列进行某种运算**

T

**accumulate**(InputIterator beg,InputIterator end,

​          T initValue)

T 

**accumulate**(InputIterator beg,InputIterator end,

​          T initValue,BinaryFunc op)

1.第一种形式计算InitValue和区间[beg,end)内所有元素的总和。

2.第二种形式计算initValue和区间[beg,end)内每一个元素进行op运算的结果。更具体的说，它针对每一个元素调用以下表达式：

  initValue=op(initValue,elem)

 

下面这个例子展示如何使用accumulate()得到区间内所有元素的总和和乘积：

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,9);
    PRINT_ELEMENTS(coll);
    cout<<"sum: "
        <<accumulate(coll.begin(),coll.end(),0)
        <<endl;
    cout<<"sum: "
        <<accumulate(coll.begin(),coll.end(),-100)
        <<endl;
    cout<<"product: "
        <<accumulate(coll.begin(),coll.end(),1,multiplies<int>())
        <<endl;
    cout<<"product: "
        <<accumulate(coll.begin(),coll.end(),0,multiplies<int>())
        <<endl;
}
```

**2.计算两序列的内积**

T

**inner_product**(InputIterator beg1,InputIterator end1,

​            InputIterator beg2,T initValue)

T

**inner_product**(InputIterator beg1,InputIterator end1,

​            InputIterator beg2,T initValue,

​            BinaryFunc op1,BinaryFunc op2)

1.第一种形式针对“两区间内的每一组对应元素”调用以下表达式：

  initValue=initValue+elem1+elem2

2.第二形式则调用以下表达式:

 initValue=op1(initValue,op2(elem1,elem2))

 

以下程序示范inner_product()的用法

```
#include "algostuff.hpp"
using namespace std;

int main()
{
    list<int> coll;
    INSERT_ELEMENTS(coll,1,6);
    PRINT_ELEMENTS(coll);
    cout<<"inner product: "
        <<inner_product(coll.begin(),coll.end(),coll.begin(),0)
        <<endl;
    cout<<"inner reverse product: "
        <<inner_product(coll.begin(),coll.end(),coll.rbegin(),0)
        <<endl;
    cout<<"product of sums: "
        <<inner_product(coll.begin(),coll.end(),coll.begin(),1,multiplies<int>(),plus<int>())
        <<endl;
}
```

#### **相对值跟绝对值之间的转换**

###### **1.将相对值转换成绝对值**

OutputIterator

**partial_sum**(InputIterator sourceBeg,

​          InputIterator sourceEnd,

​          OutputIterator destBeg)

OutputIterator

**partial_sum**(InputIterator sourceBeg,

​          InputIterator sourceEnd,

​          OutputIterator destBeg,BinaryFunc op)

1.第一形式计算源区间[sourceBeg,sourceEnd)中每个元素的部分和，然后将结果写入以destBeg为起点的目标区间

2.第二形式将源区间[sourceBeg,sourceEnd)中的每个元素和其先前所有元素进行op运算，并将结果写入destBeg为起点的目标区间

  例如对于以下数值序列:a1 a2 a3 ...

  它们分别计算：

  a1,a1+a2,a1+a2+a3,..

  a1,a1 op a2,a1 op a2 op a2,...

以下程序示范partial_sum()的用法

```
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    vector<int> coll;
    INSERT_ELEMENTS(coll,1,6);
    PRINT_ELEMENTS(coll);
    partial_sum(coll.begin(),coll.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
    partial_sum(coll.begin(),coll.end(),ostream_iterator<int>(cout," "),multiplies<int>());
    cout<<endl;
}
```

###### **2.将绝对值转换成相对值**

OutputIterator

**adjacent_difference**(InputIterator sourceBeg,

​                InputIterator sourceEnd,

​                OutputIterator destBeg)

OutputIterator

**adjacent_difference**(InputIterator sourceBeg,

​                InputIterator sourceEnd,

​                OutputIterator destBeg)

1.第一种形式计算区间[sourceBeg,sourceEnd)中每一个元素和其紧邻前驱元素的差额，并将结果写入destBeg为起点的目标区间

2.第二种形式针对区间[sourceBeg,sourceEnd)中每一个元素和其紧邻前驱元素调用op操作，并将结果写入destBeg为起点的目标区间

  对于以下数值序列：

  a1,a2,a3,a4,...

  它们分别计算：

  a1,a2-a1,a3-a2,a4-a3,...

  a1,a2 op a1,a3 op a2,a4 op a3,...

以下程序示范adjacent_difference()的用法

```
#include <iterator>
#include "algostuff.hpp"
using namespace std;

int main()
{
    deque<int> coll;
    INSERT_ELEMENTS(coll,1,6);
    PRINT_ELEMENTS(coll);
    adjacent_difference(coll.begin(),coll.end(),ostream_iterator<int>(cout," "),plus<int>());
    cout<<endl;
    adjacent_difference(coll.begin(),coll.end(),ostream_iterator<int>(cout," "),multiplies<int>());
    cout<<endl;
}
```

## String

#### **动机**

C++标准程序库中的string class使我们可以将string当做一个一般型别。我们可以像对待基本型别那样地复制、赋值和比较string，

再也不必但系内存是否足够、占用的内存实际长度等问题。

#### **操作函数**

###### **1.构造函数和析构函数**

下表列出string的所有构造函数和析构函数

<img src="./STL_img\18_1.png" style="zoom: 50%;" />

###### **2.大小和容量**

size()和length():返回string中现有的字符个数。

max_size()：返回一个string最多能够包含的字符数，这个跟机器本身的限制有关系。

capacity():重新分配内存之前，string所能包含的最大字符数。

 

###### **3.元素存取**

string允许我们对齐所办函字符进行读写。有两种方法可以访问单一字符：

subscript(下标)操作符[]和成员函数at()。两者都返回某指定索引的对应位置上的字符。

 

###### **4.比较**

string支持常见的比较操作符，操作数可以是string或C-string：

 

###### **5.更改内容**

赋值：可以用operator=对字符串赋新值。新值可以是string、C-string或单一字符。

交换：string提供了一个特殊的swap()函数用来交换两字符串内容。

 

###### **6.安插和移除字符**

string提供许多成员函数用于安插、移除、替换、擦除字符。

我们可以使用operator+=、append()和push_back()添加字符。可以使用erase()函数来移除字符

 

###### **7.字串和字符串接合**

我们可以使用成员函数substr()从string身上提取出子字符串。例如：

```
std::string s("interchangeability");
s.substr()              //return a copy of s
s.substr(11)            //return string("ability")
s.substr(5,6)           //returns string("change")
s.substr(s.find('c'))   //returns string("changeability")
```

###### **8.搜索和查找**

下表列出string的搜寻函数

<img src="./STL_img\18_2.png" style="zoom: 50%;" />

所有搜寻函数都返回符合搜寻条件之字符区间内的第一个字符的索引。如果搜寻不成狗，则返回npos。

这些搜寻函数都采用下面的参数方案：

第一参数总是被搜寻的对象；

第二参数(可有可无)指出string内的搜寻起点(索引)

第三参数(可有可无)指出搜寻的字符个数

#### **String对迭代器的支持**

string是字符的有序群集。所以C++标准程序库为string提供了相应接口，以便将字符串当做STL容器使用。

string迭代器是random access(随机存取)迭代器。所以任何STL算法都可与它搭配。

下表列出了string的迭代器操作函数

![]()

<img src="./STL_img\18_3.png" alt="18_3" style="zoom:67%;" />

 下面是string迭代器的运用实例 

```
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

bool nocase_compare(char c1,char c2)
{
    return toupper(c1)==toupper(c2);
}

int main()
{
    string s1("This is a string");
    string s2("STRING");
    if(s1.size()==s2.size()&&equal(s1.begin(),s1.end(),s2.begin(),nocase_compare))
        cout<<"the strings are equal"<<endl;
    else
        cout<<"the strings are not equal"<<endl;
    string::iterator pos;
    pos=search(s1.begin(),s1.end(),s2.begin(),s2.end(),nocase_compare);
    if(pos==s1.end())
        cout<<"s2 is not a substring of s1"<<endl;
    else
        cout<<'"'<<s2<<"\" is a substring of \""
            <<s1<<"\"(at index "<<pos-s1.begin()<<")"<<endl;
}
```

