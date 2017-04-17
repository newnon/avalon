//
//  FacebookSocialPlugin.h
//  SkillGames
//
//  Created by Vitaly on 14/04/2017.
//
//

#include "SocialPlugin.h"

namespace avalon {
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
