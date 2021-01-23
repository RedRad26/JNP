#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <ostream>

class Memory{
    std::vector<int64_t> mem;

    public:
    Memory(std::size_t size): mem(size) {};

    int64_t& operator[](int64_t addr){
        if(addr < 0 || (std::size_t) addr >= mem.size()){
            throw std::out_of_range("Access to negative memory index");
        }
        return mem[addr];
    }
    
    const int64_t& operator[](int64_t addr) const{
        if(addr < 0  || (std::size_t) addr >= mem.size()){
            throw std::out_of_range("Access to negative memory index");
        }
        return mem[addr];
    }

    void reset(){
        std::fill(mem.begin(), mem.end(), 0);
    }

    void dump(std::ostream& s) const{
        for(int64_t data: mem){
            s << data << " ";
        }
    }
};

#endif /* MEMORY_H */