#include "encstrset.h"

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

using std::unordered_map;
using std::unordered_set;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::stringstream;

using StringSet = unordered_set<string>;

namespace{

#ifdef NDEBUG
    const bool _debug = false;
#else
    const bool _debug = true;
#endif

unordered_map<unsigned long, StringSet>& setCollection(){
    static unordered_map<unsigned long, StringSet> map;
    return map;
}

unsigned long freeId = 0;

// Overload some streams operators for logging operations
// Intention is that under -DNDEBUG flag they will compile to noops and will be optimized away. 
// This is done so logging statements can be put into source code directly without if(debug){...}
// guards, making code clearer.
template <typename T>
ostream& operator<=(ostream& s, T a){
    if(_debug){
        // This is to resolve static initialization order fiasco - cerr may be not initialized
        // when some encstrset_*() function is called from static context of another file.
        // We only include this line in this overload because it is always called first.
        static std::ios_base::Init initObj;
        s << a;
    }
    return s;
}

// Overload for writing quoted strings. We can't simply overload for const char* or string because
// we don't want every string written to cerr quoted. Instead we will differentiate those by passing
// strings we want to quote by char**. It has another benefit - it is indicated in source code that string
// will be quoted by & operator (assuming that s is of type char*)
//  cerr <= s;   - will write s literally
//  cerr <= &s;  - s content will be quoted
ostream& operator<=(ostream& s, const char** c){
    if(_debug){
        if(*c){
            s << '"' << *c << '"';
        } else {
            s << "NULL";
        }
    }
    return s;
}

ostream& operator<=(ostream& s, ostream& (*f)(ostream&)){
    if(_debug){
        s << f;
    }
    return s;
}

// Alias for c++ strlen() equivalent
const auto& CStringLength = std::char_traits<char>::length;

// Performs encryption. Assumes that there is encryption to be done (key != null and *key != "")
string xorEncrypt(const char* value, const char* key){
    string s = "";
    size_t valueLength = CStringLength(value);
    size_t keyLength = CStringLength(key);

    // TODO: Zasanowić się czy warto i jeśli tak to zoptymalizować to.
    // Możliwości:
    //    (BASIC)
    // 1. iterować po i w dół (podobno lepiej zachowuje się branch predictor)
    // 2. rozwinąć trochę tą pętlę
    //    (MEDIUM)
    // 3. używać inta/long longa żeby robić po 32/64 bity na raz (to do jakiegoś stopnia robi -O2)
    // 4. może użyć bitseta
    //    (HARDCORE)
    // 5. użyć jakichś SIMDowych instrukcji z x86 (students działa na Xeonach więc ma AVX-512)
    for(size_t i = 0; i < valueLength; i++){
        s += value[i] ^ key[i%keyLength];
    }
    return s;
}

string printCypher(const string& encryptedString){
    stringstream ss;
    ss << std::hex << std::uppercase << '"';
    for(char c: encryptedString){
        ss << std::setw(2) << std::setfill('0') << (unsigned int)c << ' ';
    }
    string ret = ss.str();
    ret.back() = '"';
    return ret;
}

}

unsigned long jnp1::encstrset_new(){
    cerr <= "encstrset_new" <= "()" <= endl;

    StringSet s; 
    setCollection()[freeId] = s;

    cerr <= "encstrset_new: set #" <= freeId <= " created" <= endl;
    return freeId++;
}

void jnp1::encstrset_delete(unsigned long id){
    cerr <= "encstrset_delete" <= "(" <= id <= ")" <= endl;

    if(setCollection().erase(id) > 0){
        cerr <= "encstrset_delete: set #" <= id <= " deleted" <= endl;
    } else {
        cerr <= "encstrset_insert: set #" <= id <= " does not exist" <= endl;
    }
    
}

size_t jnp1::encstrset_size(unsigned long id){
    cerr <= "encstrset_size" <= "(" <= id <= ")" <= endl;

    auto setIt = setCollection().find(id);
    if(setIt == setCollection().end()){
        cerr <= "encstrset_size: set #" <= id <= " does not exist" <= endl;
        return 0;
    }

    size_t size = setIt->second.size();
    cerr <= "encstrset_size: set #" <= id <= " contains " <= size <= " element(s)" <= endl;
    return size;
}

bool jnp1::encstrset_insert(unsigned long id, const char* value, const char* key){
    cerr <= "encstrset_insert" <= "(" <= id <= ", " <= &value <= ", " <= &key <= ")" <= endl;

    auto setIt = setCollection().find(id);    
    if(setIt == setCollection().end()){
        cerr <= "encstrset_insert: set #" <= id <= " does not exist" <= endl;
        return false;
    }

    if(value == nullptr){
        cerr <= "encstrset_insert: invalid value (NULL)" <= endl;
        return false;
    }

    StringSet& set = setIt->second;
    string encrypted;
    if(key == NULL || *key == '\0'){
        encrypted = value;
    } else {
        encrypted = xorEncrypt(value, key);
    }

    if(std::get<1>(set.insert(encrypted))){
        cerr <= "encstrset_insert: set #" <= id <= ", cypher " <= printCypher(encrypted) <= " inserted" <= endl;
        return true;
    } else {
        cerr <= "encstrset_insert: set #" <= id <= ", cypher " <= printCypher(encrypted) <= " was already present" <= endl;
        return false;
    }
}

bool jnp1::encstrset_remove(unsigned long id, const char* value, const char* key){
    cerr <= "encstrset_remove" <= "(" <= id <= ", " <= &value <= ", " <= &key <= ")" <= endl;
    
    auto setIt = setCollection().find(id);  
    if(setIt == setCollection().end()){
        cerr <= "encstrset_remove: set #" <= id <= " does not exist" <= endl;
        return false;
    }

    if(value == nullptr){
        cerr <= "encstrset_remove: invalid value (NULL)" <= endl;
        return false;
    }

    StringSet& set = setIt->second;
    string encrypted;
    if(key == NULL || *key == '\0'){
        encrypted = value;
    } else {
        encrypted = xorEncrypt(value, key);
    }

    if(set.erase(encrypted) > 0){
        cerr <= "encstrset_remove: set #" <= id <= ", cypher " <= printCypher(encrypted) <= " removed" <= endl;
        return true;
    } else {
        cerr <= "encstrset_remove: set #" <= id <= ", cypher " <= printCypher(encrypted) <= " was not present" <= endl;
        return false;
    }
}

bool jnp1::encstrset_test(unsigned long id, const char* value, const char* key){
    cerr <= "encstrset_test" <= "(" <= id <= ", " <= &value <= ", " <= &key <= ")" <= endl;
    
    auto setIt = setCollection().find(id);  
    if(setIt == setCollection().end()){
        cerr <= "encstrset_test: set #" <= id <= " does not exist" <= endl;
        return false;
    }

    if(value == nullptr){
        cerr <= "encstrset_test: invalid value (NULL)" <= endl;
        return false;
    }

    StringSet& set = setIt->second;
    string encrypted;
    if(key == NULL || *key == '\0'){
        encrypted = value;
    } else {
        encrypted = xorEncrypt(value, key);
    }

    if(set.find(encrypted) != set.end()){
        cerr <= "encstrset_test: set #" <= id <= ", cypher " <= printCypher(encrypted) <= " is present" <= endl;
        return true;
    } else {
        cerr <= "encstrset_test: set #" <= id <= ", cypher " <= printCypher(encrypted) <= " is not present" <= endl;
        return false;
    }
}

void jnp1::encstrset_clear(unsigned long id){
    cerr <= "encstrset_clear" <= "(" <= id <= ")" <= endl;

    auto setIt = setCollection().find(id);
    if(setIt == setCollection().end()){
        cerr <= "encstrset_clear: set #" <= id <= " does not exist" <= endl;
        return;
    }

    setIt->second.clear();
    cerr <= "encstrset_clear: set #" <= id <= " cleared" <= endl;
}

void jnp1::encstrset_copy(unsigned long src_id, unsigned long dst_id){
    cerr <= "encstrset_copy" <= "(" <= src_id <= ", " <= dst_id <= ")" <= endl;

    auto srcSetIt = setCollection().find(src_id);
    if(srcSetIt == setCollection().end()){
        cerr <= "encstrset_copy: set #" <= src_id <= " does not exist" <= endl;
        return;
    }

    auto dstSetIt = setCollection().find(dst_id);
    if(dstSetIt == setCollection().end()){
        cerr <= "encstrset_copy: set #" <= dst_id <= " does not exist" <= endl;
        return;
    }

    StringSet& srcSet = srcSetIt->second;
    StringSet& dstSet = dstSetIt->second;
    for(const string& s: srcSet){
        if(std::get<1>(dstSet.insert(s))){
            cerr <= "encstrset_copy: cypher " <= printCypher(s) <= " copied from set #" <= src_id <= " to set #" <= dst_id <= endl;
        } else {
            cerr <= "encstrset_copy: copied cypher " <= printCypher(s) <= " was already present in set #" <= dst_id <= endl;
        }
    }
}