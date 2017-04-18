//
//  VKSocialPlugin.h
//  SkillGames
//
//  Created by Sergey on 18/04/2017.
//
//

#ifndef VKSocialPlugin_h
#define VKSocialPlugin_h

#include "SocialPlugin.h"

namespace avalon {
    
    
enum class VKPermissionType
{
    NONE = 0,
};

using VKPermission = PlatformSocialPermission<VKPermissionType>;
    
class VKSocialPlugin : public SocialPlugin
{
public:
    static VKSocialPlugin &getInstance();
};
}

#endif /* VKSocialPlugin_h */
