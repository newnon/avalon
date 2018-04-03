//
//  OKSocialPlugin.h
//  Odnoklassniki iOS
//
//  Created by Vladimir Shlobin on 02.04.2018.
//

#ifndef OKSocialPlugin_h
#define OKSocialPlugin_h

#include "SocialPlugin.h"

namespace avalon {

enum class OKPermissionType
{
    NONE = 0
};

using OKPermission = PlatformSocialPermission<OKPermissionType>;

class OKSocialPlugin : public SocialPlugin
{
public:
    static OKSocialPlugin &getInstance();
};

} // namespace avalon

#endif /* OKSocialPlugin_h */
