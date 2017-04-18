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

template <class T>
class GenderHelper
{
public:
    GenderHelper(const T &male, const T &female, const T &unknown)
    {
        _toGenderMap[male] = SocialProfile::Gender::MALE;
        _toGenderMap[female] = SocialProfile::Gender::FEMALE;
        _toGenderMap[unknown] = SocialProfile::Gender::UNKNOWN;
    }
    SocialProfile::Gender toGender(const T &value) const
    {
        auto it = _toGenderMap.find(value);
        return it == _toGenderMap.end()?SocialProfile::Gender::OTHER:it->second;
    }
private:
    std::map<T, SocialProfile::Gender> _toGenderMap;
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
