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
    std::string s;
    A() = delete;
    A(const std::string& s): s(s) {
        std::cout << "ctor\n" << std::endl;
    }
    A(A&& other) = delete;
    A(const A& other) {
        std::cout << "copy ctor" << std::endl;
    }
    A& operator=(const A& other) = delete;
    A& operator=(A&& other) = delete;
    operator A() = delete;
};

int main(){
    A a("aaa");
    // ???????????????????
    auto b = A(A(A(A(A(a)))));
    std::cout << b.s << std::endl;
    std::cout << a.s << std::endl;
    


}