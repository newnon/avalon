#ifndef __FACEBOOK_H__
#define __FACEBOOK_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <functional>
#include <ctime>

namespace avalon {
    
class FBSDKLoginResult;
    
class Facebook
{
public:
    
    class AccessToken
    {
    public:
        /*!
         @abstract Returns the app ID.
         */
        const std::string& getAppID() const { return _appID; }
        
        /*!
         @abstract Returns the known declined permissions.
         */
        const std::set<std::string>& getDeclinedPermissions() const { return _declinedPermissions; }
        
        /*!
         @abstract Returns the expiration date.
         */
        std::time_t getExpirationDate() const { return _expirationDate; }
        
        /*!
         @abstract Returns the known granted permissions.
         */
        const std::set<std::string>& getPermissions() const { return _permissions; }
        
        /*!
         @abstract Returns the date the token was last refreshed.
         */
        std::time_t getRefreshDate() const { return _refreshDate; }
        
        /*!
         @abstract Returns the opaque token string.
         */
        const std::string& getTokenString() const { return _tokenString; }
        
        /*!
         @abstract Returns the user ID.
         */
        const std::string& getUserID() const { return _userID; }
        
        AccessToken(const std::string &appID,
                    const std::set<std::string> &declinedPermissions,
                    std::time_t expirationDate,
                    const std::set<std::string> &permissions,
                    std::time_t refreshDate,
                    const std::string &tokenString,
                    const std::string &userID)
        : _appID(appID)
        , _declinedPermissions(declinedPermissions)
        , _expirationDate(_expirationDate)
        , _permissions(permissions)
        , _refreshDate(refreshDate)
        , _tokenString(tokenString)
        , _userID(userID)
        {}
        
    private:
        
        std::string _appID;
        std::set<std::string> _declinedPermissions;
        std::time_t _expirationDate;
        std::set<std::string> _permissions;
        std::time_t _refreshDate;
        std::string _tokenString;
        std::string _userID;
    }
    
    /*!
     @abstract Describes the call back to the FBSDKLoginManager
     @param result the result of the authorization
     @param error the authorization error, if any.
     */
    typedef std::function<void(const FBSDKLoginManagerLoginResult &result, void *error)> RequestTokenHandler;
    
    
    /*!
     @typedef FBSDKDefaultAudience enum
     
     @abstract
     Passed to open to indicate which default audience to use for sessions that post data to Facebook.
     
     @discussion
     Certain operations such as publishing a status or publishing a photo require an audience. When the user
     grants an application permission to perform a publish operation, a default audience is selected as the
     publication ceiling for the application. This enumerated value allows the application to select which
     audience to ask the user to grant publish permission for.
     */
    enum class DefaultAudience
    {
        /*! Indicates that the user's friends are able to see posts made by the application */
        Friends = 0,
        /*! Indicates that only the user is able to see posts made by the application */
        OnlyMe,
        /*! Indicates that all Facebook users are able to see posts made by the application */
        Everyone,
    };
    
    /*!
     @typedef FBSDKLoginBehavior enum
     
     @abstract
     Passed to the \c FBSDKLoginManager to indicate how Facebook Login should be attempted.
     
     @discussion
     Facebook Login authorizes the application to act on behalf of the user, using the user's
     Facebook account. Usually a Facebook Login will rely on an account maintained outside of
     the application, by the native Facebook application, the browser, or perhaps the device
     itself. This avoids the need for a user to enter their username and password directly, and
     provides the most secure and lowest friction way for a user to authorize the application to
     interact with Facebook.
     
     The \c FBSDKLoginBehavior enum specifies which log-in methods may be used. The SDK
     will determine the best behavior based on the current device (such as iOS version).
     */
    enum class LoginBehavior
    {
        /*!
         @abstract This is the default behavior, and indicates logging in through the native
         Facebook app may be used. The SDK may still use Safari instead.
         */
        Native = 0,
        /*!
         @abstract Attempts log in through the Safari or SFSafariViewController, if available.
         */
        Browser,
        /*!
         @abstract Attempts log in through the Facebook account currently signed in through
         the device Settings.
         @note If the account is not available to the app (either not configured by user or
         as determined by the SDK) this behavior falls back to \c FBSDKLoginBehaviorNative.
         */
        SystemAccount,
        /*!
         @abstract Attemps log in through a modal \c UIWebView pop up
         
         @note This behavior is only available to certain types of apps. Please check the Facebook
         Platform Policy to verify your app meets the restrictions.
         */
        Web,
    };
    
    static Facebook *getInstance();
    virtual const std::string& getVersion() const = 0;
    
    /*!
     @abstract the default audience.
     @discussion you should set this if you intend to ask for publish permissions.
     */
    virtual void setDefaultAudience(DefaultAudience defaultAudience) = 0;
    virtual DefaultAudience getDefaultAudience() const = 0;
    
    /*!
     @abstract the login behavior
     */
    virtual void setLoginBehavior(LoginBehavior loginBehavior) = 0;
    virtual LoginBehavior getLoginBehavior() const = 0;
    
    /*!
     @abstract Logs the user in or authorizes additional permissions.
     @param permissions the optional array of permissions. Note this is converted to NSSet and is only
     an NSArray for the convenience of literal syntax.
     @param fromViewController the view controller to present from. If nil, the topmost view controller will be
     automatically determined as best as possible.
     @param handler the callback.
     @discussion Use this method when asking for read permissions. You should only ask for permissions when they
     are needed and explain the value to the user. You can inspect the result.declinedPermissions to also
     provide more information to the user if they decline permissions.
     
     If `[FBSDKAccessToken currentAccessToken]` is not nil, it will be treated as a reauthorization for that user
     and will pass the "rerequest" flag to the login dialog.
     
     This method will present UI the user. You typically should check if `[FBSDKAccessToken currentAccessToken]`
     already contains the permissions you need before asking to reduce unnecessary app switching. For example,
     you could make that check at viewDidLoad.
     */
    virtual void logInWithReadPermissions(const std::vector<std::string> &permissions, const RequestTokenHandler &handler) = 0;
    
    /*!
     @abstract Logs the user in or authorizes additional permissions.
     @param permissions the optional array of permissions. Note this is converted to NSSet and is only
     an NSArray for the convenience of literal syntax.
     @param fromViewController the view controller to present from. If nil, the topmost view controller will be
     automatically determined as best as possible.
     @param handler the callback.
     @discussion Use this method when asking for publish permissions. You should only ask for permissions when they
     are needed and explain the value to the user. You can inspect the result.declinedPermissions to also
     provide more information to the user if they decline permissions.
     
     If `[FBSDKAccessToken currentAccessToken]` is not nil, it will be treated as a reauthorization for that user
     and will pass the "rerequest" flag to the login dialog.
     
     This method will present UI the user. You typically should check if `[FBSDKAccessToken currentAccessToken]`
     already contains the permissions you need before asking to reduce unnecessary app switching. For example,
     you could make that check at viewDidLoad.
     */
    virtual void logInWithPublishPermissions(const std::vector<std::string> &permissions, const RequestTokenHandler &handler) = 0;
    
    /*!
     @abstract Logs the user out
     @discussion This calls [FBSDKAccessToken setCurrentAccessToken:nil] and [FBSDKProfile setCurrentProfile:nil].
     */
    virtual void logOut = 0;
    
    virtual void const AccessToken* getAccessToken() = 0;
    
};
}

#endif /* __FACEBOOK_H__ */
