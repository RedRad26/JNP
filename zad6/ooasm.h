#ifndef OOASM_H
#define OOASM_H

#include <initializer_list>
#include <memory>
#include "runtime.h"
#include "util.h"

// Represents variable id. const char* is implicitly convertible to this class hence behaviour is correct.
// Size check in constructor is convenient and removes code repetition.
class VarId: public std::string{
    void checkSize(){
        if(size() < 1 || size() > 10){
            throw std::invalid_argument("invalid OOAsm identifier (must be 1 to 10 characters long)");
        }
    }
    public:

    VarId(const char* id): std::string(id){
        checkSize();
    }

    VarId(const std::string& id): std::string(id){
        checkSize();
    }

    VarId(std::string&& id): std::string(std::move(id)){
        checkSize();
    }
};




class rvalue: public NonCopyable{
    public:

    // Gets value that rvalue points to in given runtime.
    virtual int64_t get(const Runtime& rt) const = 0;

    // Like OOAsmInstruction::move
    virtual rvalue* move() && = 0;
    virtual ~rvalue() = default;
};

class lvalue: public rvalue{
    public:

    // Gets reference to place in memory that lvalue refers to in given runtime
    virtual int64_t& get(Runtime& rt) const = 0;

    // Like OOAsmInstruction::move
    virtual lvalue* move() && override  = 0;
};

class num: public rvalue{
    int64_t value;

    public:
    num(int64_t value): value(value) {};

    int64_t get([[maybe_unused]] const Runtime& rt) const override{
        return value;
    }

    // Special overload for getting num's value since it does not depend on runtime
    int64_t get() const{
        return value;
    }

    num* move() && override{
        return new num(value);
    }
};

class lea: public rvalue{
    VarId id;

    lea(lea&& other): id(std::move(other.id)) {}
    public:
    lea(VarId id): id(id) {};

    int64_t get(const Runtime& rt) const override{
        return rt[id];
    }

    lea* move() && override{
        return new lea(std::move(*this));
    }
};



class _mem: public lvalue{
    std::unique_ptr<rvalue> addr;

    _mem(_mem&& other): addr(std::move(other.addr)) {}

    public:
    // Crates new _mem by reconstructing given rvalue under unique_ptr ownership.
    _mem(rvalue&& addr): addr(std::move(addr).move()) {}

    int64_t& get(Runtime& rt) const override{
        return rt[addr->get(rt)];
    }

    int64_t get(const Runtime& rt) const override{
        return rt[addr->get(rt)];
    }

    _mem* move() && override{
        return new _mem(std::move(*this));
    }
};

// This is a workaround because of weird c++ behaviour of nested constructors.
// For some reason ctor(ctor(ctor(ctor(args)))) calls constructor only once
// (in fact it produces exact same binary as ctor(args))
_mem mem(rvalue&& addr){
    return _mem(std::move(addr));
}

// Represents all OOAsm instructions that may be included in program.
class OOAsmInstruction: public NonCopyable{
    public:

    // Action taken during declaration pass of a program.
    virtual void declare(Runtime& rt) const = 0;

    // Action taken during execution pass of a program.
    virtual void execute(Runtime& rt) const = 0;
    virtual ~OOAsmInstruction() = default;

    // "Virtual constructor" but with move semantics. Creates new, equivalent instruction using new,
    // without any guarantees about this instruction.
    virtual OOAsmInstruction* move() && = 0;
};

class OOAsmDeclaration: public OOAsmInstruction{
    public:
    virtual void execute([[maybe_unused]] Runtime& rt) const{
        // Declaration has no effect during program execution.
    }
};

class OOAsmCommand: public OOAsmInstruction{
    public:
    virtual void declare([[maybe_unused]] Runtime& rt) const{
        // Executable command does not declare any variables.
    }
};


class data: public OOAsmDeclaration{
    std::string id;
    int64_t value;

    data(data&& other): id(std::move(other.id)), value(other.value) {};

    public:
    data(VarId id, num&& value): id(id), value(value.get()) {};

    virtual void declare(Runtime& rt) const override{
        rt.declareVariable(id, value);
    }

    virtual data* move() && override{
        return new data(std::move(*this));
    }
};



class mov: public OOAsmCommand{
    std::unique_ptr<lvalue> dst;
    std::unique_ptr<rvalue> src;

    mov(mov&& other): dst(std::move(other.dst)), src(std::move(other.src)) {}

    public:
    mov(lvalue&& dst, rvalue&& src): dst(std::move(dst).move()), src(std::move(src).move()) {}

    virtual void execute(Runtime& rt) const override{
        dst->get(rt) = src->get(rt);
    }

    virtual mov* move() && override{
        return new mov(std::move(*this));
    }
};


class add: public virtual OOAsmCommand{
    std::unique_ptr<lvalue> dst;
    std::unique_ptr<rvalue> src;

    add(add&& other): dst(std::move(other.dst)), src(std::move(other.src)) {}

    public:
    add(lvalue&& dst, rvalue&& src): dst(std::move(dst).move()), src(std::move(src).move()) {};

    virtual void execute(Runtime& rt) const override{
        int64_t& res = dst->get(rt);
        res += src->get(rt);
        rt.setFlags(res);
    }

    virtual add* move() && override{
        return new add(std::move(*this));
    }
};


class sub: public OOAsmCommand{
    std::unique_ptr<lvalue> dst;
    std::unique_ptr<rvalue> src;

    sub(sub&& other): dst(std::move(other.dst)), src(std::move(other.src)) {}

    public:
    sub(lvalue&& dst, rvalue&& src): dst(std::move(dst).move()), src(std::move(src).move()) {};

    virtual void execute(Runtime& rt) const override{
        int64_t& res = dst->get(rt);
        res -= src->get(rt);
        rt.setFlags(res);
    }

    virtual sub* move() && override{
        return new sub(std::move(*this));
    }
};

class inc: public OOAsmCommand{
    std::unique_ptr<lvalue> arg;

    inc(inc&& other): arg(std::move(other.arg)) {}

    public:
    inc(lvalue&& arg): arg(std::move(arg).move()) {};

    virtual void execute(Runtime& rt) const override{
        int64_t& res = arg->get(rt);
        ++res;
        rt.setFlags(res);
    }

    virtual inc* move() && override{
        return new inc(std::move(*this));
    }
};

class dec: public OOAsmCommand{
    std::unique_ptr<lvalue> arg;

    dec(dec&& other): arg(std::move(other.arg)) {}

    public:
    dec(lvalue&& arg): arg(std::move(arg).move()) {};
    
    virtual void execute(Runtime& rt) const override{
        int64_t& res = arg->get(rt);
        --res;
        rt.setFlags(res);
    }

    virtual dec* move() && override{
        return new dec(std::move(*this));
    }
};

class one: public OOAsmCommand{
    std::unique_ptr<lvalue> arg;

    one(one&& other): arg(std::move(other.arg)) {}

    public:
    one(lvalue&& arg): arg(std::move(arg).move()) {};

    virtual void execute(Runtime& rt) const override{
        arg->get(rt) = 1;
    }

    virtual one* move() && override{
        return new one(std::move(*this));
    }
};

class onez: public OOAsmCommand{
    std::unique_ptr<lvalue> arg;

    onez(onez&& other): arg(std::move(other.arg)) {}

    public:
    onez(lvalue&& arg): arg(std::move(arg).move()) {};

    virtual void execute(Runtime& rt) const override{
        if(rt.ZF()){
            arg->get(rt) = 1;
        }
    }

    virtual onez* move() && override{
        return new onez(std::move(*this));
    }
};

class ones: public OOAsmCommand{
    std::unique_ptr<lvalue> arg;

    ones(ones&& other): arg(std::move(other.arg)) {}

    public:
    ones(lvalue&& arg): arg(std::move(arg).move()) {};

    virtual void execute(Runtime& rt) const override{
        if(rt.SF()){
            arg->get(rt) = 1;
        }
    }

    virtual ones* move() && override{
        return new ones(std::move(*this));
    }
};

class program{
    using InsContainer = std::vector<std::unique_ptr<OOAsmInstruction>>;
    std::vector<std::unique_ptr<OOAsmInstruction>> instructions;

    public:
    program(std::initializer_list<RefWrapper<OOAsmInstruction>> instructions){
        for(OOAsmInstruction& instruction: instructions){
            this->instructions.push_back(std::unique_ptr<OOAsmInstruction>(std::move(instruction).move()));
        }
    }

    InsContainer::const_iterator begin() const{
        return instructions.begin();
    }

    InsContainer::const_iterator end() const{
        return instructions.end();
    }
};


#endif /* OOASM_H */