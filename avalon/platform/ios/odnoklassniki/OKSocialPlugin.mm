//
//  OKSocialPlugin.mm
//  Odnoklassniki iOS
//
//  Created by Vladimir Shlobin on 02.04.2018.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "OKSDK.h"
#include <vector>
#include <string>
#include "SocialPluginHelpers.h"
#include "OKSocialPlugin.h"

namespace avalon {

static std::vector<std::string> split(const char *str, char c)
{
    std::vector<std::string> result;

    if (str && *str != '\0')
    {
        do
        {
            const char *begin = str;

            while(*str != c && *str)
                str++;

            result.push_back(std::string(begin, str));
        } while (0 != *str++);
    }

    return result;
}

class OKSocialPluginIOS : public OKSocialPlugin {
public:
    OKSocialPluginIOS();
    void setDelegate(SocialPluginDelegate *delegate) override;
    void setDebug(bool value) override;
    void login() override;
    void login(const std::vector<SocialPermission> &permissions) override;
    void requestReadPermissions(const std::vector<SocialPermission> &permissions) override;
    void requestPublishPermissions(const std::vector<SocialPermission> &permissions) override;
    void logout() override;
    bool isLoggedIn() const override;
    std::string getUserID() const override;
    std::string getAccessToken() const override;
    std::string getAppId() const override;
    std::vector<SocialPermission> getReadPermissionList() const override;
    std::vector<SocialPermission> getPublishPermissionList() const override;
    void getMyProfile(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override;
    void getProfiles(const std::vector<std::string> &userIds, void *userData, int preferedPictureSize, const std::vector<std::string> &additionalFields) override;
    void getAppFriends(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override;
private:
    void doLogout();
    SocialPermissionsHelper<OKPermission> _permissionsHelper;
    GenderHelper<std::string> _genderHelper;
    std::map<int, std::string> _picturesMap;
    std::string _pictureIDKey;
    SocialProfile _emptyProfile;
    SocialPluginDelegate *_delegate = nullptr;
    bool _debug = false;
    std::string _userID;
    bool _isLoggedIn = false;
    std::vector<SocialPermission> _readPermissions;
    std::vector<SocialPermission> _publishPermissions;
};

OKSocialPluginIOS::OKSocialPluginIOS()
: _permissionsHelper({
    {SocialPermission::Type::PUBLIC_PROFILE, "VALUABLE_ACCESS"},
    {SocialPermission::Type::EMAIL, "GET_EMAIL"}})
, _genderHelper("male", "female", "")
, _picturesMap{
    {50, "pic_1"},
    {128, "pic_2"},
    {190, "pic_3"},
    {228, "pic_4"},
    {768, "pic_full"}}
, _pictureIDKey("photo_id")
{
    OKSDKInitSettings *settings = [OKSDKInitSettings new];
    settings.appKey = [NSBundle.mainBundle.infoDictionary objectForKey:@"OKAppKey"];
    settings.appId = [NSBundle.mainBundle.infoDictionary objectForKey:@"OKAppId"];
    settings.controllerHandler = ^{
        return UIApplication.sharedApplication.keyWindow.rootViewController;
    };
    [OKSDK initWithSettings:settings];
}

void OKSocialPluginIOS::setDelegate(SocialPluginDelegate *delegate)
{
    _delegate = delegate;
}

void OKSocialPluginIOS::setDebug(bool value)
{
    _debug = value;
}

void OKSocialPluginIOS::login()
{
    login({SocialPermission::Type::PUBLIC_PROFILE, SocialPermission::Type::FRIENDS, SocialPermission::Type::EMAIL});
}

void OKSocialPluginIOS::login(const std::vector<SocialPermission> &permissions)
{
    requestReadPermissions(permissions);
}

void OKSocialPluginIOS::requestReadPermissions(const std::vector<SocialPermission> &permissions)
{
    NSMutableArray *permissionArray = [NSMutableArray new];
    for (const auto &permission : permissions)
    {
        const auto &permissionString = _permissionsHelper.toString(permission);
        if (!permissionString.empty())
            [permissionArray addObject:[NSString stringWithUTF8String:permissionString.c_str()]];
    }
    [permissionArray addObject:@"LONG_ACCESS_TOKEN"];
    _readPermissions = permissions;

    // TODO debug

    auto localPermissions = permissions;
    OKErrorBlock errorBlock = ^(NSError *error)
    {
        _readPermissions.clear();
        if (_delegate)
            _delegate->onLogin({SocialPluginDelegate::Error::Type::UNDEFINED, 0, error.description.UTF8String}, "", {}, {});
    };
    [OKSDK authorizeWithPermissions:permissionArray success:^(id data){
        [OKSDK invokeMethod:@"users.getLoggedInUser" arguments:@{} success:^(id data)
        {
            NSString *userIdString = data;
            _userID = userIdString.UTF8String;
            _isLoggedIn = true;
            if (_delegate)
                _delegate->onLogin({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, [OKSDK currentAccessToken].UTF8String, localPermissions, {});
        } error:^(NSError *error)
        {
            doLogout();
            requestReadPermissions(localPermissions);
        }];
    } error:errorBlock];
}

void OKSocialPluginIOS::requestPublishPermissions(const std::vector<SocialPermission> &permissions)
{
    // TODO
}

void OKSocialPluginIOS::doLogout()
{
    _userID.clear();
    _isLoggedIn = false;
    [OKSDK clearAuth];
}

void OKSocialPluginIOS::logout()
{
    doLogout();

    if (_delegate)
        _delegate->onLogout({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""});
}

bool OKSocialPluginIOS::isLoggedIn() const
{
    return _isLoggedIn;
}

std::string OKSocialPluginIOS::getUserID() const
{
    return _userID;
}

std::string OKSocialPluginIOS::getAccessToken() const
{
    return [OKSDK currentAccessToken].UTF8String;
}

std::string OKSocialPluginIOS::getAppId() const
{
    return ((NSString *)[NSBundle.mainBundle.infoDictionary objectForKey:@"OKAppId"]).UTF8String;
}

std::vector<SocialPermission> OKSocialPluginIOS::getReadPermissionList() const
{
    return _readPermissions;
}

std::vector<SocialPermission> OKSocialPluginIOS::getPublishPermissionList() const
{
    return _publishPermissions;
}

void OKSocialPluginIOS::getMyProfile(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields)
{
    if (!isLoggedIn())
    {
        if (_delegate)
            _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""}, userData, _emptyProfile);
        return;
    }

    std::string fields = "uid,first_name,last_name,email,birthday,gender";
    for (const auto &field : additionalFields)
        fields += "," + field;

    auto picturesMapIterator = _picturesMap.lower_bound(preferedPictureSize);
    const auto &pictureField = (picturesMapIterator == _picturesMap.end()) ? _picturesMap.rbegin()->second : picturesMapIterator->second;
    fields += "," + _pictureIDKey + "," + pictureField;

    OKResultBlock successBlock = ^(id data)
    {
        SocialProfile profile;
        NSDictionary *dataDictionary = data;
        for (id key in dataDictionary)
        {
            id object = [dataDictionary objectForKey:key];

            if ([key isEqualToString:@"uid"])
            {
                profile.uid = [object UTF8String];
            }
            else if ([key isEqualToString:@"first_name"])
            {
                profile.firstName = [object UTF8String];
            }
            else if ([key isEqualToString:@"last_name"])
            {
                profile.lastName = [object UTF8String];
            }
            else if ([key isEqualToString:[NSString stringWithUTF8String:pictureField.c_str()]])
            {
                profile.pictureUrl = [object UTF8String];
            }
            else if ([key isEqualToString:@"email"])
            {
                profile.email = [object UTF8String];
            }
            else if ([key isEqualToString:[NSString stringWithUTF8String:_pictureIDKey.c_str()]])
            {
                profile.pictureId = [object UTF8String];
            }
            else if ([key isEqualToString:@"birthday"])
            {
                std::vector<std::string> parts = split([object UTF8String], '-');
                if (parts.size() == 3)
                    profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[2]), std::stoi(parts[1]), std::stoi(parts[0]));
            }
            else if ([key isEqualToString:@"gender"])
            {
                profile.gender = _genderHelper.toGender([object UTF8String]);
            }
            else
            {
                if ([object isKindOfClass:[NSArray class]] || [object isKindOfClass:[NSDictionary class]])
                {
                    BOOL ret = [NSJSONSerialization isValidJSONObject:object];
                    if (ret)
                    {
                        NSError *error = nil;
                        NSData *data = [NSJSONSerialization dataWithJSONObject:object options:NSJSONWritingPrettyPrinted error:&error];
                        if (!error)
                        {
                            profile.otherValue.emplace_back([key UTF8String], std::string(static_cast<const char*>(data.bytes), data.length));
                        }
                    }
                }
                else if ([object isKindOfClass:[NSString class]])
                {
                    profile.otherValue.emplace_back([key UTF8String], [object UTF8String]);
                }
                else if ([object isKindOfClass:[NSNumber class]])
                {
                    profile.otherValue.emplace_back([key UTF8String], [object stringValue].UTF8String);
                }
                else
                {
                    if (_debug)
                        NSLog(@"OKSocialPluginIOS::getMyProfile Wrong type for object %@ by key %@", object, key);
                }
            }
        }

        // Placeholder image case
        if (profile.pictureId.empty())
            profile.pictureUrl.clear();

        if (_delegate)
            _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, userData, profile);
    };
    OKErrorBlock errorBlock = ^(NSError *error)
    {
        if (_delegate)
            _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::UNDEFINED, static_cast<int>(error.code), error.description.UTF8String}, userData, _emptyProfile);
    };
    [OKSDK invokeMethod:@"users.getCurrentUser" arguments:@{@"fields": [NSString stringWithUTF8String:fields.c_str()]} success:successBlock error:errorBlock];
}

void OKSocialPluginIOS::getProfiles(const std::vector<std::string> &userIds, void *userData, int preferedPictureSize, const std::vector<std::string> &additionalFields)
{
    // TODO -
}

void OKSocialPluginIOS::getAppFriends(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields)
{
    // TODO -
}

OKSocialPlugin &OKSocialPlugin::getInstance()
{
    static OKSocialPluginIOS instance;
    return instance;
}

} // namespace avalon
