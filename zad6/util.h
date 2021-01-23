#include <functional>

// Utility class for code reuse. Inheriting from it disables copying of object.
// This prevents structures like mov(mov(...)) from being able to compile.
class NonCopyable{
    protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(NonCopyable& other) = delete;
    NonCopyable(NonCopyable&& other) = delete;
    NonCopyable& operator=(NonCopyable& other) = delete;
    NonCopyable& operator=(NonCopyable&& other) = delete;
};

// Extension of std::reference_wrapper that allows for wrapping temporary values.
// This allows us to simply write foo(std::initializer_list<RefWrapper<Base>>){...}
// and pass initializer_lists of objects, not pointers to them.
template <typename T>
struct RefWrapper: public std::reference_wrapper<T>{
    RefWrapper(T& a): std::reference_wrapper<T>(a) {}
    RefWrapper(T&& a): std::reference_wrapper<T>(static_cast<T&>(a)) {}
};