#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <unordered_set>

struct A{
    int a;
    bool operator<(const A& other) const{
        return a < other.a;
    }
};

template <template<class, class...> class T, class... Rest>
int sum(T<A, Rest...> v){
    int s = 0;
    for(const A& a: v){
        s += a.a;
    }
    return s;
}

int main(){
    std::vector<A> va = {{2}, {3}, {4}};
    std::set<A> sa = {{3}, {4}, {5}};
    std::cout << sum(va) << std::endl;
    std::cout << sum(sa) << std::endl;


}