//
//  dictionary.cpp
//

#include "avalon/dictionary.h"
#include <float.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <iomanip>

namespace avalon {
namespace utils {
    
Value::Value()
: _type(Type::NONE)
{
    memset(&_field, 0, sizeof(_field));
}

Value::Value(int v)
: _type(Type::INTEGER)
{
    _field.intVal = v;
}
    
Value::Value(long v)
    : _type(Type::INTEGER)
{
    _field.intVal = v;
}
Value::Value(long long v)
    : _type(Type::INTEGER)
{
    _field.intVal = v;
}

Value::Value(float v)
: _type(Type::FLOAT)
{
    _field.floatVal = v;
}

Value::Value(double v)
: _type(Type::DOUBLE)
{
    _field.doubleVal = v;
}

Value::Value(bool v)
: _type(Type::BOOLEAN)
{
    _field.boolVal = v;
}

Value::Value(const char* v)
: _type(Type::STRING)
{
    _field.strVal = new std::string();
    if (v)
    {
        *_field.strVal = v;
    }
}

Value::Value(const std::string& v)
: _type(Type::STRING)
{
    _field.strVal = new std::string();
    *_field.strVal = v;
}

Value::Value(const ValueVector& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueVector();
    *_field.vectorVal = v;
}

Value::Value(ValueVector&& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) ValueVector();
    *_field.vectorVal = std::move(v);
}

Value::Value(const ValueMap& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueMap();
    *_field.mapVal = v;
}

Value::Value(ValueMap&& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) ValueMap();
    *_field.mapVal = std::move(v);
}

Value::Value(const Value& other)
: _type(Type::NONE)
{
    *this = other;
}

Value::Value(Value&& other)
: _type(Type::NONE)
{
    *this = std::move(other);
}

Value::~Value()
{
    clear();
}

Value& Value::operator= (const Value& other)
{
    if (this != &other) {
        reset(other._type);
        
        switch (other._type) {
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                if (_field.strVal == nullptr)
                {
                    _field.strVal = new std::string();
                }
                *_field.strVal = *other._field.strVal;
                break;
            case Type::VECTOR:
                if (_field.vectorVal == nullptr)
                {
                    _field.vectorVal = new (std::nothrow) ValueVector();
                }
                *_field.vectorVal = *other._field.vectorVal;
                break;
            case Type::MAP:
                if (_field.mapVal == nullptr)
                {
                    _field.mapVal = new (std::nothrow) ValueMap();
                }
                *_field.mapVal = *other._field.mapVal;
                break;
            default:
                break;
        }
    }
    return *this;
}

Value& Value::operator= (Value&& other)
{
    if (this != &other)
    {
        clear();
        switch (other._type)
        {;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                _field.strVal = other._field.strVal;
                break;
            case Type::VECTOR:
                _field.vectorVal = other._field.vectorVal;
                break;
            case Type::MAP:
                _field.mapVal = other._field.mapVal;
                break;
            default:
                break;
        }
        _type = other._type;
        
        memset(&other._field, 0, sizeof(other._field));
        other._type = Type::NONE;
    }
    
    return *this;
}

Value& Value::operator= (int v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}
    
Value& Value::operator= (long v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}

Value& Value::operator= (long long v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}

Value& Value::operator= (float v)
{
    reset(Type::FLOAT);
    _field.floatVal = v;
    return *this;
}

Value& Value::operator= (double v)
{
    reset(Type::DOUBLE);
    _field.doubleVal = v;
    return *this;
}

Value& Value::operator= (bool v)
{
    reset(Type::BOOLEAN);
    _field.boolVal = v;
    return *this;
}

Value& Value::operator= (const char* v)
{
    reset(Type::STRING);
    *_field.strVal = v ? v : "";
    return *this;
}

Value& Value::operator= (const std::string& v)
{
    reset(Type::STRING);
    *_field.strVal = v;
    return *this;
}

Value& Value::operator= (const ValueVector& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = v;
    return *this;
}

Value& Value::operator= (ValueVector&& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = std::move(v);
    return *this;
}

Value& Value::operator= (const ValueMap& v)
{
    reset(Type::MAP);
    *_field.mapVal = v;
    return *this;
}

Value& Value::operator= (ValueMap&& v)
{
    reset(Type::MAP);
    *_field.mapVal = std::move(v);
    return *this;
}

bool Value::operator!= (const Value& v)
{
    return !(*this == v);
}
bool Value::operator!= (const Value& v) const
{
    return !(*this == v);
}

bool Value::operator== (const Value& v)
{
    const auto &t = *this;
    return t == v;
}
bool Value::operator== (const Value& v) const
{
    if (this == &v) return true;
    if (v._type != this->_type) return false;
    if (this->isNull()) return true;
    switch (_type)
    {
        case Type::INTEGER: return v._field.intVal    == this->_field.intVal;
        case Type::BOOLEAN: return v._field.boolVal   == this->_field.boolVal;
        case Type::STRING:  return *v._field.strVal   == *this->_field.strVal;
        case Type::FLOAT:   return fabs(v._field.floatVal  - this->_field.floatVal)  <= FLT_EPSILON;
        case Type::DOUBLE:  return fabs(v._field.doubleVal - this->_field.doubleVal) <= FLT_EPSILON;
        case Type::VECTOR:
        {
            const auto &v1 = *(this->_field.vectorVal);
            const auto &v2 = *(v._field.vectorVal);
            const auto size = v1.size();
            if (size == v2.size())
            {
                for (size_t i = 0; i < size; i++)
                {
                    if (v1[i] != v2[i]) return false;
                }
            }
            return true;
        }
        case Type::MAP:
        {
            const auto &map1 = *(this->_field.mapVal);
            const auto &map2 = *(v._field.mapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        default:
            break;
    };
    
    return false;
}

int Value::asInt() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return static_cast<int>(_field.intVal);
    }
    
    if (_type == Type::STRING)
    {
        return atoi(_field.strVal->c_str());
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<int>(_field.floatVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<int>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }
    
    return 0;
}
    
long Value::asLong() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return static_cast<long>(_field.intVal);
    }
    
    if (_type == Type::STRING)
    {
        return atol(_field.strVal->c_str());
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<long>(_field.floatVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<int>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }
    
    return 0;
}
    
long long Value::asLongLong() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return static_cast<long long>(_field.intVal);
    }
    
    if (_type == Type::STRING)
    {
        return atoll(_field.strVal->c_str());
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<long long>(_field.floatVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<long long>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }
    
    return 0;
}

float Value::asFloat() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::FLOAT)
    {
        return _field.floatVal;
    }
    
    if (_type == Type::STRING)
    {
        return atof(_field.strVal->c_str());
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<float>(_field.intVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<float>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0f : 0.0f;
    }
    
    return 0.0f;
}

double Value::asDouble() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal;
    }
    
    if (_type == Type::STRING)
    {
        return static_cast<double>(atof(_field.strVal->c_str()));
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<double>(_field.intVal);
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<double>(_field.floatVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0 : 0.0;
    }
    
    return 0.0;
}

bool Value::asBool() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal;
    }
    
    if (_type == Type::STRING)
    {
        return (*_field.strVal == "0" || *_field.strVal == "false") ? false : true;
    }
    
    if (_type == Type::INTEGER)
    {
        return _field.intVal == 0 ? false : true;
    }
    
    if (_type == Type::FLOAT)
    {
        return _field.floatVal == 0.0f ? false : true;
    }
    
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal == 0.0 ? false : true;
    }
    
    return false;
}

std::string Value::asString() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    
    if (_type == Type::STRING)
    {
        return *_field.strVal;
    }
    
    std::stringstream ret;
    
    switch (_type)
    {
        case Type::INTEGER:
            ret << _field.intVal;
            break;
        case Type::FLOAT:
            ret << std::fixed << std::setprecision( 7 )<< _field.floatVal;
            break;
        case Type::DOUBLE:
            ret << std::fixed << std::setprecision( 16 ) << _field.doubleVal;
            break;
        case Type::BOOLEAN:
            ret << (_field.boolVal ? "true" : "false");
            break;
        default:
            break;
    }
    return ret.str();
}

ValueVector& Value::asValueVector()
{
    assert(_type == Type::VECTOR && "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

const ValueVector& Value::asValueVector() const
{
    assert(_type == Type::VECTOR && "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

ValueMap& Value::asValueMap()
{
    assert(_type == Type::MAP && "The value type isn't Type::MAP");
    return *_field.mapVal;
}

const ValueMap& Value::asValueMap() const
{
    assert(_type == Type::MAP && "The value type isn't Type::MAP");
    return *_field.mapVal;
}

void Value::clear()
{
    // Free memory the old value allocated
    switch (_type)
    {
        case Type::INTEGER:
            _field.intVal = 0;
            break;
        case Type::FLOAT:
            _field.floatVal = 0.0f;
            break;
        case Type::DOUBLE:
            _field.doubleVal = 0.0;
            break;
        case Type::BOOLEAN:
            _field.boolVal = false;
            break;
        case Type::STRING:
            delete _field.strVal;
            _field.strVal = nullptr;
            break;
        case Type::VECTOR:
            delete _field.vectorVal;
            _field.vectorVal = nullptr;
            break;
        case Type::MAP:
            delete _field.mapVal;
            _field.mapVal = nullptr;
            break;
        default:
            break;
    }
    
    _type = Type::NONE;
}

void Value::reset(Type type)
{
    if (_type == type)
        return;
    
    clear();
    
    // Allocate memory for the new value
    switch (type)
    {
        case Type::STRING:
            _field.strVal = new std::string();
            break;
        case Type::VECTOR:
            _field.vectorVal = new (std::nothrow) ValueVector();
            break;
        case Type::MAP:
            _field.mapVal = new (std::nothrow) ValueMap();
            break;
        default:
            break;
    }
    
    _type = type;
}

} // namespace utils
} // namespace avalon
