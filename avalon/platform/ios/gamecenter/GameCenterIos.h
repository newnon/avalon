#import <GameKit/GameKit.h>
@interface GameCenterIos : NSObject <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate>
{
}

+ (GameCenterIos*)shared;

- (void)login;
- (void)logout;

- (BOOL)isLoggedIn;

- (BOOL)showAchievements;
- (void)postAchievement:(const char*)idName percent:(NSNumber*)percentComplete showBanner:(BOOL)show;
- (void)clearAllAchivements;

- (BOOL)showScores;
- (void)postScore:(const char*)idName score:(NSNumber*)score;
- (void)clearAllScores;
- (NSString *)getPlayerId;

@end