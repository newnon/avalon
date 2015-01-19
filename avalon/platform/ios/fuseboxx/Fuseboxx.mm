#include "avalon/Fuseboxx.h"
#import "FuseSDK.h"

@interface IOSFuseDelegate : NSObject <FuseDelegate> {
}
@end

@interface IOSFuseAdDelegate : NSObject <FuseAdDelegate> {
}
@end

@interface IOSFuseOverlayDelegate : NSObject <FuseOverlayDelegate> {
}
@end

namespace avalon {
    
class IOSFuseAPI : public FuseAPI
{
public:
    IOSFuseAPI()
    {
        _delegate = [[IOSFuseDelegate alloc] init];
    }
    virtual void startSession(const std::string &game_id, FuseDelegate *delegate, bool AutoRegisterForPush) override
    {
        [::FuseAPI startSession:[NSString stringWithUTF8String:game_id.c_str()] Delegate:_delegate AutoRegisterForPush:AutoRegisterForPush];
    }
        
    virtual void setPlatform(const std::string &game_Platform) override
    {
        [::FuseAPI setPlatform:[NSString stringWithUTF8String:game_Platform.c_str()]];
    }

    virtual int registerEvent(const std::string &message, const std::map<std::string,std::string> &dict) override
    {
        NSMutableDictionary *dictionary = nil;
        return [::FuseAPI registerEvent:[NSString stringWithUTF8String:message.c_str()] withDict:dictionary];
    }

    virtual int registerEvent(const std::string &name,const std::string &param_name, const std::string &param_value, const std::map<std::string,std::string> &variables) override
    {
        NSMutableDictionary *dictionary = nil;
        return [::FuseAPI registerEvent:[NSString stringWithUTF8String:name.c_str()] ParameterName:[NSString stringWithUTF8String:param_name.c_str()] ParameterValue:[NSString stringWithUTF8String:param_value.c_str()] Variables:dictionary];
    }

    virtual void registerCrash(const std::string &name, const std::string &reason) override
    {
        [::FuseAPI registerCrash:[NSException exceptionWithName:[NSString stringWithUTF8String:name.c_str()] reason:[NSString stringWithUTF8String:reason.c_str()] userInfo:nil]];
    }
    virtual void registerInAppPurchase(const std::vector<uint8_t> &receipt_data, int tx_state, const std::string &price, const std::string &currency, const std::string &product_id, const std::string &tx_id) override
    {
        [::FuseAPI registerInAppPurchase:[NSData dataWithBytes:&receipt_data.front() length:receipt_data.size()] TxState:tx_state  Price:[NSString stringWithUTF8String:price.c_str()] Currency:[NSString stringWithUTF8String:currency.c_str()] ProductID:[NSString stringWithUTF8String:product_id.c_str()] TransactionID:[NSString stringWithUTF8String:tx_id.c_str()]];
    }

    virtual void showAdWithDelegate(FuseAdDelegate *delegate) override
    {
        [::FuseAPI showAdWithDelegate:[[IOSFuseAdDelegate alloc] init]];
    }

    virtual void showAdWithDelegate(FuseAdDelegate *delegate, const std::string &zone) override
    {
        [::FuseAPI showAdWithDelegate:[[IOSFuseAdDelegate alloc] init] adZone:[NSString stringWithUTF8String:zone.c_str()]];
    }

    virtual void checkAdAvailable() override
    {
        [::FuseAPI checkAdAvailable];
    }

    virtual void checkAdAvailableWithDelegate(FuseAdDelegate *delegate) override
    {
        [::FuseAPI checkAdAvailableWithDelegate:[[IOSFuseAdDelegate alloc] init]];
    }
    
    virtual void checkAdAvailableWithDelegate(FuseAdDelegate *delegate, const std::string &adZone) override
    {
        [::FuseAPI checkAdAvailableWithDelegate:[[IOSFuseAdDelegate alloc] init] withAdZone:[NSString stringWithUTF8String:adZone.c_str()]];
    }

    virtual void preLoadAdForZone(const std::string &adZone) override
    {
        [::FuseAPI preLoadAdForZone:[NSString stringWithUTF8String:adZone.c_str()]];
    }

    virtual void displayNotifications() override
    {
        [::FuseAPI displayNotifications];
    }

    virtual bool isNotificationAvailable() override
    {
        return [::FuseAPI isNotificationAvailable];
    }

    virtual void displayMoreGames(FuseOverlayDelegate *delegate) override
    {
        [::FuseAPI displayMoreGames:[[IOSFuseOverlayDelegate alloc] init]];
    }

    virtual void registerGender(kFuseGender gender) override
    {
        [::FuseAPI registerGender:static_cast<int>(gender)];
    }

    virtual void registerAge(int age) override
    {
        [::FuseAPI registerAge:age];
    }

    virtual void registerBirthday(int year, int month, int day) override
    {
        [::FuseAPI registerBirthday:year Month:month Day:day];
    }

    virtual const std::string& getFuseID() const override
    {
        return _id;
    }

    virtual void gameCenterLogin() override
    {
        [::FuseAPI gameCenterLogin:[GKLocalPlayer localPlayer]];
    }
    
    virtual void facebookLogin(const std::string &facebook_id, const std::string &name, const std::string &accesstoken) override
    {
        [::FuseAPI facebookLogin:[NSString stringWithUTF8String:facebook_id.c_str()] Name:[NSString stringWithUTF8String:name.c_str()] withAccessToken:[NSString stringWithUTF8String:accesstoken.c_str()]];
    }
    
    virtual void facebookLogin(const std::string &facebook_id, const std::string &name, kFuseGender gender, const std::string &accesstoken) override
    {
        [::FuseAPI facebookLogin:[NSString stringWithUTF8String:facebook_id.c_str()] Name:[NSString stringWithUTF8String:name.c_str()] Gender:static_cast<int>(gender) withAccessToken:[NSString stringWithUTF8String:accesstoken.c_str()]];
    }
    
    virtual void twitterLogin(const std::string &twitter_id) override
    {
        [::FuseAPI twitterLogin:[NSString stringWithUTF8String:twitter_id.c_str()]];
    }
    
    virtual void twitterLogin(const std::string &twitter_id, const std::string &alias) override
    {
        [::FuseAPI twitterLogin:[NSString stringWithUTF8String:twitter_id.c_str()] Alias:[NSString stringWithUTF8String:alias.c_str()]];
    }
    
    virtual void openFeintLogin(const std::string &openfeint_id) override
    {
        [::FuseAPI openFeintLogin:[NSString stringWithUTF8String:openfeint_id.c_str()]];
    }
    
    virtual void fuseLogin(const std::string &fuse_id, const std::string &alias) override
    {
        [::FuseAPI fuseLogin:[NSString stringWithUTF8String:fuse_id.c_str()] Alias:[NSString stringWithUTF8String:alias.c_str()]];
    }
    
    virtual void emailLogin(const std::string &email, const std::string &alias) override
    {
        [::FuseAPI emailLogin:[NSString stringWithUTF8String:email.c_str()] Alias:[NSString stringWithUTF8String:alias.c_str()]];
    }
    
    virtual void deviceLogin(const std::string &alias) override
    {
        [::FuseAPI deviceLogin:[NSString stringWithUTF8String:alias.c_str()]];
    }
    
    virtual void googlePlayLogin(const std::string &alias, const std::string &token) override
    {
        [::FuseAPI googlePlayLogin:[NSString stringWithUTF8String:alias.c_str()] AccessToken:[NSString stringWithUTF8String:token.c_str()]];
    }
    
    virtual std::string getOriginalAccountID() override
    {
        return [[::FuseAPI getOriginalAccountID] cStringUsingEncoding:NSUTF8StringEncoding];
    }
    
    virtual kFuseAccountType getOriginalAccountType() override
    {
        return static_cast<kFuseAccountType>([::FuseAPI getOriginalAccountType]);
    }
    
    virtual std::string getOriginalAccountAlias() override
    {
        return [[::FuseAPI getOriginalAccountAlias] cStringUsingEncoding:NSUTF8StringEncoding];
    }
    
    virtual int gamesPlayed() override
    {
        return [::FuseAPI gamesPlayed];
    }
    
    virtual const std::string& libraryVersion() override
    {
        return _libraryVersion;
    }
    
    virtual bool connected() override
    {
        return [::FuseAPI connected];
    }
    
    virtual void utcTimeFromServer() override
    {
        [::FuseAPI utcTimeFromServer];
    }
    
    virtual bool notReadyToTerminate() override
    {
        return [::FuseAPI notReadyToTerminate];
    }
    
    virtual void disableData() override
    {
        [::FuseAPI disableData];
    }
    
    virtual void enableData() override
    {
        [::FuseAPI enableData];
    }
    
    virtual bool dataEnabled() override
    {
        return [::FuseAPI dataEnabled];
    }

    virtual int setGameData(const std::map<std::string,std::string> &data, FuseGameDataDelegate *delegate) override
    {
        return 0;
    }

    virtual int setGameData(const std::map<std::string,std::string> &data, const std::string &key, FuseGameDataDelegate *delegate) override
    {
        return 0;
    }

    virtual int setGameData(const std::map<std::string,std::string> &data, const std::string &key, FuseGameDataDelegate *delegate, bool collection) override
    {
        return 0;
    }

    virtual int setGameData(const std::map<std::string,std::string> &data, const std::string &fuse_id, const std::string &key, FuseGameDataDelegate *delegate, bool collection) override
    {
        return 0;
    }

    virtual int getGameData(const std::vector<std::string> &keys, FuseGameDataDelegate *delegate) override
    {
        return 0;
    }

    virtual int getGameData(const std::vector<std::string> &keys, const std::string &key, FuseGameDataDelegate *delegate) override
    {
        return 0;
    }

    virtual int getFriendGameData(const std::vector<std::string> &keys, const std::string &fuse_id, FuseGameDataDelegate *delegate)override
    {
        return 0;
    }

    virtual int getFriendGameData(const std::vector<std::string> &keys, const std::string &key, const std::string &fuse_id, FuseGameDataDelegate *delegate) override
    {
        return 0;
    }

    virtual void updateFriendsListFromServer() override
    {
        [::FuseAPI updateFriendsListFromServer];
    }

    virtual const std::map<std::string, FuseboxxFriendEntry> &getFriendsList() override
    {
        return _friends;
    }

    virtual void addFriend(const std::string &fuse_id) override
    {
        [::FuseAPI addFriend:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual void removeFriend(const std::string &fuse_id) override
    {
        [::FuseAPI removeFriend:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual void acceptFriend(const std::string &fuse_id) override
    {
        [::FuseAPI acceptFriend:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual void rejectFriend(const std::string &fuse_id) override
    {
        [::FuseAPI rejectFriend:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual void migrateFriends(const std::string &fuse_id) override
    {
        [::FuseAPI migrateFriends:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual void postUserChatMessage(const std::string &message, const std::string &fuse_id) override
    {
        [::FuseAPI postUserChatMessage:[NSString stringWithUTF8String:message.c_str()] TargetUser:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual void postUserChatMessage(const std::string &message, const std::string &fuse_id, int level) override
    {
        [::FuseAPI postUserChatMessage:[NSString stringWithUTF8String:message.c_str()] TargetUser:[NSString stringWithUTF8String:fuse_id.c_str()] Level:level];
    }
 
    virtual void getUserChatListFromServer(const std::string &fuse_id) override
    {
        [::FuseAPI getUserChatListFromServer:[NSString stringWithUTF8String:fuse_id.c_str()]];
    }

    virtual const std::map<std::string, FuseboxxChatListEntry>& getUserChatList(const std::string &fuse_id) override
    {
        return _chatList;
    }

    virtual void deleteUserChatMessage(int message_id, const std::string &fuse_id) override
    {
        
    }

    virtual void userPushNotification(const std::string &fuse_id, const std::string &message) override
    {
        
    }

    virtual void friendsPushNotification(const std::string &message) override
    {
        
    }

    virtual void getMailListFromServer() override
    {
        
    }

    virtual void getMailListFriendFromServer(const std::string &fuse_id) override
    {
        
    }

    virtual const std::map<std::string, FuseboxxMailListEntry>& getMailList(const std::string &_fuse_id) override
    {
        return _mailList;
    }

    virtual void setMailAsReceived(int message_id) override
    {
        
    }
    
    virtual int sendMailWithGift(const std::string &fuse_id, const std::string &message, int gift_id, int amount) override
    {
        return 0;
    }
    
    virtual int sendMail(const std::string &fuse_id, const std::string &message) override
    {
        return 0;
    }
    
    virtual void registerLevel(int level) override
    {
        
    }
    
    virtual void registerCurrency(int currencyType, int balance) override
    {
        
    }

    virtual void registerFlurryView() override
    {
        
    }

    virtual void registerFlurryClick() override
    {
        
    }

    virtual void registerTapjoyReward(int amtCurrency) override
    {
        
    }

    virtual std::string getGameConfigurationValue(const std::string &key) override
    {
        return "";
    }
    
    virtual const std::map<std::string,std::string>& getGameConfiguration() override
    {
        return _gameConfiguration;
    }
private:
    IOSFuseDelegate *_delegate;
    std::string _id;
    std::string _libraryVersion;
    std::map<std::string, FuseboxxFriendEntry> _friends;
    std::map<std::string, FuseboxxChatListEntry> _chatList;
    std::map<std::string, FuseboxxMailListEntry> _mailList;
    std::map<std::string,std::string> _gameConfiguration;
};

FuseAPI& FuseAPI::getInstance()
{
    static IOSFuseAPI instance;
    return instance;
}

}
