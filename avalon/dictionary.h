#ifndef AVALON_UTILS_DICTIONARY_H
#define AVALON_UTILS_DICTIONARY_H

#include <string>
#include <vector>
#include <unordered_map>

namespace avalon {
namespace utils {

class Value;

typedef std::vector<Value> ValueVector;
typedef std::unordered_map<std::string, Value> ValueMap;

class Value
{
public:
    static const Value Null;
    
    Value();
    explicit Value(int v);
    explicit Value(float v);
    explicit Value(double v);
    explicit Value(bool v);
    explicit Value(const char* v);
    explicit Value(const std::string& v);
    
    explicit Value(const ValueVector& v);
    explicit Value(ValueVector&& v);
    
    explicit Value(const ValueMap& v);
    explicit Value(ValueMap&& v);
    
    Value(const Value& other);
    Value(Value&& other);
    ~Value();
    
    // assignment operator
    Value& operator= (const Value& other);
    Value& operator= (Value&& other);
    
    Value& operator= (int v);
    Value& operator= (float v);
    Value& operator= (double v);
    Value& operator= (bool v);
    Value& operator= (const char* v);
    Value& operator= (const std::string& v);
    
    Value& operator= (const ValueVector& v);
    Value& operator= (ValueVector&& v);
    
    Value& operator= (const ValueMap& v);
    Value& operator= (ValueMap&& v);
    
    // equal operator
    bool operator!= (const Value& v);
    bool operator!= (const Value& v) const;
    bool operator== (const Value& v);
    bool operator== (const Value& v) const;
    
    int asInt() const;
    float asFloat() const;
    double asDouble() const;
    bool asBool() const;
    std::string asString() const;
    
    ValueVector& asValueVector();
    const ValueVector& asValueVector() const;
    
    ValueMap& asValueMap();
    const ValueMap& asValueMap() const;;
    
    inline bool isNull() const { return _type == Type::NONE; }
    
    enum class Type
    {
        NONE = 0,
        INTEGER,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        STRING,
        VECTOR,
        MAP
    };
    
    inline Type getType() const { return _type; };
    
    std::string getDescription();
    
private:
    void clear();
    void reset(Type type);
    
    union
    {
        int intVal;
        float floatVal;
        double doubleVal;
        bool boolVal;
        
        std::string* strVal;
        ValueVector* vectorVal;
        ValueMap* mapVal;
    }_field;
    
    Type _type;
};
    
} // namespace utils
} // namespace avalon

#endif /* AVALON_UTILS_DICTIONARY_H */
