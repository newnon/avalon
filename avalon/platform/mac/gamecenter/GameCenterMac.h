#import <GameKit/GameKit.h>
@interface GameCenterMac : NSObject <GKGameCenterControllerDelegate, GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate>
{
}

+ (GameCenterMac*)shared;

- (void)login;
- (void)logout;

- (BOOL)isLoggedIn;

- (BOOL)showGameCenter;

- (BOOL)showAchievements;
- (void)postAchievement:(const char*)idName percent:(NSNumber*)percentComplete showBanner:(BOOL)show;
- (void)clearAllAchivements;

- (BOOL)showScores;
- (void)postScore:(const char*)idName score:(NSNumber*)score;
- (void)clearAllScores;
- (NSString *)getPlayerId;

@end