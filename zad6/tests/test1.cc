#include <iostream>
#include <vector>

/*class A{
    public:
    std::string s;
    A(){
        std::cout << "def ctor\n" << std::endl;
    }
    A(const std::string& s): s(s) {
        std::cout << "ctor\n" << std::endl;
    }
    A(A&& other): s(std::move(other.s)) {
        std::cout << "move ctor\n" << std::endl;
    }
    A(const A& other): s(std::move(other.s)) {
        std::cout << "copy ctor\n" << std::endl;
    }
    A& operator=(const A& other){
        std::cout << "=\n" << std::endl;
        return *this;
    }
    A& operator=(A&& other){
        std::cout << "=&&\n" << std::endl;
        return *this;
    }
    explicit operator A() const{
        std::cout << "cast" << std::endl;
        return *this;
    }
};*/

class A{
    public:
    int a;

    A(int a): a(a) {}
    A(const A& other): a(other.a+1) {}

};

A B(const A& a){
    return A(a);
}

int main(){
    A a(1);
    auto b = B(B(B(B(a))));
    auto c = A(A(A(A(a))));
    std::cout << b.a << std::endl;
    std::cout << c.a << std::endl;
}