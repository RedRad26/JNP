#include <initializer_list>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

struct A{
    virtual int a() const = 0;
};

struct B: public A{
    virtual int a() const override {return 3;}
};

struct C: public A{
    virtual int a() const override {return 5;}
};

struct D{};

void aaa(A& a){
    std::cout << a.a() << std::endl;
}

template <typename T>
struct RR: public std::reference_wrapper<T>{
    RR(T& a): std::reference_wrapper<T>(a) {}
    RR(T&& a): std::reference_wrapper<T>(static_cast<T&>(a)) {}
};

void fooo(std::initializer_list<RR<A>> l){
    int sum = 0;

    for(const auto& a: l){
        aaa(a);
    }
    
}





int main(){
    B b;
    C c;
    D d;
    fooo({b, c, B(), C()});
}