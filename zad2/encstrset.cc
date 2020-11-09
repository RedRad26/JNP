#include "encstrset.h"

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>

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

ostream& err() {
    static std::ios_base::Init init;
    return std::cerr;
}

#define error_stream if (!_debug) {} else err()

ostream& operator<<(ostream& stream, const char ** c){
    if(*c){
        stream << '"' << *c << '"';
    } else {
        stream << "NULL";
    }
    return stream;
}

unsigned long freeId = 0;

// Alias for c++ strlen() equivalent
const auto& CStringLength = std::char_traits<char>::length;

// Performs encryption. Assumes not null value.
string xorEncrypt(const char* value, const char* key){
    string encrypted;
    if(key == NULL || *key == '\0'){
        encrypted  = value;
        return encrypted;
    }
    size_t valueLength = CStringLength(value);
    size_t keyLength = CStringLength(key);

    for(size_t i = 0; i < valueLength; i++){
        encrypted += value[i] ^ key[i%keyLength];
    }
    return encrypted;
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
    error_stream << "encstrset_new" << "()" << endl;

    StringSet s; 
    setCollection()[freeId] = s;

    error_stream << "encstrset_new: set #" << freeId << " created" << endl;
    return freeId++;
}

void jnp1::encstrset_delete(unsigned long id){
    error_stream << "encstrset_delete" << "(" << id << ")" << endl;

    if(setCollection().erase(id) > 0){
        error_stream << "encstrset_delete: set #" << id << " deleted" << endl;
    } else {
        error_stream << "encstrset_insert: set #" << id << " does not exist" << endl;
    }
    
}

size_t jnp1::encstrset_size(unsigned long id){
    error_stream << "encstrset_size" << "(" << id << ")" << endl;

    auto setIt = setCollection().find(id);
    if(setIt == setCollection().end()){
        error_stream << "encstrset_size: set #" << id << " does not exist" << endl;
        return 0;
    }

    size_t size = setIt->second.size();
    error_stream << "encstrset_size: set #" << id << " contains " << size << " element(s)" << endl;
    return size;
}

bool jnp1::encstrset_insert(unsigned long id, const char* value, const char* key){
    error_stream << "encstrset_insert" << "(" << id << ", " << &value << ", " << &key << ")" << endl;

    auto setIt = setCollection().find(id);    
    if(setIt == setCollection().end()){
        error_stream << "encstrset_insert: set #" << id << " does not exist" << endl;
        return false;
    }

    if(value == nullptr){
        error_stream << "encstrset_insert: invalid value (NULL)" << endl;
        return false;
    }

    StringSet& set = setIt->second;
    string encrypted = xorEncrypt(value, key);

    if(std::get<1>(set.insert(encrypted))){
        error_stream << "encstrset_insert: set #" << id << ", cypher " << printCypher(encrypted) << " inserted" << endl;
        return true;
    } else {
        error_stream << "encstrset_insert: set #" << id << ", cypher " << printCypher(encrypted) << " was already present" << endl;
        return false;
    }
}

bool jnp1::encstrset_remove(unsigned long id, const char* value, const char* key){
    error_stream << "encstrset_remove" << "(" << id << ", " << &value << ", " << &key << ")" << endl;
    
    auto setIt = setCollection().find(id);  
    if(setIt == setCollection().end()){
        error_stream << "encstrset_remove: set #" << id << " does not exist" << endl;
        return false;
    }

    if(value == nullptr){
        error_stream << "encstrset_remove: invalid value (NULL)" << endl;
        return false;
    }

    StringSet& set = setIt->second;
    string encrypted = xorEncrypt(value, key);

    if(set.erase(encrypted) > 0){
        error_stream << "encstrset_remove: set #" << id << ", cypher " << printCypher(encrypted) << " removed" << endl;
        return true;
    } else {
        error_stream << "encstrset_remove: set #" << id << ", cypher " << printCypher(encrypted) << " was not present" << endl;
        return false;
    }
}

bool jnp1::encstrset_test(unsigned long id, const char* value, const char* key){
    error_stream << "encstrset_test" << "(" << id << ", " << &value << ", " << &key << ")" << endl;
    
    auto setIt = setCollection().find(id);  
    if(setIt == setCollection().end()){
        error_stream << "encstrset_test: set #" << id << " does not exist" << endl;
        return false;
    }

    if(value == nullptr){
        error_stream << "encstrset_test: invalid value (NULL)" << endl;
        return false;
    }

    StringSet& set = setIt->second;
    string encrypted = xorEncrypt(value, key);

    if(set.find(encrypted) != set.end()){
        error_stream << "encstrset_test: set #" << id << ", cypher " << printCypher(encrypted) << " is present" << endl;
        return true;
    } else {
        error_stream << "encstrset_test: set #" << id << ", cypher " << printCypher(encrypted) << " is not present" << endl;
        return false;
    }
}

void jnp1::encstrset_clear(unsigned long id){
    error_stream << "encstrset_clear" << "(" << id << ")" << endl;

    auto setIt = setCollection().find(id);
    if(setIt == setCollection().end()){
        error_stream << "encstrset_clear: set #" << id << " does not exist" << endl;
        return;
    }

    setIt->second.clear();
    error_stream << "encstrset_clear: set #" << id << " cleared" << endl;
}

void jnp1::encstrset_copy(unsigned long src_id, unsigned long dst_id){
    error_stream << "encstrset_copy" << "(" << src_id << ", " << dst_id << ")" << endl;

    auto srcSetIt = setCollection().find(src_id);
    if(srcSetIt == setCollection().end()){
        error_stream << "encstrset_copy: set #" << src_id << " does not exist" << endl;
        return;
    }

    auto dstSetIt = setCollection().find(dst_id);
    if(dstSetIt == setCollection().end()){
        error_stream << "encstrset_copy: set #" << dst_id << " does not exist" << endl;
        return;
    }

    StringSet& srcSet = srcSetIt->second;
    StringSet& dstSet = dstSetIt->second;
    for(const string& s: srcSet){
        if(std::get<1>(dstSet.insert(s))){
            error_stream << "encstrset_copy: cypher " << printCypher(s) << " copied from set #" << src_id << " to set #" << dst_id << endl;
        } else {
            error_stream << "encstrset_copy: copied cypher " << printCypher(s) << " was already present in set #" << dst_id << endl;
        }
    }
}