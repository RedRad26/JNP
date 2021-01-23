#ifndef COMPUTER_H
#define COMPUTER_H

#include "ooasm.h"
#include "memory.h"
#include "runtime.h"

class Computer{
    Memory memory;

    public:
    
    Computer(std::size_t memorySize): memory(memorySize) {}

    void boot(const program& p){
        memory.reset();
        Runtime runtime(memory);
        for(const auto& instruction: p){
            instruction->declare(runtime);
        }
        for(const auto& instruction: p){
            instruction->execute(runtime);
        }
    }

    void memory_dump(std::ostream& s) const{
        memory.dump(s);
    }
};

#endif /* COMPUTER_H */