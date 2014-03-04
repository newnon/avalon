#ifdef AVALON_CONFIG_GAMECENTER_ENABLED
#ifndef AVALON_GAMECENTER_H
#define AVALON_GAMECENTER_H

#include <avalon/utils/platform.h>

namespace avalon {

class GameCenter
{
public:
    void login();

    bool showAchievements();
    void postAchievement(const char* idName, int percentComplete, bool showBanner);
    void clearAllAchievements();

    std::string getPlayerId();

    bool showScores();
    void postScore(const char* idName, int score);
    void clearAllScores();

#if AVALON_PLATFORM_IS_ANDROID_GOOGLE
    void signIn();
    void signOut();
    bool isSignedIn();
#endif
};

} // namespace avalon

#endif /* AVALON_GAMECENTER_H */

#endif /* AVALON_CONFIG_GAMECENTER_ENABLED */
