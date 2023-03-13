#include<iostream>
using namespace std;
class A {
public:
    A()
    {
        std::cout << "A()" << std::endl;
    }
//private: 如果是private继承则类A无法被继承了，我们这里探讨的是protected继承
protected:
   virtual ~A()
   {
        std::cout << "~A()" << std::endl;
   }
};


class B : public A{
public:
    B()
    {
        std::cout << "B()" << std::endl;
    }
    ~B()
    {
        std::cout << "~B()" << std::endl;
    }
};

int main(int argc, char* argv[])
{ 
    /*
    一般来说，一个类如果做父类，那么它应该有析构函数并且这个析构函数都应该是一个虚函数。
    什么情况下父类中可以没有析构函数或者析构函数可以不为虚函数：
    1)子类并没有析构函数（不需要在析构函数中释放任何new出来的数据）。
    2)代码中不会出现父类指针指向子类对象（多态）的情形。
    为防止用父类指针new一个子类对象，可以把父类的析构函数用protected来修饰*/
    A* p = new B();
    //delete p; //错误,protected继承时，~B无法调用~A,即无法访问，所以会编译失败。
    return 0;
}