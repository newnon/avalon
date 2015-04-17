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
    
Value idTovalue(id value)
{
    if ([value isKindOfClass:[NSNumber class]])
    {
        NSNumber *val = (NSNumber*)value;
        if (strcmp([val objCType], @encode(BOOL)) == 0) {
            return Value([val boolValue]);
        } else if (strcmp([val objCType], @encode(int)) == 0 || strcmp([val objCType], @encode(long)) == 0 || strcmp([val objCType], @encode(short)) == 0) {
            return Value([val intValue]);
        } else if (strcmp([val objCType], @encode(float)) == 0) {
            return Value([val floatValue]);
        } else if (strcmp([val objCType], @encode(double)) == 0) {
            return Value([val doubleValue]);
        }
    }
    else if ([value isKindOfClass:[NSString class]])
    {
        NSString *val = (NSString*)value;
        return Value([val UTF8String]);
    }
    else if ([value isKindOfClass:[NSArray class]])
    {
        NSArray *val = (NSArray*)value;
        __block ValueVector vector;
        for (id object in val) {
            vector.push_back(idTovalue(object));
        }
        return Value(vector);
    }
    else if ([value isKindOfClass:[NSDictionary class]])
    {
        NSDictionary *val = (NSDictionary*)value;
        __block ValueMap map;
        [val enumerateKeysAndObjectsUsingBlock:^(id key, id val, BOOL* stop) {
            map[std::string([[key stringValue] UTF8String])] = idTovalue(val);
        }];
        return Value(map);
    }

    return Value();
}

}
}
