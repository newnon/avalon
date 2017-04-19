//
//  SocialPlugin.h
//  avalon
//
//  Created by Sergey on 14.04.17.
//  Copyright © 2017 Sergey. All rights reserved.
//

#ifndef SocialPlugin_h
#define SocialPlugin_h

#include <vector>
#include <string>
#include <functional>
#include <utility>

namespace avalon {
class SocialPermission
{
public:
    enum class Type : uint16_t
    {
        NONE = 0,
        PUBLIC_PROFILE,
        EMAIL,
        FRIENDS,
        PLATFORM = 65535,
    };
    SocialPermission(Type type):_type(type) {}
    
    int getPlatformType() const { return _platformType; }
    Type getType() const { return _type; }
    
    bool operator<(const SocialPermission &other) const { return _type == other._type?_platformType<other._platformType:_type<other._type; }
    
protected:
    SocialPermission(Type type, uint16_t platformType):_type(type), _platformType(platformType) {}
    
private:
    Type _type;
    uint16_t _platformType = 0;
};
    
template <class T>
class PlatformSocialPermission:public SocialPermission {
public:
    typedef T Type;
    PlatformSocialPermission(SocialPermission::Type type):SocialPermission(type) {}
    PlatformSocialPermission(T type):SocialPermission(SocialPermission::Type::PLATFORM, static_cast<int>(type)) {}
    PlatformSocialPermission(SocialPermission permission):SocialPermission(permission.getType(), permission.getPlatformType()) {}
};

struct SocialProfile
{
    enum class Gender
    {
        UNKNOWN,
        MALE,
        FEMALE,
        OTHER
    };
    
    struct BirthDate
    {
        BirthDate():day(0),month(0),year(0) {};
        BirthDate(int day, int month, int year):day(day),month(month),year(year){}
        BirthDate(int fromAge, int toAge):day(0),month(0),year(0),ageRange(fromAge, toAge){}
        int day;
        int month;
        int year;
        std::pair<int,int> ageRange;
    };
    
    std::string uid;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string nickName;
    std::string pictureUrl;
    std::string email;
    BirthDate birthDate;
    Gender gender;
    std::vector<std::pair<std::string, std::string>> otherValue;
};

class SocialPluginDelegate
{
public:
    struct Error
    {
        enum class Type
        {
            SUCCESS,
            USER_CANCEL,
            UNDEFINED,
            NO_LOGIN,
        };
        
        Type type;
        int code;
        std::string description;
    };
    
    enum class Gender
    {
        UNKNOWN,
        MALE,
        FEMALE,
        OTHER
    };
    
    virtual void onLogin(Error error, const std::string& token, const std::vector<SocialPermission>& grantedPermissions, const std::vector<SocialPermission>& declinedPermissions) = 0;
    virtual void onLogout(Error error) = 0;
    virtual void onGetMyProfile(Error error, void *userData, const SocialProfile &profiles) = 0;
    virtual void onGetProfiles(Error error, void *userData, const std::vector<SocialProfile> &profiles) = 0;
    virtual void onAppFriends(Error error, void *userData, const std::vector<SocialProfile> &friends) = 0;
    
    ~SocialPluginDelegate(){}
};

class SocialPlugin
{
public:
    
    /**
     * @brief set delegate
     *
     * This method set delegates for all results
     */
    virtual void setDelegate(SocialPluginDelegate *delegate) = 0;
    
    /**
     * @brief log in
     *
     * This method calls login with default permissions
     */
    virtual void login() = 0;
    
    /**
     * @brief log in
     * @param permissions permissions
     * This method calls login with default permissions
     */
    virtual void login(const std::vector<SocialPermission>& permissions) = 0;
    
    /**
     * @brief log in with specific read permissions, conflict with publish permissions
     * @param permissions permissions
     */
    virtual void requestReadPermissions(const std::vector<SocialPermission>& permissions) = 0;
    
    /**
     * @brief log in with specific public permissions
     * @param permissions permissions
     */
    virtual void requestPublishPermissions(const std::vector<SocialPermission>& permissions) = 0;
    
    /**
     * @brief log out
     */
    virtual void logout() = 0;
    
    /**
     * @brief Check whether the user logined or not
     */
    virtual bool isLoggedIn() const = 0;
    
    /**
     * @brief get UserID
     */
    virtual std::string getUserID() const = 0;
    
    /**
     * @brief get AccessToken
     */
    virtual std::string getAccessToken() const = 0;
    
    /**
     * @brief get read permissoin list
     */
    virtual std::vector<SocialPermission> getReadPermissionList() const = 0;
    
    /**
     * @brief get publish permissoin list
     */
    virtual std::vector<SocialPermission> getPublishPermissionList() const = 0;
    
    /**
     * @brief get user profile
     * @preferedPictureSize preffered user picture size 0 for default
     * @additionalFields social newwork specified additionla fields
     */
    virtual void getMyProfile(int preferedPictureSize = 0, void *userData = nullptr, const std::vector<std::string> &additionalFields = {}) = 0;
    
    /**
     * @brief get users profiles
     * @param userIds vector of social user id strings
     * @preferedPictureSize preffered user picture size in pixels 0 for default
     * @additionalFields social newwork specified additionla fields
     */
    virtual void getProfiles(const std::vector<std::string> &userIds, void *userData = nullptr, int preferedPictureSize = 0, const std::vector<std::string> &additionalFields = {}) = 0;
    
    /**
     * @brief get friends that alredy have installed this app
     * @preferedPictureSize preffered user picture size in pixels 0 for default
     * @additionalFields social newwork specified additionla fields
     */
    virtual void getAppFriends(int preferedPictureSize = 0, void *userData = nullptr, const std::vector<std::string> &additionalFields = {}) = 0;
    
protected:
    virtual ~SocialPlugin() {}
};
}

#endif /* SocialPlugin_h */
