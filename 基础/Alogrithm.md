## 插入排序

####  **基本思想** 

<img src=".\算法\1_1.png" style="zoom:50%;" />

可以把插入排序看成是摸牌。把数组分成三个部分：

1.A[1...j-1]代表手中的牌（已经排序好了）；

2.下标j指出当前正被插入手中的牌

3.A[j+1...n]代表仍在桌子上的牌堆

算法开始的时候手上只有一张牌，然后每次从牌堆中拿出一张牌插入到手中的正确位置，直到牌堆的牌为空。

每次插入时候的算法：

因为每次插入之前我们都保证手上的牌已经排序好了，我们只要从右面开始（j-1）找到第一个比A[j]小的牌（或者没有找到就应该插到第一位），就可以找到A[j]应该插入的位置。

#### **伪代码**

```
INSERTION-SORT(A)
for j=2 to A.length
    key=A[j]
    //Insert A[j] into sorted sequence A[1...j-1]  将A[j]插入到已排序好的队列 A[1...j-1]中 
    i=j-1
    while i>0 and A[i]>key
        A[i+1]=A[i]
        i=i-1
    A[i+1]=key
```

####  **实现跟测试代码** 

```c++
#include <iostream>
using namespace std;

void insertion_sort(int arr[],int length)
{
    for(int j=2;j<=length;++j)
    {
        int key=arr[j];
        int i=j-1;
        while(i>0&&arr[i]>key)   //找到比key大的数就向后移一位
        {
            arr[i+1]=arr[i];
            i=i-1;
        }
        arr[i+1]=key;
    }
}

int main()
{
    int arr[]={0,5,2,4,6,1,3};
    insertion_sort(arr,6);
    for(int i=1;i<=6;++i)
        cout<<arr[i]<<' ';
    cout<<endl;
    system("pause");
}
```

## 归并排序

#### **基本思想** 

归并排序使用的是分治法

分治法的基本思想是将原问题分解为规模较小但类似于原问题的子问题， 递归地求解这些子问题，然后合并这些子问题的解来建立原问题的解。

分治模式在每层递归都有3个步骤

1.分解

2.解决

3.合并

归并排序对应的有下面3个步骤

1.分解：分解待排序的n个元素的序列成各具n/2个元素的两个子序列

2.解决：使用归并排序递归地排序两个子序列

3.合并：合并两个排序的子序列以产生已排序的答案

 

归并算法的关键操作是将两个已排序序列的合并。

我们拿扑克牌当例子：假设桌上有两堆牌已排序的面朝上的牌（最小的在上面），我们希望把这两堆牌合并成单一的排序好的牌。

我们的基本步骤是从两堆牌的牌顶上面拿出较小的一张，放到输出堆，重复这个步骤（如果一个堆为空，则可以当它面朝上的牌为∞）就可以达到目的。 

####  **伪代码** 

 合并代码：将排序好的两个数组A[p...q]和A[q+1...r]合并成一个排序好的数组 

```
MERGE(A,p,q,r)
n1=q-p+1
n2=r-q
let L[1...n1+1] and R[1...n2+1] be a new arrays
for i=1 to n1
    L[i]=A[p+i-1]
for j=1 to n2
    R[j]=A[p+j]
L[n1+1]=max
R[n2+1]=max
i=1
j=1
for k=p to r
    if L[i]<=R[j]
        A[k]=L[i]
        i=i+1
    else
        A[k]=R[j]
        j=j+1
```

 我们把过程MERGE作为归并排序算法中的一个子程序来用。递归的把问题分解成两个子问题，然后合并子问题得到原问题的解 

```
MERGE-SORT(A,p,r)
if p<r
    q=(p+r)/2   向下取整 
    MERGE-SORT(A,p,q)
    MERGE-SORT(A,q+1,r)
    MERGE(A,p,q,r) 
```

####  **实现和测试代码** 

```c
#include <iostream>
#include <limits>
using namespace std;

void merge(int arr[],int p,int q,int r)
{
    int n1=q-p+1;
    int n2=r-q;
    int *L=new int[n1+2];
    int *R=new int[n2+2];
    for(int i=1;i<=n1;++i)
        L[i]=arr[p+i-1];
    for(int j=1;j<=n2;++j)
        R[j]=arr[q+j];
    L[n1+1]=numeric_limits<int>::max();
    R[n2+1]=numeric_limits<int>::max();
    int i=1,j=1;
    for(int k=p;k<=r;++k)
    {
        if(L[i]<=R[j])
            arr[k]=L[i++];
        else
            arr[k]=R[j++];
    }
    delete[] L;
    delete[] R;
}

void merge_sort(int arr[],int p,int r)
{
    if(p<r)
    {
        int q=(p+r)/2;
        merge_sort(arr,p,q);
        merge_sort(arr,q+1,r);
        merge(arr,p,q,r);
    }
}

int main()
{
    int arr[]={0,5,2,4,7,1,3,2,6};
    merge_sort(arr,1,8);
    for(int i=1;i<=8;++i)
        cout<<arr[i]<<' ';
    cout<<endl;
    system("pause");
}
```

## 堆排序

#### **堆** 

如下图所示。（二叉堆）是一个数组，它可以被看成一个近似的完全二叉树。树上的每一个结点对应数组中的一个元素。

`A.length`给出数组元素的个数，`A.heap-size`表示有多少个堆元素存储在该数组中。

![](.\算法\3_1.png)

树的根结点是A[1]，这样给定一个结点的下标i，我们很容易计算得到它的父结点、左孩子和右孩子的下标 

```
PARENT(i)
return i/2        //向下取整

LEFT(i)
return 2i

RIGHT(i)
return 2i+1
```

二叉堆可以分为两种形式：最大堆和最小堆。在这两种堆中，结点的值都要满足堆的性质

在最大堆中，最大堆性质是指出来根以外的所有结点i都要满足：

```
A[PARENT(i)]>=A[i]
```

也就是说，某个结点的值之多与其父结点一样大。因此，堆中的最大元素存放在根结点中。而最小堆性质刚好相反。

在堆排序算法中，我们使用的是最大堆。最小堆通常用于构造优先队列。

####  **维护堆的性质** 

`MAX-HEAPIFY`是用于维护最大堆性质的重要过程。

它的输入为一个数组A和一个下标i。在调用`MAX-HEAPIFY`的时候，我们假定根结点为LEFT(i)和RIGHT(i)的二叉树是最大堆。

这时A[i]有可能小于其孩子，违背了最大堆的性质。`MAX-HEAPIFY`通过让A[i]的值在最大堆中“逐级下降”，从而使得以下标i为根结点的子树重新遵循最大堆性质。

```
MAX-HEAPIFY(A,i)
l=LEFT(i)
r=RIGHT(i)
if l<=A.heap-size and A[l]>A[i]
    largest=l
else
    largest=i
if r<=A.heap-size and A[r]>A[largest]
    largest=r
if largest!=i
    exchange A[i] with A[largest]
    MAX-HEAPIFY(A,largest) 
```

 下面是执行`MAX-HEAPIFY(A,2)`的情况 

<img src=".\算法\3_2.png" style="zoom:50%;" />

在程序的每一步，从A[i],A[LEFT(i)]和A[RIGHT[i]]中选出最大的，并将其下标存储在largest中。

如果A[i]是最大的，那么以i为结点的子树已经是最大堆，程序结束。否则，最大元素是i的某个孩子结点，则交换A[i]和A[largest]的值。

交换之后，下标为largest的结点的值是原来的A[i]，以该节点为根的子树又可能违反最大堆性质。因此，需要对该子树递归调用MAX-HEAPIFY。

对于一个树高为h的结点来说，MAX-HEAPIFY的时间复杂度是O(h)



####  **建堆** 

我们可以用自底向上的方法利用过程MAX-HEAPIFY把一个大小为n=A.length的数组转换成最大堆。

子数组A(n/2+1...n)中的元素都是树的叶结点，每个叶结点都可以看成包含一个元素的堆，所以只需要对树中的其它结点调用一次MAX-HEAPIFY

```
BUILD-MAX-HEAP(A)
A.heap-size=A.length
for i=A.length/2 downto 1
    MAX-HEAPIFY(A,i) 
```

#### **堆排序算法**

初始时候，堆排序算法利用BUILD-MAX-HEAP将输入数组A[1...n]建成最大堆。

因为数组中的最大元素总在根结点A[1]中，通过把它与A[n]进行交换（把A[1]放到数组最后），我们可以让该元素放在正确的位置。

这时候，我们从堆中去掉结点n(通过减少A.heap-size的值来实现)，原来跟的孩子结点仍然是最大最，而新的根结点可能会违背最大堆性质，因此我们需要调用MAX-HEAPIFY(A,1)在A[1...n-1]上构造一个新的最大堆。

堆排序算法会不断重复这一过程，直到堆的大小从n-1降到2

```
HEAPSORT(A)
BUILD-MAX-HEAP(A)
for i=A.length downto 2
    exchange A[1] with A[i]
    A.heap-size=A.heap-size-1
    MAX-HEAPIFY(A,1) 
```

####  **实现与测试代码** 

```c
#include <iostream>
#include <algorithm>
using namespace std;

int heap_size;

void max_heapify(int arr[],int i)
{
    int l=2*i;
    int r=2*i+1;
    int largest;
    if(l<=heap_size&&arr[l]>arr[i])
        largest=l;
    else
        largest=i;
    if(r<=heap_size&&arr[r]>arr[largest])
        largest=r;
    if(largest!=i)
    {
        swap(arr[i],arr[largest]);
        max_heapify(arr,largest);
    }
}

void build_max_heap(int arr[],int length)
{
    heap_size=length;
    for(int i=length/2;i>=1;--i)
        max_heapify(arr,i);
}

void heapsort(int arr[],int length)
{
    build_max_heap(arr,length);
    for(int i=length;i>=2;--i)
    {
        swap(arr[1],arr[i]);
        heap_size--;
        max_heapify(arr,1);
    }
}

int main()
{
    int arr[]={0,4,1,3,2,16,9,10,14,8,7};
    heapsort(arr,10);
    for(int i=1;i<=10;++i)
        cout<<arr[i]<<' ';
    cout<<endl;
    system("pause");
}
```

## 快速排序

与归并排序一样，快速排序也使用了分治思想。

对一个典型的字数组A[p...r]进行快速排序的三步分治过程：

1.数组A[p...r]被划分成两个（可能为空）子数组A[p...q-1]和A[q+1...r]，使得：

  A[p...q-1]中的每一个元素都小于等于A[q]，而A[q]也小于等于A[q+1...r]中的每个元素。

2.通过递归调用快速排序，对字数组A[p...q-1]和A[q+1...r]进行排序

3.因为字数组都是原址排序，所以不需要合并操作：数组A[p...r]已经有序

#### **伪代码**

下面的程序实现快速排序

```
QUICKSORT(A,p,r)
if p<r
    q=PARTITION(A,p,r)
    QUICKSORT(A,p,q-1)
    QUICKSORT(A,q+1,r)
```

为了排序一个数组A的全部元素，初始调用是QUICKSORT(A,1,A.length)

算法的关键部分时PARTITION过程，它实现了对字数组A[p...r]的原址重排

```
//确定最后一个元素的位置    
PARTITION(A,p,r)
x=A[r]
i=p-1
for j=p to r-1
    if A[j]<=x
        i=i+1
        exchange A[i] with A[j]
exchange A[i+1] with A[r]
return i+1
```

数组A[p...r]被划分为4个区域，每个区域都满足一定的性质，对于任意数组下标k，有：

1.若p≤k≤i，则A[k]≤x

2.若i+1≤k≤j-1，则A[k]>x

3.若k=r,则A[k]=x

下图演示了PARTITION如何在一个包含8个元素的数组上进行操作的过程

<img src=".\算法\4_1.png" style="zoom: 67%;" />

####  **实现跟测试代码** 

```c
#include <iostream>
#include <algorithm>
using namespace std;

int partition(int arr[],int p,int r)
{
    int x=arr[r];
    int i=p-1;
    for(int j=p;j<=r-1;++j)
    {
        if(arr[j]<=x)
        {
            i++;
            swap(arr[i],arr[j]);
        }
    }
    swap(arr[i+1],arr[r]);
    return i+1;
}

void quick_sort(int arr[],int p,int r)
{
    if(p<r)
    {
        int q=partition(arr,p,r);
        quick_sort(arr,p,q-1);
        quick_sort(arr,q,r);
    }
}

int main()
{
    int arr[]={0,4,7,5,6,1,2,3};
    quick_sort(arr,1,7);
    for(int i=1;i<=7;++i)
        cout<<arr[i]<<' ';
    cout<<endl;
    system("pause");
}
```

## 散列表

#### **基本概念**

散列表是普通数组概念的推广。对于普通数组可以直接寻址（根据下标），使得能在O(1)时间内访问数组中的任意位置。

如果存储空间允许，我们可以提供一个数组，为每个可能的关键字保留一个位置，以利用直接寻址技术的优势。

当实际存储的关键字数目比全部的可能关键字总数要小时，采用散列表就成为直接数组寻址的一种代替。

在散列表中，不是直接把关键字作为数组的下标，而是根据关键字计算出相应的下标。

#### **直接寻址表**

当关键字的全域U比较小时，直接寻址是一种简单而有效的技术。

假设某应用要用到一个动态集合，其中每个元素都是取自于全域U={0,1,...,m-1}中的一个关键字，这里m不是一个很大的数。

为表示动态集合，我们用一个数组（直接寻址表），记为T[0...m-1]，其中的每个位置（槽）对应全域U中的一个关键字。下图描绘了该方法

<img src=".\算法\5_1.png" style="zoom:67%;" />

其中槽k指向集合中一个关键字为k的元素。如果该集合中没有关键字为k的元素，则T[k]=NIL

几个字典操作实现起来比较简单，而且每一个操作都只需O(1)时间

```
DIRECT-ADDRESS-SEARCH(T,k)
return T[k]

DIRECT-ADDRESS-INSERT(T,x)
T[x.key]=x

DIRECT-ADDRESS-DELETE(T,x)
T[x.key]=NIL
```

#### **散列表**

直接寻址技术的缺点是非常明显的：

如果全域U很大，要存储大小为|U|的一张表T也许不太实际。

还有，实际存储的关键字集合K相对U来说可能很小，使得分配给T的大部分空间都将浪费掉。

在直接寻址方式下，具有关键字k的元素被存放在槽k中。在散列方式下，该元素存放在槽h(k)中，即利用散列函数h，由关键字k计算出槽的位置。

<img src=".\算法\5_2.png" style="zoom:67%;" />

这里散列表的大小m了一般要比|U|小得多。这里存在一个问题：两个关键字可能映射到同一个槽中，我们称这种情况为冲突。

通过链接法解决冲突

在链接法中，把散列到同一槽中的所有元素都放在一个链表中。槽j中由一个指针，它指向存储在所有散列到j的元素的链表的表头，如果不存在这样的元素，则槽j中为NIL

<img src=".\算法\5_3.png" style="zoom:67%;" />

 在采用链接法解决冲突后，散列表T上的字典操作就很容易实现 

```
CHAINED-HASH-INSERT(T,x)
insert x at the head of list T[h(x.key)]

CHAINED-HASH-SEARCH(T,k)
search for an element with key k in list T[h(k)]

CHAINED-HASH-DELETE(T,x)
delete x from the list T[h(x.key)]
```

## 二叉搜索树

#### **基本概念**

二叉搜索树是以一棵二叉树来组织的。这样的一棵树可以使用一个链表数据结构来表示：

其中每个节点就是一个对象，除了key和data之外，每个节点还包含left、right和p属性，分别指向节点的左孩子、右孩子和双亲。

**二叉搜索树的性质**：对于任何结点x，其左子树中的关键字最大不超过`x.key`，其右子树的关键字最小不低于`x.key`。

二叉搜索树性质允许我们通过一个简单的递归算法来按序输出二叉搜索树中的所有关键字，这种算法称为**中序遍历算法**。

```
INORDER-TREE-WALK(x)
if x≠NIL
    INORDER-TREE-WALK(x.left)
    print x.key
    INORDER-TREE-WALK(x.right)
```

调用`INORDER-TREE-WALK(T.root)`就可以输出一个二叉搜索树中的所有元素。

#### **查询二叉搜索树**

**查找**：我们使用下面过程在一棵二叉搜索树中查找一个具有给定关键字的结点。

​     输入一个指向树根的指针和一个关键字k，如果这个结点存在，TREE-SERACH返回一个指向关键字为k的结点的指针；否则返回NIL。

```
TREE-SEARCH(x.k)
if x==NIL or k==x.key
    return x
if k<x.key
    return TREE-SERACH(x.left,k)
else return TREE-SERACH(x.right,l)
```

这个过程从树根开始查找。对于遇到的每个结点x，比较关键字k与x.key：

如果两个关键字相等，查找就终止。

如果k小于x.key，查找在x的左子树继续。因为二叉搜索树的性质，所以k不能被存储在右子树中。

如果k大于x.key，查找在x的右子树继续。

我们可以采用while循环来展开递归，用一种迭代方式重写这个过程。对于大多数计算机，迭代版本的效率要高很多

```
ITERATIVE-TREE-SEARCH(x,k)
while x≠NIL and k≠x.key
    if x<x.key
        x=x.left
    else x=x.right
return x
```

#### **最大关键字元素和最小关键字元素**

通过从树根开始沿着left孩子指针知道遇到一个NIL，二叉搜索树的性质保证了下面代码可以找到最小关键字元素

```
TREE-MINIMUM(x)
while x.left≠NIL
    x=x.left
return x
```

寻找最大关键字元素的伪代码是对称的

```
TREE-MAXIMUM(x)
while x.right≠NIL
    x=x.right
return x
```

#### **后继和前驱**

给定一棵二叉搜索树中的一个结点，有时候需要按中序遍历的次序查找它的后继。后继是指该结点的下一个结点。

```
TREE-SUCCESSOR(x)
if x.right≠NIL
    return TREE-MINIMUM(x.right)
y=x.p
while y≠NIL and x==y.right
    x=y
    y=y.p
return y
```

TREE-SUCCESSOR分为两种情况：

1.如果结点x的右子树非空，那么x的后继恰是x右子树中的最左结点（调用TREE-MINIMUM(x.right)可以找到）

2.如果结点x的右子树为空并有一个后继y，那么y就是x的有左孩子的最底层祖先（x的后继y是x的最底层祖先，并且其左孩子也是x的祖先）。

在下图中，关键字为13的结点的后继是关键字为15的结点。为了找到y，只需简单地从x开始沿树而上直到遇到一个有左孩子的结点（并且其左孩子也是x的祖先）。

<img src=".\算法\6_1.png" style="zoom:67%;" />

#### **插入**

要将一个新值v插入到一棵二叉搜索树T中，需要调用TREE-INSERT。

该过程以结点z作为输入，其中``z.key=v``，`z.left=NIL`，`z.right=NIL`。这个过程要修改T和z的某些属性，来把z插入到树中的相应位置

```
TREE-INSERT(T,z)
y=NIL
x=T.root
while x≠NIL
    y=x
    if z.key<x.key
        x=x.left
    else x=x.right
z.p=y
if y==NIL
    T.root=z    //tree T was empty
else if z.key<y.key
    y.left=z
else y.right=z
```

 `TREE-INSERT`从树根开始，根据`z.key`和`x.key`的比较结果决定向左或向右移动 

<img src=".\算法\6_2.png" style="zoom:75%;" />

#### **删除**

从一棵二叉搜索树T中删除一个结点z分为3种基本情况：

1.如果z没有孩子结点，那么只是简单地将它删除，并修改它的父结点，用NIL作为孩子来替换z

2.如果z只有一个孩子，那么僵这个孩子提升到树中z的位置上，并修改z的父结点，用z的孩子来替换z

3.如果z有两个孩子，那么找z的后继y（一定在z的右子树中），并让y占据树中z的位置。z的原来右子树部分称为y的新的右子树，并且z的左子树称为y的新的左子树。

第三种基本情况分为两种：

我们查找z的后继y，这个后继位于z的右子树中并且没有左孩子。

1.如果y是z的右孩子，那么用y替换z，原来z的左孩子称为y的左孩子

<img src=".\算法\6_3.png" style="zoom:75%;" />

 2.如果y并不是z的右孩子。则先用y的右孩子替换y，然后用y替换z 

![](.\算法\6_4.png)

 为了在二叉搜索树内移动子树，定义一个子过程TRANSPLANT，它是用另一棵子树替换一棵子树并成为其双亲的孩子结点。

当TRANSPLANT用一棵以v为根的子树来替换一棵以u为根的子树时，结点u的双亲就变为结点v的双亲，并且最后v成为u的双亲的相应孩子

```
TRANSPLANT(T,u,v)
if u.p==NIL         //u是T的树根的情况
    T.root=v
else if u==u.p.left //如果u是其双亲的左孩子，则设置u双亲的左孩子为v
    u.p.left=v
else u.p.right=v    //如果u是其双亲的右孩子，则设置u双亲的右孩子为v
if v≠NIL            //设置v的双亲为u的双亲
　　v.p=u.p
```

 利用现成的TRANSPLANT过程，下面是从二叉搜索树T中删除结点z的删除过程 

```
TREE-DELETE(T,z)
//只有一个孩子的情况
if z.left==NIL
    TRANSPLANT(T,z,z.right)
else if z.right==NIL
    TRANSPLANT(T,z,z.left)
//有两个孩子的情况
else y=TREE-MINIMUM(z.right)
    //如果y并不是z的右孩子，则先执行if里面的代码
    if y.p≠z
        TRANSPLANT(T,y,y.right)
        y.right=z.right
        y.right.p=y
    TRANSPLANT(T,z,y)
    y.left=z.left
    y.left.p=y
```

####  **实现与测试代码** 

 定义数据结构和建树函数 

```c
typedef struct node
{
    int key;
    int data;
    node *left;
    node *right;
    node *parent;
}*BSTree;

BSTree tree_build(int k,int d)
{
    //初始化根结点
    node *t=new node();
    t->key=k;
    t->data=d;
    t->left=NULL;
    t->right=NULL;
    t->parent=NULL;
    return t;
}
```

 插入函数 

```c
void tree_insert(BSTree t,int k,int d)
{
    //初始化插入结点
    node *z=new node();
    z->key=k;
    z->data=d;
    z->left=NULL;
    z->right=NULL;
    z->parent=NULL;
    //x是当前结点，y用来记录插入位置的父结点
    node *y=NULL;
    node *x=t;
    //找到插入结点的父结点，并用y记录起来
    while(x!=NULL)
    {
        y=x;
        if(z->key<x->key)
            x=x->left;
        else
            x=x->right;
    }
    //插入结点
    z->parent=y;
    if(z->key<y->key)
        y->left=z;
    else
        y->right=z;
}
```

 中序遍历函数 

```c
void tree_inorder(BSTree t)
{
    if(t!=NULL)
    {
        tree_inorder(t->left);
        cout<<"key:"<<t->key<<"  data:"<<t->data<<endl;
        tree_inorder(t->right);
    }
}
```

 查找函数 

```c
node *tree_search(BSTree t,int key)
{
    while(t!=NULL&&key!=t->key)
    {
        if(key<t->key)
            t=t->left;
        else
            t=t->right;
    }
    return t;
}
```

 最大/小关键字元素和后继 

```c
node *tree_minimum(BSTree t)
{
    while(t->left!=NULL)
        t=t->left;
    return t;
}

node *tree_maximum(BSTree t)
{
    while(t->right!=NULL)
        t=t->right;
    return t;
}

node *tree_next(node *x)
{
    if(x->right!=NULL)
        return tree_minimum(x->right);
    node *y=x->parent;
    while(y!=NULL&&x==y->right)
    {
        x=y;
        y=y->parent;
    }
    return y;
}
```

 删除函数 

```c
void tree_transplant(BSTree *t,node *u,node *v)
{
    if(u->parent==NULL)
        *t=v;
    else if(u==u->parent->left)
        u->parent->left=v;
    else
        u->parent->right=v;
    if(v!=NULL)
        v->parent=u->parent;
}

void tree_delete(BSTree *t,node *z)
{
    if(z->left==NULL)
        tree_transplant(t,z,z->right);
    else if(z->right==NULL)
        tree_transplant(t,z,z->left);
    else
    {
        node *y=NULL;
        y=tree_minimum(z->right);
        if(y->parent!=z)
        {
            tree_transplant(t,y,y->right);
            y->right=z->right;
            y->right->parent=y;
        }
        tree_transplant(t,z,y);
        y->left=z->left;
        y->left->parent=y;
    }
    delete z;
}
```

 测试代码 

```c
int main()
{
    BSTree t=tree_build(3,20);
    cout<<"inorder:"<<endl;
    tree_insert(t,1,50);
    tree_insert(t,4,30);
    tree_insert(t,2,10);
    tree_insert(t,5,40);
    tree_inorder(t);
    cout<<endl;
    int key=3;
    cout<<"search:\nkey="<<key<<"  data="<<tree_search(t,key)->data<<endl<<endl;
    cout<<"minimum:data="<<tree_minimum(t)->data<<endl;
    cout<<"maximum:data="<<tree_maximum(t)->data<<endl;
    node *next=tree_next(tree_search(t,key));
    cout<<"next:key="<<key<<" the next key="<<next->key<<"  data="<<next->data<<endl<<endl;
    tree_delete(&t,tree_search(t,key));
    cout<<"after delete:"<<endl;
    tree_inorder(t);
    return 0;
}
```

## 红黑树

之前介绍了二叉搜索树。它的操作的时间复杂度跟树的高度有关，如果树的高度较高的时候，这些集合操作可能并不比在链表执行得快。

红黑树是许多“平衡”搜索树的一种，可以保证在最坏情况下基本动态集合操作的时间复杂度为`O(lgn)`。

####  **红黑树的性质** 

红黑树是一棵二叉搜索树，它在每个结点上增加了一个存储位来表示结点的颜色，可以是RED和BLACK。

通过对各个结点的颜色进行约束，红黑树确保没有一条路径会比其它路径长出两倍，因此是近似于平衡的。

一个红黑树是满足下面**红黑性质**的二叉搜索树：

1.每个结点或是红色的，或是黑色的

2.根节点是黑色的

3.每个叶结点（NIL）是黑色的

4.如果一个结点是红色的，则它的两个子结点都是黑色的

5.对于每个结点，从该结点到其所有后代叶结点的简单路径上，均包含相同数目的黑色结点

下面是一棵红黑树的例子

![](.\算法\7_1.png)

 为了便于处理红黑树代码的边界条件，使用一个哨兵来代表NIL。 

![](.\算法\7_2.png)

#### **旋转**

搜索树操作TREE-INSERT和TREE-DELETE会对树做修改，结果可能违反了红黑性质。为了维护这些性质，必须改变树中某些结点的颜色以及指针结构。

指针结构的修改是通过旋转来完成的，下面给出了两种旋转：左旋和右旋

<img src=".\算法\7_3.png" style="zoom:75%;" />

 下面是LEFT-ROTATE的伪代码，假设`x.right≠T.nil`且根结点的父结点为`T.nil`。 

```c
LEFT-ROTATE(T,x)
y=x.right            //set y
x.right=y.left       //trun y's left subtree into x'right subtree
if y.left≠T.nil
    y.left.p=x
y.p=x.p              //link x' parent to y
if x.p==T.nil
    T.root=y
else if x==x.p.left
    x.p.left=y
else x.p.right=y
y.left=x             //put x on y'left
x.p=y
```

 下面是LEFT-ROTATE操作修改二叉搜索树的例子。 

![](.\算法\7_4.png)

 RIGHT-ROTATE操作的代码是对称的。 

```
RIGHT-ROTATE(T,y)
x=y.left                    //set x
y.left=x.right            //turn x's right subtree into y's right subtree
if x.right≠T.nil
    x.right.p=y
x.p=y.p                    //link y's parent to x
if y.p==T.nil
    T.root=x
else if y==y.p.left
    y.p.left=x
else y.p.right=x
x.right=y                 //put y on x's left
y.p=x
```

#### 插入

 我们需要对二叉平衡树的插入过程略作修改。我们需要一个辅助程序RB-INSERT-FIXUP来对结点重新着色并选择 。

```
RB-INSERT(T,z)
y=T.nil
x=T.root
while x≠T.nil
    y=x
    if z.key<x.key
        x=x.left
    else
        x=x.right
z.p=y
if y==T.nil
    T.root=z
else y.right=z
z.left=T.nil
z.right=T.nil
z.color=RED
RB-INSERT-FIXUP(T,z)
```

`TREE-INSERT`和`RB-INSERT`之间有4处不同：

1.`TREE-INSERT`内所有的`NIL`都被`T.nil`代替

2.`RB-INSERT`将`z.left`和`z.right`置为`T.nil`

3.将z着为红色

4.因为z着为红色可能违反红黑性质，所有调用`RB-INSERT-FIXUP(T,z)`来保持红黑性质。

```
RB-INSERT-FIXUP(T,z)
while z.p.color==RED
    if z.p==z.p.p.left
        y=z.p.p.right
        if y.color==RED               //case 1
            z.p.color=BALCK
            y.color=BALCK
            y.p.p.color=RED
            z=z.p.p
        else
            if z==z.p.right             //case 2
                z=z.p
                LEFT-ROTATE(T,z)
           else
                z.p.color=BALCK      //case 3
                z.p.p.color=RED
                RIGHT-ROTATE(T,z.p.p)
   else(same as then clause with "right" and "left" exchanged)
T.root.color=BALCK
```

下面给出一个范例，显示在一棵红黑树上`RB-INSERT-FIXUP`如何操作

![](.\算法\7_5.png)

RB-INSERT-FIXUP(T,z)过程一直把z迭代向上，直到z结点的父结点为黑色，每次有3种情况：

(a)判断z的叔结点，如果为红色，应用case 1。

(b)如果z的叔结点为黑色，而且z是的父结点的右孩子，应用case 2

(c)如果z的叔结点为黑色，而且z是的父结点的左孩子，应用case 3

#### **删除**

修改二叉平衡树中TREE-DELETE调用的子过程TRANSPLANT，并将其应用到红黑树上。

RB-TRANSPLANT用一棵以v为根的子树来替换一棵以u为根的子树：结点u的双亲就变为结点v的双亲，并且最后v成为u的双亲的相应孩子

```
RB-TRANSPLANT(T,u,v)
if u.p==T.nil
    T.root=v
else if u==u.p.left
    u.p.left=v
else u.p.right=v
v.p=u.p
```

过程`RB-DELETE`与`TREE-DELETE`类似，只是多了几行伪代码用来记录结点y的踪迹，y有可能导致红黑性质的破坏。

1.当要删除结点z，且此时z有少于两个子结点时，z从树中删除，并将这个孩子（可能为T.nil）提升到树中z的位置上。

2.当z有两个子结点时，y应该是z的后继，并且y将移至树中的z位置。

(a)如果y是z的右孩子，那么用y替换z，原来z的左孩子称为y的左孩子（可以证明原来y的左孩子必为T.nil）。

![](.\算法\7_6.png)

 (b)如果y并不是z的右孩子。则先用y的右孩子替换y，然后用y替换z 

![](.\算法\7_7.png)

如果z的子结点少于两个时，使用y记录z的颜色，x用来记录z的左或右结点。

如果z的子结点数目为两个的时候，y用来记录替换z的结点，x用来记录y的右结点。

```
RB-DELETE(T,z)
y=z
y-original-color=y.color
//只有一个子结点
if z.left==T.nil
    x=z.right
    RB-TRANSPLANT(T,z,z.right)
else if z.right==T.nil
    x=z.left
    RB-TRANSPLANT(T,z,z.left)
//有两个子结点
else y=TREE-MINIMUM(z.right)
    y-original-color=y.color
    x=y.right
    if y.p==z
        x.p=y
    else RB-TRANSPLANT(T,y,y.right)
        y.right=z.right
        y.right.p=y
    RB-TRANSPLANT(T,z,y)
    y.left=z.left
    y.left.p=y
    y.color=z.color
if y.original-color==BALCK
    RB-DELETE-FIXUP(T,x)
```

 删除结点z之后，如果`y.original-color==BALCK`，`RB-DELETE`调用一个辅助过程`RB-DELETE-FIXUP`，该过程通过改变颜色和执行旋转来恢复红黑性质 

```c
while x≠T.root and x.color==BALCK
    if x==x.p.left
        w=x.p.right
        if w.color=RED                 //case 1
            w.color=BALCK
            x.p.color=RED
            LEFT-ROTATE(T,x.p)
            w=x.p.right
        if w.left.color==BALCK and w.right.color==BALCK  //case 2
            w.color=RED
            x=x.p
        else
            if w.right.color=BALCK    //case 3
                w.left.color=BALCK
                w.color=RED
                RIGHT-ROTATE(T,w)
                w=x.p.right
            else                               //case 4
                w.color=x.p.color
                x.p.color=BALCK
                w.right.color=BALCK
                LEFT-ROTATE(T,x.p)
                x=T.root
    else (same as then clause with "right" and "left" exchanged)
x.color=BLACK
```

 下面给出代码中的4种情况 

![](.\算法\7_8.png)

1.x的兄弟结点w是红色的

2.x的兄弟结点w是黑色的，而且w的两个子结点都是黑色的

3.x的兄弟结点w是黑色的，w的左孩子是红色的，w的右孩子是黑色的

4.x的兄弟结点w是黑色的，且w的右孩子是红色的

####  **实现和测试代码** 

因为红黑色应用比较广，所有决定把它的操作封装成一个类。跟伪代码有一点不同的是：没有设置哨兵nil，很多函数都加上判断是否为NULL。

没有处理查找不到跟删除没有的元素的代码。

```c
#define RED 0
#define BLACK 1

#include <stdbool.h>
#include <iostream>
using namespace std;


struct node
{
    int key;
    int data;
    bool color;
    node *left;
    node *right;
    node *parent;
};

class RBTree
{
private:
    node *root;
    //左旋
    void left_rotate(node *x)
    {
        node *y=x->right;
        x->right=y->left;
        if(y->left!=NULL)
            y->left->parent=x;
        y->parent=x->parent;
        if(x->parent==NULL)
            root=y;
        else if(x==x->parent->left)
            x->parent->left=y;
        else
            x->parent->right=y;
        y->left=x;
        x->parent=y;
    }
    //右旋
    void right_rotate(node *y)
    {
        node *x=y->left;
        y->left=y->right;
        if(x->right!=NULL)
            x->right->parent=y;
        x->parent=y->parent;
        if(y->parent==NULL)
            root=x;
        else if(y==y->parent->left)
            y->parent->left=x;
        else
            y->parent->right=x;
        x->right=y;
        y->parent=x;
    }
    //找出根结点为t的子树的最小键的结点
    node *tree_minimum(node *t)
    {
        while(t->left!=NULL)
            t=t->left;
        return t;
    }
    //对插入之后进行修正，保持红黑性质
    void rb_insert_fixup(node *z)
    {
        //当z不是根同时父结点的颜色是red
        while(root!=z&&z->parent->color==RED)
        {
            //如果z结点的父结点是其父结点的左孩子（如果是右孩子则执行else）
            if(z->parent==z->parent->parent->left)
            {
                //叔结点
                node *y=z->parent->parent->right;
                //如果叔结点不为NULL并且颜色为RED
                if(y!=NULL&&y->color==RED)
                {
                    z->parent->color=BLACK;
                    y->color=BLACK;
                    z->parent->parent->color=RED;
                    z=z->parent->parent;
                }
                //否则执行else
                else
                {
                    //如果该结点是其父结点的右孩子
                    if(z==z->parent->right)
                    {
                        z=z->parent;
                        left_rotate(z);
                    }
                    //如果该结点是其父结点的左孩子
                    else
                    {
                        z->parent->color=BLACK;
                        z->parent->parent->color=RED;
                        right_rotate(z->parent->parent);
                    }
                }
            }
            else
            {
                node *y=z->parent->parent->left;
                if(y!=NULL&&y->color==RED)
                {
                    z->parent->color=BLACK;
                    y->color=BLACK;
                    z->parent->parent->color=RED;
                    z=z->parent->parent;
                }
                else
                {
                    if(z==z->parent->left)
                    {
                        z=z->parent;
                        right_rotate(z);
                    }
                    else
                    {
                        z->parent->color=BLACK;
                        z->parent->parent->color=RED;
                        left_rotate(z->parent->parent);
                    }
                }
            }
        }
        root->color=BLACK;
    }
    //中序遍历
    void tree_inorder_p(node *t)
    {
        if(t!=NULL)
        {
            tree_inorder_p(t->left);
            cout<<"key:"<<t->key<<"  data:"<<t->data<<"  color:"<<(t->color?"BLACK":"RED")<<endl;
            tree_inorder_p(t->right);
        }
    }
    //删除结点调用的子过程  用来将一棵以v为根的子树来替换一棵以u为根的子树
    void rb_transplant(node *u,node *v)
    {
        //u为根结点
        if(u->parent==NULL)
            root=v;
        //判断u是其父结点的左孩子还是右孩子
        else if(u==u->parent->left)
            u->parent->left=v;
        else
            u->parent->right=v;
        if(v!=NULL)
            v->parent=u->parent;
    }
    //删除结点之后的修正函数  用来保持红黑性质
    void rb_delete_fixup(node *x)
    {
        node *w;
        while(x!=root&&(x==NULL||x->color==BLACK))
        {
            //如果x是其父结点的左孩子
            if(x==x->parent->left)
            {
                w=x->parent->right;
                if(w->color==RED)
                {
                    w->color=BLACK;
                    x->parent->color=RED;
                    left_rotate(x->parent);
                    w=x->parent->right;
                }
                if((w->left==NULL||w->left->color==BLACK)&&
                   (w->right==NULL||w->right->color==BLACK))
                {
                    w->color=RED;
                    x=x->parent;
                }
                else
                {
                    if((w->right==NULL||w->right->color==BLACK))
                    {
                        if(w->left!=NULL)
                            w->left->color=BLACK;
                        w->color=RED;
                        right_rotate(w);
                        w=x->parent->right;
                    }
                    w->color=x->parent->color;
                    x->parent->color=BLACK;
                    if(w->right!=NULL)
                        w->right->color=BLACK;
                    left_rotate(x->parent);
                    x=root;
                }
            }
            else
            {
                w=x->parent->left;
                if(w->color==RED)
                {
                    w->color=BLACK;
                    x->parent->color=RED;
                    right_rotate(x->parent);
                    w=x->parent->left;
                }
                if((w->left==NULL||w->left->color==BLACK)&&
                   (w->right==NULL||w->right->color==BLACK))
                {
                    w->color=RED;
                    x=x->parent;
                }
                else
                {
                    if((w->left==NULL||w->left->color==BLACK))
                    {
                        if(w->right!=NULL)
                            w->right->color=BLACK;
                        w->color=RED;
                        left_rotate(w);
                        w=x->parent->right;
                    }
                    w->color=x->parent->color;
                    x->parent->color=BLACK;
                    if(w->left!=NULL)
                        w->left->color=BLACK;
                    left_rotate(x->parent);
                    x=root;
                }
            }
        }
        x->color=BLACK;
    }
    void delete_tree(node *t)
    {
        if(t!=NULL)
        {
            delete_tree(t->left);
            delete t;
            delete_tree(t->right);
        }
    }

public:
    RBTree(node *r) {root=r;root->color=BLACK;}
    //插入函数
    void tree_insert(node* z)
    {
        //x是当前结点，y用来记录插入位置的父结点
        node *y=NULL;
        node *x=root;
        //找到插入结点的父结点，并用y记录起来
        while(x!=NULL)
        {
            y=x;
            if(z->key<x->key)
                x=x->left;
            else
                x=x->right;
        }
        //插入结点
        z->parent=y;
        if(y==NULL)
            root=z;
        else if(z->key<y->key)
            y->left=z;
        else
            y->right=z;
        z->right=NULL;
        z->left=NULL;
        z->color=RED;
        rb_insert_fixup(z);
    }
    //根据键值来make_node
    static node *make_node(int k,int d)
    {
        node *z=new node();
        z->key=k;
        z->data=d;
        z->color=RED;
        z->left=NULL;
        z->right=NULL;
        z->parent=NULL;
        return z;
    }
    //中序遍历包装函数
    void tree_inorder(){tree_inorder_p(root);}
    //查找函数
    node *tree_search(int key)
    {
        node *t=root;
        while(t!=NULL&&key!=t->key)
        {
            if(key<t->key)
                t=t->left;
            else
                t=t->right;
        }
        return t;
    }
    //删除函数
    void rb_delete(node *z)
    {
        node *y=z;
        node *x;
        int y_original_color=y->color;
        if(z->left==NULL)
        {
            x=z->left;
            rb_transplant(z,z->left);
        }
        else if(z->right==NULL)
        {
            x=z->left;
            rb_transplant(z,z->left);
        }
        else
        {
            y=tree_minimum(z->right);
            y_original_color=y->color;
            x=y->right;
            if(y->parent==z)
                x->parent=y;
            else
            {
                rb_transplant(y,y->right);
                y->right=z->right;
                y->right->parent=y;
            }
            rb_transplant(z,y);
            y->left=z->left;
            y->left->parent=y;
            y->color=z->color;
        }
        if(y_original_color==BLACK)
            rb_delete_fixup(x);
        delete z;
    }
    //析构函数
    ~RBTree()
    {
        delete_tree(root);
    }
};
```

 测试代码 

```c
#include <stdbool.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;

int main()
{
    srand(time(NULL));
    RBTree *t=new RBTree(RBTree::make_node(1,20));

    node *n1=RBTree::make_node(2,rand()%100);
    node *n2=RBTree::make_node(3,rand()%100);
    node *n3=RBTree::make_node(4,rand()%100);
    node *n4=RBTree::make_node(5,rand()%100);
    t->tree_insert(n1);
    t->tree_insert(n2);
    t->tree_insert(n3);
    t->tree_insert(n4);

    cout<<"after insert:"<<endl;
    t->tree_inorder();
    t->rb_delete(n2);
    cout<<endl;
    cout<<"after delete:"<<endl;
    t->tree_inorder();
    cout<<endl;

    cout<<"serach key=2 data="<<t->tree_search(2)->data<<endl;
    delete t;
    return 0;
}
```

## 动态规划

####  **动态规划与分治法的区别** 

动态规划与分治法相似，都是通过组合子问题的解来求解原问题。

分治法将问题划分为互不相交的子问题，递归地求解子问题，再将它们的解组合起来，求出原问题的解。

与之相反，动态规划应用于子问题重叠的情况。这种情况下，分治算法会反复地求解相同的子问题，而动态规划算法会将这些重复子问题的解保存起来（通常使用数组），避免不必要的计算工作。

#### **动态规划算法的步骤** 

我们通常按如下4个步骤设计一个动态规划算法（最优解跟最优值是不同的概念，下面会举例讲到）：

1.刻画一个最优解的结构特征

2.递归地定义最优值

3.计算最优值，通常采用自底向上的方法

4.利用计算出的信息构造出一个最优解

#### **钢条切割问题**

钢条切割问题是这样的：给定一段长度为n英寸的钢条和一张价格表（如下图所示），求切割钢条方案（最优解），使得销售利益（最优值）最大。

![](.\算法\8_1.png)

长度为n英寸的钢条共有2^n-1种切割方案，因为每一英寸都可以选择切割或不切割。

如果一个最优解将钢条切割成k端(1≤k≤n)  

那么最优切割方案：

<img src=".\算法\8_2.png" style="zoom:75%;" />

 得到的最大利益为： 

![](.\算法\8_3.png)

 对于rn(n≥1)，我们可以用更短的钢条的最优切割利益来描述它（rk(1≤k≤n)代表长度为k英寸的钢条切割之后的最大利益） 

![]()

![8_4](.\算法\8_4.png)

为了求解规模为n的原问题，我们先求解形式完全一样，但规模更小的子问题：

在首次切割后，我们将两端钢条看成两个独立的钢条切割问题的实例。

我们通过组合两个相关子问题的最优解，并在所有可能的两段切割方案中选取组合收益最大者，构成原问题的最优解。

我们称钢条切割问题满足**最优子结构**性质：问题的最优解由相关子问题的最优解组合而成，而这些子问题可以独立求解。

**我们可以使用简单递归的方法来求解这个问题：**

将长度为n的钢条分解为左面开始一段，以及剩余的一段（继续分解）。于是我们可以得到下面的公式：

<img src=".\算法\8_5.png" style="zoom:75%;" />

 下面过程实现了该公式的计算，它采用的是一种直接的自顶而下的递归方法 

```
CUT-ROD(p,n)
if n==0
    return 0
q=-∞
for i=1 to n
    q=max(q,p[i]+CUT-ROD(p,n-i))
return q
```

过程CUT-ROD以价格数组p[1...n]和整数n位输入，返回长度为n的钢条的最大利益。

CUT-ROD的效率很差，因为CUT-ROD反复地用相同的参数值对自身进行递归调用，即反复求解相同的子问题。

**使用动态规划方法求解钢条切割问题：**

动态规划对于每个子问题只求解一次，并将结果保存下来。如果随后再次需要此子问题的解，只需查找保存的结果而不必重新计算。

动态规划有两种等价的实现方法，下面以钢条切割问题为例展示这两种方法：

1.带备忘的自顶向下的方法

下面给出的是自顶向下CUT-ROD过程的伪代码，加入了备忘机制：

```
MEMOIZED-CUT-ROD(p,n)
let r[0..n] be a new arrar
for i=0 to n
    r[i]=-∞
return MEMOIZED-CUT-ROD-AUX(p,n,r)

//算法主体
MEMOIZED-CUT-ROD-AUX(p,n,r)
if r[n]≥0
    return r[n]
if n==0
    q=0
else 
    q=-∞
    for i=1 to n
        q=max(q,p[i]+MEMOIZED-CUT-ROD-AUX(p,n-1,r))
r[n]=q
return q
```

每个子问题的解保存在数组r中，其中每个元素初始化为负无穷来表示未知值。

调用MEMOIZED-CUT-ROD-AUX第一步判断所需值是否已知，如果是，则直接返回保存的值，避免了重复计算子问题。

自顶向下的方法可以这样理解：每次的递归调用会将问题分解为子问题，可以从最终的子问题开始着手，本例可以从MEMOIZED-CUT-ROD-AUX(p,0,r)尝试往上推。

下面是该方法的实现代码

```
#define N 10
#include <iostream>
#include <algorithm>
using namespace std;

//初始化在main函数中实现
int memoized_cut_rod(int p[],int n,int r[])
{
    if(r[n]>=0)
        return r[n];
    int q;
    if(n==0)
        q=0;
    else
    {
        q=-1;
        for(int i=1;i<=n;++i)
            q=max(q,p[i]+memoized_cut_rod(p,n-i,r));
    }
    r[n]=q;
    return q;
}

int main()
{
    //数组p从1开始，因此把p[0]设置为0
    int p[]={0,1,5,8,9,10,17,17,20,24,30};
    int r[N+1];
    for(int i=0;i<=N;++i)
        r[i]=-1;
    for(int i=1;i<=N;++i)
        cout<<"n="<<i<<" r="<<memoized_cut_rod(p,i,r)<<endl;
    return 0;
}
```

 2.自底向上法 

```
BOTTOM-UP-CUT-ROD(p,n)
let r[0..n] be a new array
r[0]=0
for j=1 to n
    q=-∞
    for i=1 to j
        q=max(q,p[i]+r[j-i])
    r[j]=q
return r[n]
```

自底向上版本更为简单，先创建一个数组r来保存子问题的解。对j=1,2,...,n按升序求解每个规模为j的子问题。

自底向上的方法似乎更容易理解：钢条长度从1开始每次递增并记录当前长度的最优值。

下面是该方法的实现代码。为了打印最优值，数组r的初始化放在了main函数中，然后作为参数传进botton_up_cut_rod函数中

```
#define N 10
#include <iostream>
#include <algorithm>

using namespace std;

int botton_up_cut_rod(int p[],int n)
{
    int r[N+1];
    r[0]=0;
    int q;
    for(int j=1;j<=n;++j)
    {
        q=-1;
        for(int i=1;i<=j;++i)
            q=max(q,p[i]+r[j-i]);
        r[j]=q;
        cout<<"n="<<j<<" r="<<r[j]<<endl;
    }
    return q;
}

int main()
{
    int p[]={0,1,5,8,9,10,17,17,20,24,30};
    botton_up_cut_rod(p,N);
    return 0;
}
```

**重构解**

上面给出了使用动态规划方法求解钢条切割问题的最优值。

修改我们的BOTTON-UP-CUT-ROD方法，让它保存最优解的信息（数组s），根据这些信息来构造最优解。

```
EXTENDED-BOTTOM-UP-CUT-ROD(p,n)
let r[0..n] and s[0..n] be new arrays
r[0]=0
for j=1 to n
    q=-∞
    for i=1 to j
        if q<p[i]+r[j-i]
            q=p[i]+r[j-i]
            s[j]=i
    r[j]=q
return r and s
```

跟BOTTON-UP-CUT-ROD有一点不同的是：

在求解规模为j的子问题时将第一段钢条的最优切割长度i保存在s[j]中。EXTENDED-BOTTOM-UP-CUT-ROD(p,10)会返回下面的数组

<img src=".\算法\8_6.png" style="zoom:75%;" />

s[1..n]记录了每条钢条的长度，可以根据数组s构造最优解。

下面是实现代码。为了输出最优值跟最优解，数组r跟数组s的定义都放在main函数中。

```
#define N 10
#include <iostream>

using namespace std;

int extened_botton_up_cut_rod(int p[],int r[],int s[],int n)
{
    int q;
    for(int j=1;j<=n;++j)
    {
        q=-1;
        for(int i=1;i<=j;++i)
        {
            if(q<p[i]+r[j-i])
            {
                q=p[i]+r[j-i];
                s[j]=i;
            }
        }
        r[j]=q;
    }
    return q;
}

int main()
{
    int p[]={0,1,5,8,9,10,17,17,20,24,30};
    int r[N+1]={0};
    int s[N+1]={0};
    extened_botton_up_cut_rod(p,r,s,N);
    for(int i=1;i<=N;++i)
    {
        cout<<"n="<<i<<" r="<<r[i]<<" ";
        //构造最优解
        cout<<"solution:";
        int n=i;
        while(n>0)
        {
            cout<<s[n]<<" ";
            n=n-s[n];
        }
        cout<<endl;
    }
    return 0;
}
```

## 贪心算法

除了最优子结构性质之外，与动态规划不同的是，使用贪心算法解决的问题必须符合**贪心选择性质**：

通过作出局部最优（贪心）选择来构造全局最优解。

在解决符合贪心选择性质的问题时，与动态规划算方法相比，贪心算法更简单、更高效。

#### **活动安排问题**

假定有一个n个活动的集合S={a1,a2,...,an}，这些活动使用同一个资源（例如一个教室），而这个资源在某个时刻只能供一个活动使用。

每个活动ai都有一个开始时间si和一个结束时间fi。我们希望选出一个最大兼容活动集（其中两两活动都不重叠）。例如，考虑下面的活动集合S：

<img src=".\算法\9_1.png" style="zoom:75%;" />

 已按结束时间的单调递增顺序排序： 

<img src=".\算法\9_2.png" style="zoom:75%;" />

我们可以使用贪心算法解决这个问题（可以有其他的贪心选择方案）：

每次我们从可以选择的活动集中选择最早结束的活动，并把它加入最优解。可以选择的活动集是S中开始时间大于fi（i是上次选择的活动）的活动。

因为活动已按结束时间的单调递增顺序排序，因此第一次选择a1加入最优解。然后每次执行贪心选择，得到最优解。

可以简单的证明一下：在满足相容条件下，使结束时间靠前的活动得到资源，这样为后续留下更多的时间，以使更多的活动得到安排。

 **伪代码** 

```
GREEDY-ACTIVITY-SELECTOR(s,f)
n=s.length
A={a1}
k=1
for m=2 to n
    if s[m]≥f[k]
        A=A∪{am}
        k=m
return A
```

其中A代表最大兼容的活动集，k记录了最近加入集合A的活动的下标。

因为活动已按结束时间的单调递增顺序排序，所以只需要每次找出可以选择的活动集中的第一个活动加入A中。

 **代码的实现与测试** 

```
#define N 11
#include <iostream>

using namespace std;

void greedy_activity_selector(int s[],int f[],int a[])
{
    int i=0;
    a[i]=1;
    int k=1;
    for(int m=2;m<=N;++m)
    {
        if(s[m]>=f[k])
        {
            a[++i]=m;
            k=m;
        }
    }
}


int main()
{
    int s[]={0,1,3,0,5,3,5,6,8,8,2,12};
    int f[]={0,4,5,6,7,9,9,10,11,12,14,16};
    int a[N]={0};
    greedy_activity_selector(s,f,a);
    int i=0;
    //输出最大兼容活动集的活动
    while(a[i])
        cout<<a[i++]<<" ";
    cout<<endl;
    return 0;
}
```

## 图的广度优先遍历

 广度优先遍历是最简单的图搜索算法之一，也是许多重要的图算法的模型。 

####  **图的表示** 

对于图G=(V,E)（V代表图中结点的集合，E代表图中所有边的集合），可以用两种标准表示方法表示：

**1.将图作为邻接链表的组合**

对于图G=(V,E)，其邻接链表表示由一个包含|V|条链表的数组Adj构成。

对于每个结点u∈V，邻接链表Adj[u]包含图G中所有与u邻接的结点

 

**2.将图作为邻接矩阵来看待**

对于邻接矩阵表示来说，我们将图G的结点编号为1,2，...，|V|，用一个|V|×|V|的矩阵来表示图G，该矩阵满足下述条件：

![10_1](.\算法\10_1.png)

 下面分别展示了有向图跟无向图的两种表示方法 

![10_2](.\算法\10_2.png)

![](.\算法\10_3.png)

**广度优先遍历**

广度优先遍历的基本思想如下：

1、从图中某个顶点V0出发，并访问此顶点；

2、从V0出发，访问V0的各个未曾访问的邻接点W1，W2，…,Wk;然后,依次从W1,W2,…,Wk出发访问各自未被访问的邻接点；

3、重复步骤2，直到全部顶点都被访问为止。

 

首先我们把图中的每个结点都涂成白色。在算法推进过程中，当第一次遇到该结点时，其颜色会发生改变而且符合下面条件：

所有与黑色结点邻接的结点都已经被发现，而对于灰色结点来说，其邻接结点中可能存在未被发现的白色结点。

假定输入图G=(V,E)是以邻接链表锁表示的，下面给出了广度优先搜索过程BFS的伪代码

```
BFS(G,s)
for each vertex u∈G.V-{s}
    u.color=WHITE
    u.d=∞
    u.π=NIL
s.color=GRAY
s.d=0
s.π=NIL
Q=∅
ENQUEUE(Q,s)
while Q≠∅
    u=DEQUEUE(Q)
    for each v∈G.Adj[u]
        if v.color=WHITE
            v.color=GRAY
            v.d=u.d+1
            v.π=u
            ENQUEUE(Q,v)
    u.color=BLACK
```

其中每个加点的颜色存放在属性u.color中，u的前驱结点存放在属性u.π中，属性u.d记录从源结点s到结点u之间的距离，用一个先进先出的队列Q来管理灰色结点集。

下图描述的是BFS在一个样本图中的推进过程

![](.\算法\10_4.png)

 把结点涂成灰色是为了方便查看推进的顺序，实际算法中只需要涂成白色或者黑色。 

## 图的深度优先搜索

深度优先搜索所使用的策略就像其名字所隐含的：只要可能，就在图中尽量”深入“。

深度优先搜索的步骤如下：

1.访问图中的任一结点后，访问它的任一邻接结点w1

2.从w1出发，访问与w1邻接但还没有访问过的结点w2（向下深入）

3.重复过程2（如果当前访问的结点不能”向下深入“，则回退到上一结点，访问它另外一个邻接结点）知道所有结点都被访问过为止。

#### **深度优先搜索算法**

与广度优先搜索算法一样，深度优先搜索算法在搜索过程中也是对结点进行涂色来指明结点的状态。

与广度优先搜索不同的是，广度优先搜索的前驱子图形成一棵树，而深度优先搜索的前驱子图是由多课深度优先树构成的深度优先森林。

伪代码

```
DFS(G)
for each vertex u∈WHITE
    x.color=WHITE
    u.π=NIL
time=0
for each vertex u∈G.V
     if u.color==WHITE
         DFS-VISIT(G,u)

DFS-VISIT(G,u)
time=time+1
u.d=time
u.color=GRAY
for each v∈G.Adj[u]
    if v.color=WHITE
        v.π=u
        DFS-VISIT(G,v)
u.color=BLACK
```

其中v.d记录的是结点v第一次被发现的时间（涂上灰色的时候）

v.f记录的是搜索完成对v的邻接链表扫描的时间（涂上黑色的时间）

下面是深度优先搜索算法在一个有向图上运行的过程

![](.\算法\11_1.png)

## 最小生成树

**一个例子**

在电子电路设计中，我们常常需要将多个组件的针脚连接在一起。要连接n个针脚，我们可以使用n-1根连线，每根连线连接两个针脚。显然，我们希望所使用的连线长度最短。

我们可以将上述问题用一个连通无向图G=(V,E)来表示，这里V是针脚的集合，E是针脚之间可能连接，每条边的权重代表两个针脚间连线的长度。

我们希望找到E的一个无环子集T（必为一棵树），既能够将所有针脚连接起来，又具有最小权重。我们称取该生成树的问题为最小生成树问题。

![](.\算法\12_1.png)

最小生成树可以用kruskal算法或prim算法求出。

可以形象的区分这两个算法：kruskal算法每次确定一条边，prim算法每次确定一个结点。

#### **kruskal算法**

伪代码

```
MST-KRUSKAL(G,w)
A=∅
for each vertex v∈G.V
    MAKE-SET(v)
sort the edges of G.E into nondecreasing order by weight w
for each edge(u,v)∈G.E,taken in nondecreasing order by weight
    if FIND-SET(v)≠FIND-SET(v)
        A=A∪{(u,v)}
        UNION(v,u)
return A
```

集合A是最小生成树的边的集合，下面是代码的分析：

2-4  将集合A初始化为一个空集，并创建|V|棵树，每棵树仅包含一个结点

5    将图G的边按照权重递增的形式排序

6-9  按照权重从低到高对每条边进行检查。对于每条边(u,v)来说，检查端点u和端点v是否属于同一棵树：

​    1.如果是，该条边不能加入到森林里（否则将形成环路）

　　  2.如果不是，则把这条边加入到集合A中，然后将两棵树中的结点进行合并

 

下面演示了kruskal算法的工作过程

![](.\算法\12_2.png)

**prim算法**

伪代码

```
MST-PRIM(G,w,r)
for each u∈G.V
    u:key=∞
    u:π=NIL
r:key=0
Q=G.V
while Q≠∅
    u=EXTRACT-MIN(Q)
    for each v∈G.Adj[u]
    if v∈Q and w(u,v)<v.key
        v.π=u
        v.key=w(u,v)
```

参数r是根结点，算法代码分析如下：

2-6  将每个结点的key值设置为∞，根结点r的key值设置为0。将每个结点的父结点设置为NIL。对最小优先队列Q进行初始化，使其包含图中所有的结点。

7-12 每次从Q中取出一个key值最小的（第一次是r）结点，然后更新该结点相邻结点的key值和父结点，直到Q为空。

下面演示了prim算法的过程。从根结点开始，每次找出一个距集合G-Q最小的结点加入。

![](.\算法\12_3.png)

## kmp算法

相比于暴力的字串匹配算法，kmp算法在当前不匹配的情况下不会直接退回subStr[0]再重新匹配。

![](.\算法\13_1.png)

![](.\算法\13_2.png)

假设i是src串中当前匹配的索引，j是sub串中当前匹配的索引。

则在src[i] != sub[j]时，kmp算法会保持i不变，然后j指向相应的位置。

观察上图可以看出，这个位置应该是sub[0, j - 1](即ABCDAB)中的最长相同前后缀:

即ABCDAB最长前后缀为AB，容易证明只有在这个位置才可以进行最大匹配(前后缀相同)。

具体算法实现如下(next数组就是sub[0, j - 1]的最长前后缀字符数):

```
int kmpSearch(char* str, char* sub, int next[])
{
    int i = 0, j = 0;
    int tLen = strlen(str);
    int sLen = strlen(sub);
    while (i < tLen && j < sLen)
    {
        // j == -1 第一个不匹配的时候(j = next[0])
        // 匹配的时候i、j向后移一位
        if (j == -1 || str[i] == sub[j])
        {
            ++i;
            ++j;
        }
        // 不匹配的时候i不变，j退回next[j]
        else
        {
            j = next[j];
        }
    }
    if (j == sLen)
        return i - j;
    return -1;
}
```

 next数组可以递归的求得 

```
void getNext(char* sub, int next[])
{
    int len = strlen(sub);
    next[0] = -1;
    int j = 0, k = -1;
    while (j < len - 1)
    {
        if (k == -1 || sub[j] == sub[k])
        {
            ++k;
            ++j;
            next[j] = k;
        }
        else
        {
            k = next[k];
        }
    }
}
```

