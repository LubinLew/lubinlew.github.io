# 智能指针

在现代 C++ 编程中，标准库实现了智能指针，用于帮助确保程序没有内存和资源泄漏，并且发生异常时仍然是安全的。
智能指针在 std 命名空间的 <memory> 头文件中定义。
RAII（Resource Acquisition Is Initialization）是一种利用对象生命周期来控制程序资源（如内存、文件句柄、网络连接、互斥量等等）的简单技术。

| Categories  | Standard        | Type           | Description                 |
| ----------- | --------------- | -------------- | --------------------------- |
| unique_ptr  | C++11           | class template | 具有唯一对象所有权语义的智能指针            |
| make_unique | C++14           | class template |                             |
| shared_ptr  | C++11           | class template | 具有共享对象所有权语义的智能指针            |
| make_shared | C++11           | class template |                             |
| weak_ptr    | C++11           | class template | 对 std::shared_ptr 管理的对象的弱引用 |
| atuo_ptr    | c++98, 被C++17废弃 | class template | 具有严格对象所有权语义的智能指针            |
|             |                 |                |                             |
|             |                 |                |                             |



unique_ptr

`unique_ptr` 对象始终是关联的原始指针的唯一所有者。 `unique_ptr` 对象无法复制，只能移动。

由于每个`unique_ptr` 对象都是原始指针的唯一所有者，因此在其析构函数中它直接删除关联的指针，不需要任何参考计数。

```cpp
#include <iostream>
#include <memory>

class Object {
public:
    //构造函数
    Object(int id) :mId(id) {std::cout << "Object::Constructor" << std::endl;}
    //析构函数
    ~Object() {std::cout << "Object::Destructor" << std::endl;}
    //成员函数
    void printValue(void) { std::cout << mId << std::endl;}

private:
    int mId;
};

int main(int argc, char* argv[])
{
    // 通过原始指针创建 unique_ptr 实例, new Object(99) 返回的指针是 智能指针的构造函数的
    std::unique_ptr<Object> ObjectPtr(new Object(99));

    //通过 unique_ptr 访问类的成员函数
    ObjectPtr->printValue();

    return 0;
}
```



make_unique

 make_unique 创建并返回 unique_ptr 至指定类型的对象，这一点从其构造函数能看出来。make_unique 相较于 unique_ptr 则更加安全。

```cpp
#include <iostream>
#include <memory>

class Object {
public:
    //构造函数
    Object(int id) :mId(id) {std::cout << "Object::Constructor" << std::endl;}
    //析构函数
    ~Object() {std::cout << "Object::Destructor" << std::endl;}
    //成员函数
    void printValue(void) { std::cout << mId << std::endl;}

private:
    int mId;
};

int main(int argc, char* argv[])
{
    // 通过原始指针创建 unique_ptr 实例, 这里就不需要用 new 来申请空间
    auto ObjectPtr = std::make_unique<Object>(99);

    //通过 unique_ptr 访问类的成员
    ObjectPtr->printValue();

    return 0;
}


```





## 用途

这个习惯用法的主要目标是确保在初始化对象的同时进行资源获取，以便在一行代码中创建并准备好对象的所有资源。

实际上，RAII 的主要原则是将任何堆分配的资源（例如，动态分配的内存或系统对象句柄）的所有权赋予堆栈分配的对象，该对象的析构函数包含删除或释放资源的代码以及还有任何相关的清理代码。

在大多数情况下，当您初始化原始指针或资源句柄以指向实际资源时，请立即将指针传递给智能指针。在现代 C++ 中，原始指针仅用于有限范围的小代码块、循环或辅助函数，其中性能至关重要且不会混淆所有权。

下面的示例将原始指针声明与智能指针声明进行比较。

```cpp
void UseRawPointer()
{
    // 使用原始指针 -- 不建议这么做.
    Song* pSong = new Song(L"Nothing on You", L"Bruno Mars"); 

    // Use pSong...

    // 使用之后, 必须手动 delete 掉!
    delete pSong;   
}


void UseSmartPointer()
{
    // 在栈上声明一个智能指针, 冰晶原始指针传递给他.
    unique_ptr<Song> song2(new Song(L"Nothing on You", L"Bruno Mars"));

    // Use song2...
    wstring s = song2->duration_;
    //...

} // 程序运行到此处, song2 会自动被 delete 掉.
```

如示例所示，智能指针是您在堆栈上声明的类模板，并使用指向堆分配对象的原始指针进行初始化。

智能指针初始化后，它拥有原始指针。这意味着智能指针负责删除原始指针指定的内存。智能指针析构函数包含对 delete 的调用，并且因为智能指针是在堆栈上声明的，所以当智能指针超出范围时会调用它的析构函数，即使在堆栈更远的某个地方抛出异常也是如此。

使用熟悉的指针运算符`->`和访问封装的指针，`*`智能指针类重载它以返回封装的原始指针。

C++ 智能指针习语类似于 C# 等语言中的对象创建：您创建对象，然后让系统负责在正确的时间删除它。不同之处在于没有单独的垃圾收集器在后台运行；内存通过标准的 C++ 范围规则进行管理，以便运行时环境更快、更高效。

> 总是在单独的代码行上创建智能指针，永远不要在参数列表中，这样就不会由于某些参数列表分配规则而发生微妙的资源泄漏。

以下示例显示了如何使用`unique_ptr`C++ 标准库中的智能指针类型来封装指向大对象的指针。

```cpp
class LargeObject
{
public:
    void DoSomething(){}
};

void ProcessLargeObject(const LargeObject& lo){}
void SmartPointerDemo()
{    
    // Create the object and pass it to a smart pointer
    std::unique_ptr<LargeObject> pLarge(new LargeObject());

    //Call a method on the object
    pLarge->DoSomething();

    // Pass a reference to a method.
    ProcessLargeObject(*pLarge);

} //pLarge is deleted automatically when function block goes out of scope.
```

该示例演示了以下使用智能指针的基本步骤。

1. 将智能指针声明为自动（局部）变量。（不要在智能指针本身上使用**`new`**or`malloc`表达式。）

2. 在类型参数中，指定封装指针的指向类型。

3. 将原始指针传递给**`new`**智能指针构造函数中的-ed 对象。（某些实用程序函数或智能指针构造函数会为您执行此操作。）

4. 使用重载`->`和`*`运算符访问对象。

5. 让智能指针删除对象。

智能指针旨在在内存和性能方面尽可能高效。例如，in 中唯一的数据成员`unique_ptr`是封装的指针。这意味着它`unique_ptr`与该指针的大小完全相同，四个字节或八个字节。使用重载的智能指针 * 和 -> 运算符访问封装的指针并不比直接访问原始指针慢得多。

智能指针有自己的成员函数，可以使用“点”表示法访问它们。例如，一些 C++ 标准库智能指针有一个重置成员函数，用于释放指针的所有权。当您想在智能指针超出范围之前释放智能指针拥有的内存时，这很有用，如以下示例所示。

```cpp
void SmartPointerDemo2()
{
    // Create the object and pass it to a smart pointer
    std::unique_ptr<LargeObject> pLarge(new LargeObject());

    //Call a method on the object
    pLarge->DoSomething();

    // Free the memory before we exit function block.
    pLarge.reset();

    // Do some other work...

}
```

智能指针通常提供一种直接访问其原始指针的方法。C++ 标准库智能指针有一个`get`用于此目的的成员函数，并`CComPtr`有一个公共`p`类成员。通过提供对底层指针的直接访问，您可以使用智能指针来管理您自己代码中的内存，并且仍然将原始指针传递给不支持智能指针的代码。

```cpp
void SmartPointerDemo4()
{
    // Create the object and pass it to a smart pointer
    std::unique_ptr<LargeObject> pLarge(new LargeObject());

    //Call a method on the object
    pLarge->DoSomething();

    // Pass raw pointer to a legacy API
    LegacyLargeObjectFunction(pLarge.get()); 
}
```

C++ 标准库智能指针
将这些智能指针用作封装指向普通旧 C++ 对象 (POCO) 的指针的首选。

unique_ptr
只允许底层指针的一个所有者。用作 POCO 的默认选择，除非您确定需要shared_ptr. 可以移至新所有者，但不能复制或共享。替换auto_ptr，已弃用。比较boost::scoped_ptr。unique_ptr体积小，效率高；大小是一个指针，它支持右值引用，以便从 C++ 标准库集合中快速插入和检索。头文件：<memory>. 有关更多信息，请参阅如何：创建和使用 unique_ptr 实例和unique_ptr 类。

shared_ptr
引用计数智能指针。当您想要将一个原始指针分配给多个所有者时使用，例如，当您从容器返回指针的副本但想要保留原始指针时。在所有shared_ptr所有者都超出范围或以其他方式放弃所有权之前，不会删除原始指针。大小是两个指针；一个用于对象，一个用于包含引用计数的共享控制块。头文件：<memory>. 有关更多信息，请参阅如何：创建和使用 shared_ptr 实例和shared_ptr 类。

weak_ptr
特殊情况下的智能指针，与shared_ptr. weak_ptr提供对一个或多个shared_ptr实例拥有的对象的访问，但不参与引用计数。当您想观察对象但不要求它保持活动状态时使用。在某些情况下需要中断shared_ptr实例之间的循环引用。头文件：<memory>. 有关更多信息，请参阅如何：创建和使用 weak_ptr 实例和weak_ptr 类。

https://en.cppreference.com/w/cpp/memory

[ Microsoft - Smart pointers (Modern C++)](https://docs.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-160)

https://blog.csdn.net/qq_40421919/article/details/88832478

https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique