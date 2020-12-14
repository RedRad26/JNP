#ifndef COMPUTER_H
#define COMPUTER_H

#include <array>
#include <type_traits>
#include <cstdint>

// Forward declarations of TMPAsm instructions

template <int64_t n>
struct Num;

template <int64_t n>
struct Lea;

template <int64_t id>
struct Label;

template <typename Addr>
struct Mem;

template <int64_t id, typename Val>
struct D;

template <typename Dst, typename Src>
struct Mov;

template <typename Arg1, typename Arg2>
struct Add;

template <typename Arg1, typename Arg2>
struct Sub;

template <typename Arg>
struct Inc;

template <typename Arg>
struct Dec;

template <typename Arg1, typename Arg2>
struct And;

template <typename Arg1, typename Arg2>
struct Or;

template <typename Arg>
struct Not;

template <typename Arg1, typename Arg2>
struct Cmp;

template <int64_t labelId>
struct Jmp;

template <int64_t labelId>
struct Jz;

template <int64_t labelId>
struct Js;


// Helper types for use in static_assert in templates

template <typename T>
struct isLValue{
    constexpr static bool value = false;
};

template <typename T>
struct isLValue<Mem<T>>{
    constexpr static bool value = true;
};

template <typename T>
struct isRValue{
    constexpr static bool value = false;
};

template <int64_t n>
struct isRValue<Num<n>>{
    constexpr static bool value = true;
};

template <int64_t id>
struct isRValue<Lea<id>>{
    constexpr static bool value = true;
};

template <typename T>
struct isRValue<Mem<T>>{
    constexpr static bool value = true;
};

template <typename T>
struct isNumericValue{
    constexpr static bool value = false;
};

template <int64_t n>
struct isNumericValue<Num<n>>{
    constexpr static bool value = true;
};

template <typename Ins>
struct isVariableDeclaration{
    constexpr static bool value = false;
};

template <int64_t id, typename Val>
struct isVariableDeclaration<D<id, Val>>{
    constexpr static bool value = true;
};

template <typename Ins>
struct isLabelDeclaration{
    constexpr static bool value = false;
};

template <int64_t id>
struct isLabelDeclaration<Label<id>>{
    constexpr static bool value = true;
};

template <typename Ins>
struct isInstruction{
    constexpr static bool value = false;
};

template <typename Arg>
struct isInstruction<Inc<Arg>>{
    constexpr static bool value = true;
};

template <typename Arg>
struct isInstruction<Dec<Arg>>{
    constexpr static bool value = true;
};

template <typename Arg>
struct isInstruction<Not<Arg>>{
    constexpr static bool value = true;
};

template <int64_t id>
struct isInstruction<Jmp<id>>{
    constexpr static bool value = true;
};

template <int64_t id>
struct isInstruction<Js<id>>{
    constexpr static bool value = true;
};

template <int64_t id>
struct isInstruction<Jz<id>>{
    constexpr static bool value = true;
};

template <typename Arg1, typename Arg2>
struct isInstruction<Mov<Arg1, Arg2>>{
    constexpr static bool value = true;
};

template <typename Arg1, typename Arg2>
struct isInstruction<Add<Arg1, Arg2>>{
    constexpr static bool value = true;
};

template <typename Arg1, typename Arg2>
struct isInstruction<Sub<Arg1, Arg2>>{
    constexpr static bool value = true;
};

template <typename Arg1, typename Arg2>
struct isInstruction<And<Arg1, Arg2>>{
    constexpr static bool value = true;
};

template <typename Arg1, typename Arg2>
struct isInstruction<Or<Arg1, Arg2>>{
    constexpr static bool value = true;
};

template <typename Arg1, typename Arg2>
struct isInstruction<Cmp<Arg1, Arg2>>{
    constexpr static bool value = true;
};


// Hashes variable name into 64 bit integer
constexpr int64_t Id(const char* s){
    const std::string_view v = s;
    const std::size_t size = v.size();
    if(size < 1 || size > 6){
        throw "Identifier must be 1 to 6 characters long";
    }
    int64_t id = 0;
    for(std::size_t i = 0; i < size; i++){
        char c = v[i];
        if(('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')){
            id += int64_t(c >= 'a' ? c - 'a' + 'A' : c) << 7*i;
        } else {
            throw "Identifier must contain only alfanumeric characters";
        }
    }
    return id;
}

// Structs for transfering between template garbage and proper representation

// Represents r- and l-values in program
struct Variable{
    int64_t val;  // Raw value, memory address or variable id
    bool id;      // Differentiates between value/address and variable id
    uint64_t dereferences;  // Indicates how many times val should be dereferenced (0 means val is a raw value)
};

// Represents declaration of variable with D instruction
struct Declaration{
    int64_t id;
    int64_t val;
};

// Represents declaration of a label. (Instruction numbers are reversed - 0 is after the last instruction)
struct LabelDeclaration{
    int64_t id;
    std::size_t instructionNumber;
};

// Represents all instructions in TMPAsm
struct Instruction{
    enum InstructionType{
        _MOV,
        _ADD,
        _SUB,
        _INC,
        _DEC,
        _AND,
        _OR,
        _NOT,
        _CMP,
        _JMP,
        _JZ,
        _JS,
    };

    InstructionType type;
    int64_t labelId;
    Variable arg1;
    Variable arg2;
};

// Generic list type. Will be used to transform Program<Ins...> type to std::arrays of
// various informations
template <typename T>
struct List{
    T value;
    List const* next;
};



// Definitions of TMPAsm instructions

template <int64_t n>
struct Num{
    constexpr static Variable v = {n, false, 0};
};

template <int64_t n>
struct Lea{
    constexpr static Variable v = {n, true, 0};
};

template <typename Addr>
struct Mem{
    static_assert(isRValue<Addr>::value, "In Mem<Addr> - Addr must be an r-value");
    constexpr static Variable v = {
        Addr::v.val,
        Addr::v.id,
        Addr::v.dereferences + 1
    };
};

template <int64_t id, typename Val>
struct D{
    static_assert(isNumericValue<Val>::value, "In D<id, Val> - Val must be a numeric value (Num<n>)");
    constexpr static Declaration d = {id, Val::v.val};
};

template <typename Dst, typename Src>
struct Mov{
    static_assert(isLValue<Dst>::value, "In Mov<Dst, Src> - Dst must be an l-value (Mem instruction)");
    static_assert(isRValue<Src>::value, "In Mov<Dst, Src> - Src must be an r-value");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_MOV, 0,
        Dst::v,
        Src::v
    };
};

template <typename Arg1, typename Arg2>
struct Add{
    static_assert(isLValue<Arg1>::value, "In Add<Arg1, Arg2> - Arg1 must be an l-value (Mem instruction)");
    static_assert(isRValue<Arg2>::value, "In Add<Arg1, Arg2> - Arg2 must be an r-value");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_ADD, 0,
        Arg1::v,
        Arg2::v
    };
};

template <typename Arg1, typename Arg2>
struct Sub{
    static_assert(isLValue<Arg1>::value, "In Sub<Arg1, Arg2> - Arg1 must be an l-value (Mem instruction)");
    static_assert(isRValue<Arg2>::value, "In Sub<Arg1, Arg2> - Arg2 must be an r-value");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_SUB, 0,
        Arg1::v,
        Arg2::v
    };
};

template <typename Arg>
struct Inc{
    static_assert(isLValue<Arg>::value, "In Inc<Arg> - Arg must be an l-value (Mem instruction)");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_INC, 0,
        Arg::v, {}
    };
};

template <typename Arg>
struct Dec{
    static_assert(isLValue<Arg>::value, "In Dec<Arg> - Arg must be an l-value (Mem instruction)");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_DEC, 0,
        Arg::v, {}
    };
};

template <typename Arg1, typename Arg2>
struct And{
    static_assert(isLValue<Arg1>::value, "In And<Arg1, Arg2> - Arg1 must be an l-value (Mem instruction)");
    static_assert(isRValue<Arg2>::value, "In And<Arg1, Arg2> - Arg2 must be an r-value");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_AND, 0,
        Arg1::v,
        Arg2::v
    };
};

template <typename Arg1, typename Arg2>
struct Or{
    static_assert(isLValue<Arg1>::value, "In Or<Arg1, Arg2> - Arg1 must be an l-value (Mem instruction)");
    static_assert(isRValue<Arg2>::value, "In Or<Arg1, Arg2> - Arg2 must be an r-value");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_OR, 0,
        Arg1::v,
        Arg2::v
    };
};

template <typename Arg>
struct Not{
    static_assert(isLValue<Arg>::value, "In Not<Arg> - Arg must be an l-value (Mem instruction)");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_NOT, 0,
        Arg::v, {}
    };
};

template <typename Arg1, typename Arg2>
struct Cmp{
    static_assert(isRValue<Arg1>::value, "In Cmp<Arg1, Arg2> - Arg1 must be an r-value");
    static_assert(isRValue<Arg2>::value, "In Cmp<Arg1, Arg2> - Arg2 must be an r-value");
    constexpr static Instruction ins = {
        Instruction::InstructionType::_CMP, 0,
        Arg1::v,
        Arg2::v
    };
};


template <int64_t _id>
struct Label{
    constexpr static int64_t id = _id;
};

template <int64_t labelId>
struct Jmp{
    constexpr static Instruction ins = {
        Instruction::InstructionType::_JMP, labelId, {}, {}
    };
};

template <int64_t labelId>
struct Jz{
    constexpr static Instruction ins = {
        Instruction::InstructionType::_JZ, labelId, {}, {}
    };
};

template <int64_t labelId>
struct Js{
    constexpr static Instruction ins = {
        Instruction::InstructionType::_JS, labelId, {}, {}
    };
};


// Some wrappers to circumvent conditional typing rules.

/*
We can't write
template <typename T>
struct A{
    constexpr static int n = containsAorB<T>::value ? T::a : T::b;
};
when one member does not exist on type T. Solution - wrap type T in type that contains both but only
one is derived from actual T, and second is a dummy.

In theory this could be solved "easier" by providing all members (including dummies) directly on
instruction types at the expense of including unnecessary fields in types.
*/

template <typename Ins>
struct vdWrapper{
    constexpr static Declaration d = Ins::d;
    constexpr static Instruction ins = {};
    constexpr static int64_t id = 0;
};

template <typename Ins>
struct insWrapper{
    constexpr static Declaration d = {};
    constexpr static Instruction ins = Ins::ins;
    constexpr static int64_t id = 0;
};

template <typename Ins>
struct ldWrapper{
    constexpr static Declaration d = {};
    constexpr static Instruction ins = {};
    constexpr static int64_t id = Ins::id;
};

template <typename Ins>
struct tmpasmWrapper{
    using W = typename std::conditional<isInstruction<Ins>::value,
                    insWrapper<Ins>,
                    typename std::conditional<isVariableDeclaration<Ins>::value,
                        vdWrapper<Ins>,
                        ldWrapper<Ins>
                    >::type
                >::type;
    constexpr static Declaration d = W::d;
    constexpr static Instruction ins = W::ins;
    constexpr static int64_t id = W::id;
};


// Program<> type - folds program "type" into linked list of conventional structs that can be
// used to initialize std::array of useful info

template<typename ... Ins>
struct Program{};

template<>
struct Program<>{
    constexpr static std::size_t decN = 0;
    constexpr static List<Declaration> declarations = {{}, nullptr};
    constexpr static std::size_t insN = 0;
    constexpr static List<Instruction> instructions = {{}, nullptr};
    constexpr static std::size_t labN = 0;
    constexpr static List<LabelDeclaration> labels = {{}, nullptr};
};

template <typename Ins, typename ... rest>
struct Program<Ins, rest...>{
    constexpr static bool ivd = isVariableDeclaration<Ins>::value;
    constexpr static bool ild = isLabelDeclaration<Ins>::value;
    constexpr static bool ii = isInstruction<Ins>::value;
    static_assert(ivd || ild || ii, "Invalid TMPAsm instruction");

    // T as in Tail
    using T = Program<rest...>;
    using I = tmpasmWrapper<Ins>;
    
    constexpr static std::size_t decN = ivd ? T::decN + 1 : T::decN;
    constexpr static List<Declaration> declarations = ivd ? List<Declaration>{I::d, &T::declarations} : T::declarations;
    constexpr static std::size_t insN = ii ? T::insN + 1 : T::insN;
    constexpr static List<Instruction> instructions = ii ? List<Instruction>{I::ins, &T::instructions} : T::instructions;
    constexpr static std::size_t labN = ild ? T::labN + 1 : T::labN;
    constexpr static List<LabelDeclaration> labels = ild ? List<LabelDeclaration>{{I::id, insN}, &T::labels} : T::labels;
};


template <typename T>
struct isTMPAsmProgram{
    constexpr static bool value = false;
};

template <typename ... Ins>
struct isTMPAsmProgram<Program<Ins...>>{
    constexpr static bool value = true;
};

// Computer class
template<std::size_t MemSize, typename WordType>
class Computer{
    private:

    using MemoryType = std::array<WordType, MemSize>;
    
    struct DeclarationType{
        WordType initVal;
        int64_t id;
    };

    struct LabelType{
        std::size_t instructionAddr;
        int64_t id;
    };

    template <typename Prog>
    constexpr static std::array<DeclarationType, Prog::decN> toVariablesArray(){
        std::array<DeclarationType, Prog::decN> arr = {};
        
        List<Declaration> curr = Prog::declarations;
        for(std::size_t i = 0; i < Prog::decN; i++){
            arr[i] = {(WordType) curr.value.val, curr.value.id};
            curr = *curr.next;
        }

        return arr;
    }

    template <typename Prog>
    constexpr static std::array<LabelType, Prog::labN> toLabelsArray(){
        std::array<LabelType, Prog::labN> arr = {};

        List<LabelDeclaration> curr = Prog::labels;
        for(std::size_t i = 0; i < Prog::labN; i++){
            arr[i] = {Prog::insN - curr.value.instructionNumber, curr.value.id};
            curr = *curr.next;
        }

        return arr;
    }

    template <typename Prog>
    constexpr static std::array<Instruction, Prog::insN> toInstructionsArray(){
        std::array<Instruction, Prog::insN> arr = {};

        List<Instruction> curr = Prog::instructions;
        for(std::size_t i = 0; i < Prog::insN; i++){
            arr[i] = curr.value;
            curr = *curr.next;
        }

        return arr;
    }


    template <std::size_t decN, std::size_t labN>
    struct RunTime{
        std::array<DeclarationType, decN> variables;
        std::array<LabelType, labN> labels;

        WordType tmp = 0;
        MemoryType memory = {};
        bool SF = false;
        bool ZF = false;
        std::size_t PC = 0;

        constexpr RunTime(const std::array<DeclarationType, decN>& variables, const std::array<LabelType, labN>& labels): variables(variables), labels(labels){
            // Initializing variables
            for(std::size_t i = 0; i < decN && variables[i].id != 0; i++){
                memory[i] = variables[i].initVal;
            }
        };

        constexpr std::size_t getVarAddr(int64_t id){
            for(std::size_t i = 0; i < decN && variables[i].id != 0; i++){
                if(variables[i].id == id){
                    return i;
                }
            }
            // This will break constexpr and report error
            throw "Access to nonexisting variable";
        }

        constexpr std::size_t getLabelAddr(int64_t id){
            for(std::size_t i = 0; i < labN && labels[i].id != 0; i++){
                if(labels[i].id == id){
                    return labels[i].instructionAddr;
                }
            }
            // This will break constexpr and report error
            throw "Access to nonexisting label";
        }

        // Fetches reference to correct value in memory or to temporary variable if v is an r-value
        constexpr WordType& getV(Variable v){
            if(v.id){
                std::size_t raw = getVarAddr(v.val);
                if(v.dereferences == 0){
                    tmp = raw;
                    return tmp;
                }
                if(v.dereferences == 1){
                    return memory[raw];
                }
                tmp = memory[raw];
            } else {
                tmp = v.val;
                if(v.dereferences == 0){
                    return tmp;
                }
                if(v.dereferences == 1){
                    return memory[tmp];
                }
                tmp = memory[tmp];
            }

            for(uint64_t i = 0; i < v.dereferences - 2; i++){
                tmp = memory[tmp];
            }
            return memory[tmp];
        }

        constexpr void execute(Instruction ins){
            using IT = Instruction::InstructionType;

            switch(ins.type){
                case IT::_MOV:{
                    WordType& left = getV(ins.arg1);
                    left = getV(ins.arg2);
                    break;
                }
                case IT::_ADD:{
                    WordType& left = getV(ins.arg1);
                    left += getV(ins.arg2);
                    ZF = left == 0;
                    SF = left < 0;
                    break;
                }
                case IT::_SUB:{
                    WordType& left = getV(ins.arg1);
                    left -= getV(ins.arg2);
                    ZF = left == 0;
                    SF = left < 0;
                    break;
                }
                case IT::_CMP:{
                    WordType cpy = getV(ins.arg1);
                    cpy -= getV(ins.arg2);
                    ZF = cpy == 0;
                    SF = cpy < 0;
                    break;
                }
                case IT::_INC:{
                    WordType& left = getV(ins.arg1);
                    left += 1;
                    ZF = left == 0;
                    SF = left < 0;
                    break;
                }
                case IT::_DEC:{
                    WordType& left = getV(ins.arg1);
                    left -= 1;
                    ZF = left == 0;
                    SF = left < 0;
                    break;
                }
                case IT::_AND:{
                    WordType& left = getV(ins.arg1);
                    left &= getV(ins.arg2);
                    ZF = left == 0;
                    break;
                }
                case IT::_OR:{
                    WordType& left = getV(ins.arg1);
                    left |= getV(ins.arg2);
                    ZF = left == 0;
                    break;
                }
                case IT::_NOT:{
                    WordType& left = getV(ins.arg1);
                    left = ~left;
                    ZF = left == 0;
                    break;
                }
                default:{
                    break;
                }
            }

            switch(ins.type){
                case IT::_JMP:{
                    PC = getLabelAddr(ins.labelId);
                    break;
                }
                case IT::_JS:{
                    // We split this so getLabelAddr will always run.
                    // This checks for jumps to undefined labels
                    std::size_t addr = getLabelAddr(ins.labelId);
                    PC = SF ? addr : PC+1;
                    break;
                }
                case IT::_JZ:{
                    std::size_t addr = getLabelAddr(ins.labelId);
                    PC = ZF ? addr : PC+1;
                    break;
                }
                default:{
                    PC++;
                    break;
                }
            }
        }
    };

    public:
    template <typename Prog>
    constexpr static MemoryType boot(){
        static_assert(isTMPAsmProgram<Prog>::value, "Computer must be booted with TMPAsm Program");

        std::array<Instruction, Prog::insN> instructions = toInstructionsArray<Prog>();
        
        RunTime<Prog::decN, Prog::labN> rt = {toVariablesArray<Prog>(), toLabelsArray<Prog>()};

        while(rt.PC < instructions.size()){
            rt.execute(instructions[rt.PC]);
        }

        return rt.memory;
    }
};
#endif /* COMPUTER_H */