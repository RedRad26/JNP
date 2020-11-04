#include <iostream>

using namespace std;

template <typename T>
string toString(T value){
    return to_string(value);
}

template <>
string toString(const string& s){
    return "\"" + s + "\"";
}

template <>
string toString(const char* s){
    string ss(s);
    return "\"" + ss + "\"";
}

template <>
string toString(nullptr_t p){
    return "nullptr";
}

template <>
string toString(char c){
    string s = "";
    return s + "'" + c + "'";
}

// Specialization for last argument
template <typename T>
void printArgs(T lastArg){
    cerr << toString(lastArg) << ")" << endl;
}

// Recursive printing - types deduced automatically when first argument is isolated from ...rest
template <typename T, typename... Ts>
void printArgs(T currentArg, Ts... rest){
    cerr << toString(currentArg) << ", ";
    printArgs(rest...);
}


template <typename... Ts>
void logFunctionCall(const string& functionName, Ts... args){
    cerr << functionName << "(";
    printArgs(args...);
}

int main()
{
    logFunctionCall("foo", 3, 5.5, true, nullptr, "abc", 'C');

    return 0;
}