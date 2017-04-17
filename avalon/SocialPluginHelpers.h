//
//  SocialPluginPrivate.h
//  avalon
//
//  Created by Sergey on 17.04.17.
//  Copyright © 2017 Sergey. All rights reserved.
//

#ifndef SocialPluginHelpers_h
#define SocialPluginHelpers_h

#include "SocialPlugin.h"
#include <map>

namespace avalon {
template <class T>
class PlatformSocialPermission:public SocialPermission {
public:
    typedef T Type;
    PlatformSocialPermission(SocialPermission::Type type):SocialPermission(type) {}
    PlatformSocialPermission(T type):SocialPermission(SocialPermission::Type::PLATFORM, static_cast<int>(type)) {}
    PlatformSocialPermission(SocialPermission permission):SocialPermission(permission.getType(), permission.getPlatformType()) {}
};

template <class T>
class SocialPermissionsHelper
{
public:
    SocialPermissionsHelper(const std::vector<std::pair<T, std::string>> &values)
    {
        for(const auto &pair:values)
        {
            _valueToString.emplace(pair.first, pair.second);
            _stringToValue.emplace(pair.second, pair.first);
        }
    }
    SocialPermission fromString(const std::string &value) const {
        auto it = _stringToValue.find(value);
        return it == _stringToValue.end()?T(SocialPermission::Type::NONE):it->second;
    }
    const std::string& toString(SocialPermission value) const {
        auto it = _valueToString.find(value);
        return it == _valueToString.end()?empty:it->second;
    }
private:
    std::map<T, std::string> _valueToString;
    std::map<std::string, T> _stringToValue;
    static const std::string empty;
};

template <class T>
const std::string SocialPermissionsHelper<T>::empty;

class GenderHelper
{
public:
    GenderHelper(const std::string &male, const std::string &female, const std::string &unknown)
    {
        _stringToValue[male] = SocialProfile::Gender::MALE;
        _stringToValue[female] = SocialProfile::Gender::FEMALE;
        _stringToValue[unknown] = SocialProfile::Gender::UNKNOWN;
    }
    SocialProfile::Gender fromString(const std::string &value) const
    {
        auto it = _stringToValue.find(value);
        return it == _stringToValue.end()?SocialProfile::Gender::OTHER:it->second;
    }
private:
    std::map<std::string, SocialProfile::Gender> _stringToValue;
};

/* Sample code
enum class TestPermissionType
{
    NONE = 0,
    TEST
};
using TestPermission = PlatformSocialPermission<TestPermissionType>;
 
SocialPermissionsHelper<TestPermission> testPermissionsHelper({{SocialPermission::Type::EMAIL, "email"}, {TestPermission::Type::TEST, "test"}});
 
GenderHelper testGenderHelper("m", "f", "");
*/
}

#endif /* SocialPluginHelpers_h */
