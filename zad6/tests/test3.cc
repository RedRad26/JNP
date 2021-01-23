class A{
    int a;
    public:
    A(A& a) = delete;
    A(int a): a(a){};
};

int main(){
    A a = 123;
    auto b = A(A(123));
}