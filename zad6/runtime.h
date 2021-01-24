#ifndef RUNTIME_H
#define RUNTIME_H

#include <map>
#include "memory.h"

// Represents runtime environment of a program.
class Runtime{
    Memory& memory;
    std::map<std::string, std::size_t> variables;
    bool _ZF = false;
    bool _SF = false;
    std::size_t variableCount = 0;

    public:

    Runtime(Memory& memory): memory(memory) {};

    int64_t& operator[](int64_t addr){
        return memory[addr];
    }

    int64_t operator[](int64_t addr) const{
        return memory[addr];
    }

    // Returns memory address of variable with given name casted to 64 signed word.
    int64_t operator[](const std::string& var) const{
        return variables.at(var);
    }

    // Declares variable with varName and initializes it with value. Throws when there is not enough memory for new variable.
    void declareVariable(const std::string& varName, int64_t value){
        variables.insert({varName, variableCount});
        memory[variableCount] = value;
        variableCount++;
    }

    void setFlags(int64_t value){
        _ZF = value == 0;
        _SF = value < 0;
    }

    bool ZF() const{
        return _ZF;
    }

    bool SF() const{
        return _SF;
    }
};

#endif /* RUNTIME_H */