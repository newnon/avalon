//
//  FacebookSocialPlugin.h
//  SkillGames
//
//  Created by Vitaly on 14/04/2017.
//
//

#ifndef FacebookSocialPlugin_h
#define FacebookSocialPlugin_h

#include "SocialPlugin.h"

namespace avalon {
    
enum class FacebookPermissionType
{
    NONE = 0,
};
    
using FacebookPermission = PlatformSocialPermission<FacebookPermissionType>;
    
class FacebookSocialPlugin : public SocialPlugin
{
public:
    enum class LoginBehavior
    {
        Native = 0,
        Browser,
        SystemAccount,
        Web,
    };
    
    static FacebookSocialPlugin &getInstance();
    
    virtual void setLoginBehavior(LoginBehavior type) = 0;
};
}

#endif /* FacebookSocialPlugin_h */
