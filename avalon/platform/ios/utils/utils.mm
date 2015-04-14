#include "utils.h"

namespace avalon {
namespace utils {

id valueToId(const Value &value)
{
    switch (value.getType()) {
        case Value::Type::INTEGER:
            return [NSNumber numberWithInt:value.asInt()];
            break;
            
        case Value::Type::FLOAT:
            return [NSNumber numberWithFloat:value.asFloat()];
            break;
            
        case Value::Type::DOUBLE:
            return [NSNumber numberWithDouble:value.asDouble()];
            break;
            
        case Value::Type::BOOLEAN:
            return [NSNumber numberWithBool:value.asBool()];
            break;
            
        case Value::Type::STRING:
            return [NSString stringWithUTF8String:value.asString().c_str()];
            break;
            
        case Value::Type::VECTOR:
        {
            NSMutableArray * array = [NSMutableArray array];
            const ValueVector &vector = value.asValueVector();
            for(const auto &it:vector)
                [array addObject:valueToId(it)];
            return array;
        }
            break;
            
        case Value::Type::MAP:
        {
            NSMutableDictionary * dictionary = [NSMutableDictionary dictionary];
            const ValueMap &map = value.asValueMap();
            for(const auto &it:map)
                [dictionary setObject:valueToId(it.second) forKey:[NSString stringWithUTF8String:it.first.c_str()]];
            return dictionary;
        }
            break;
            
        default:
            return nil;
            break;
    }
}

}
}
