#include "avalon/GameCenter.h"

#include "avalon/platform/ios/gamecenter/GameCenterIos.h"

namespace avalon {

GameCenter* GameCenter::getInstance()
{
    static GameCenter* instance = new GameCenter();
    return instance;
}

void GameCenter::login()
{
    [[GameCenterIos shared] login];
}
    
void GameCenter::logout()
{
    [[GameCenterIos shared] logout];
}
    
bool GameCenter::isLoggedIn()
{
    return [[GameCenterIos shared] isLoggedIn];
}

#pragma mark -
#pragma mark Achievements

bool GameCenter::showAchievements()
{
    return [[GameCenterIos shared] showAchievements];
}

void GameCenter::postAchievement(const std::string &idName, int percentComplete, bool showBanner)
{
    [[GameCenterIos shared] postAchievement:idName.c_str() percent:[NSNumber numberWithInt:percentComplete] showBanner:showBanner];
}

void GameCenter::clearAllAchievements()
{
    [[GameCenterIos shared] clearAllAchivements];
}

std::string GameCenter::getPlayerId()
{
    NSString *playerId = [[GameCenterIos shared] getPlayerId];
    std::string rv = [playerId cStringUsingEncoding:NSUTF8StringEncoding];
    return rv;
}
    
#pragma mark -
#pragma mark Leaderboard

bool GameCenter::showScores()
{
    return [[GameCenterIos shared] showScores];
}

void GameCenter::postScore(const std::string &idName, int score)
{
    [[GameCenterIos shared] postScore:idName.c_str() score:[NSNumber numberWithInt:score]];
}

void GameCenter::clearAllScores()
{
    [[GameCenterIos shared] clearAllScores];
}

} // namespace avalon
