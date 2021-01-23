#include <initializer_list>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

class Base {
public:
    virtual Base* test() = 0;
};

template <typename T, typename U>
class Mixin : virtual T {
public:
    virtual U* test() override { 
        return new U();
    }
};

class Example : public virtual Base, public virtual Mixin<Base, Example> {

};
 


/*
void aaa(A& a){
    std::cout << a.a() << std::endl;
    //std::cout << a.b() << std::endl;
}*/
/*
template <typename T>
struct RR: public std::reference_wrapper<T>{
    RR(T& a): std::reference_wrapper<T>(a) {}
    RR(T&& a): std::reference_wrapper<T>(static_cast<T&>(a)) {}
};*/
/*
void fooo(std::initializer_list<RR<A>> l){
    int sum = 0;

    for(const auto& a: l){
        aaa(a);
    }
    
}*/





int main(){
    Example{}.test();
}
/*

class Base {
public:
    virtual void test() = 0;
};

template <typename T>
class Mixin : virtual T {
public:
    virtual void test() override {   }
};

class Example : public virtual Base, public virtual Mixin<Base> {

};

int main(int argc, char** argv) {
    Example{}.test();
    return 0;
}

*/

