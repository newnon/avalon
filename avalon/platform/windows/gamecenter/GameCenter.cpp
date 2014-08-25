#include "avalon/GameCenter.h"

//#include "avalon/platform/mac/gamecenter/GameCenterMac.h"

namespace avalon {

GameCenter* GameCenter::getInstance()
{
    static GameCenter* instance = new GameCenter();
    return instance;
}

void GameCenter::login()
{
    //[[GameCenterMac shared] login];
}
    
void GameCenter::logout()
{
    //[[GameCenterMac shared] logout];
}
    
bool GameCenter::isLoggedIn()
{
    //return [[GameCenterMac shared] isLoggedIn];
    return false;
}
    
bool GameCenter::showGameCenter()
{
    //return [[GameCenterMac shared] showGameCenter];
    return false;
}

bool GameCenter::showAchievements()
{
    //return [[GameCenterMac shared] showAchievements];
    return false;
}

void GameCenter::postAchievement(const std::string &idName, int percentComplete, bool showBanner)
{
    //[[GameCenterMac shared] postAchievement:idName.c_str() percent:[NSNumber numberWithInt:percentComplete] showBanner:showBanner];
}

void GameCenter::clearAllAchievements()
{
    //[[GameCenterMac shared] clearAllAchivements];
}

std::string GameCenter::getPlayerId()
{
    //NSString *playerId = [[GameCenterMac shared] getPlayerId];
    //std::string rv = [playerId cStringUsingEncoding:NSUTF8StringEncoding];
    return "";
}

bool GameCenter::showScores()
{
    //return [[GameCenterMac shared] showScores];
    return false;
}

void GameCenter::postScore(const std::string &idName, int score)
{
    //[[GameCenterMac shared] postScore:idName.c_str() score:[NSNumber numberWithInt:score]];
}

void GameCenter::clearAllScores()
{
    //[[GameCenterMac shared] clearAllScores];
}

} // namespace avalon
