#ifndef AVALON_GAMECENTER_H
#define AVALON_GAMECENTER_H

#include <string>

namespace avalon {

class GameCenter
{
public:
    static GameCenter* getInstance();
    
    void login();
    void logout();
    
    bool isLoggedIn();
    
    bool showGameCenter();

    bool showAchievements();
    void postAchievement(const std::string &idName, int percentComplete, bool showBanner);
    void clearAllAchievements();

    std::string getPlayerId();

    bool showScores();
    void postScore(const std::string &idName, int score);
    void clearAllScores();
};

} // namespace avalon

#endif /* AVALON_GAMECENTER_H */

