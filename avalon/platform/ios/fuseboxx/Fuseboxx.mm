#include "avalon/Fuseboxx.h"
#import "FuseSDK.h"

@interface IOSFuseDelegate : NSObject <FuseDelegate> {
}
@end

@interface IOSFuseAdDelegate : NSObject <FuseAdDelegate> {
    avalon::FuseAdDelegate* _delegate;
}
- (id) initWithDelegate:(avalon::FuseAdDelegate*)delegate;
@end

@interface IOSFuseOverlayDelegate : NSObject <FuseOverlayDelegate> {
    avalon::FuseOverlayDelegate* _delegate;
}
- (id) initWithDelegate:(avalon::FuseOverlayDelegate*)delegate;
@end

namespace avalon {
    
class IOSFuseAPI : public FuseAPI
{
public:
    IOSFuseAPI():_nativeDelegate([[IOSFuseDelegate alloc] init]),_delegate(nullptr),_libraryVersion([[::FuseAPI libraryVersion] UTF8String])
    {
    }
    virtual void startSession(const std::string &game_id, FuseDelegate *delegate, bool autoRegisterForPush) override
    {
        _delegate = delegate;
        [::FuseAPI startSession:[NSString stringWithUTF8String:game_id.c_str()] Delegate:_nativeDelegate AutoRegisterForPush:autoRegisterForPush];
    }
        
    virtual void setPlatform(const std::string &game_Platform) override
    {
        [::FuseAPI setPlatform:[NSString stringWithUTF8String:game_Platform.c_str()]];
    }

    virtual int registerEvent(const std::string &message, const std::map<std::string,double> &dict) override
    {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
        for(const auto &it: dict)
            [dictionary setObject:[NSNumber numberWithDouble:it.second] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        return [::FuseAPI registerEvent:[NSString stringWithUTF8String:message.c_str()] withDict:dictionary];
    }

    virtual int registerEvent(const std::string &name,const std::string &param_name, const std::string &param_value, const std::map<std::string,double> &variables) override
    {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
        for(const auto &it: variables)
            [dictionary setObject:[NSNumber numberWithDouble:it.second] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
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
        [::FuseAPI showAdWithDelegate:[[IOSFuseAdDelegate alloc] initWithDelegate:delegate]];
    }

    virtual void showAdWithDelegate(FuseAdDelegate *delegate, const std::string &zone) override
    {
        [::FuseAPI showAdWithDelegate:[[IOSFuseAdDelegate alloc] initWithDelegate:delegate] adZone:[NSString stringWithUTF8String:zone.c_str()]];
    }

    virtual void checkAdAvailable() override
    {
        [::FuseAPI checkAdAvailable];
    }

    virtual void checkAdAvailableWithDelegate(FuseAdDelegate *delegate) override
    {
        [::FuseAPI checkAdAvailableWithDelegate:[[IOSFuseAdDelegate alloc] initWithDelegate:delegate]];
    }
    
    virtual void checkAdAvailableWithDelegate(FuseAdDelegate *delegate, const std::string &adZone) override
    {
        [::FuseAPI checkAdAvailableWithDelegate:[[IOSFuseAdDelegate alloc] initWithDelegate:delegate] withAdZone:[NSString stringWithUTF8String:adZone.c_str()]];
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
        [::FuseAPI displayMoreGames:[[IOSFuseOverlayDelegate alloc] initWithDelegate:delegate]];
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
        [::FuseAPI registerLevel:level];
    }
    
    virtual void registerCurrency(int currencyType, int balance) override
    {
        [::FuseAPI registerCurrency:currencyType Balance:balance];
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

    virtual const std::string* getGameConfigurationValue(const std::string &key) override
    {
        auto it = _gameConfiguration.find(key);
        if(it != _gameConfiguration.end())
            return &it->second;
        return nullptr;
    }
    
    virtual const std::map<std::string,std::string>& getGameConfiguration() override
    {
        return _gameConfiguration;
    }
    
    void sessionStartReceived()
    {
        if(_delegate)
        {
            _delegate->sessionStartReceived();
        }
    }
    void sessionLoginError(NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->sessionLoginError(static_cast<kFuseErrors>([error intValue]));
        }
    }
    void timeUpdated(NSNumber* utcTimeStamp)
    {
        if(_delegate)
        {
            _delegate->timeUpdated([utcTimeStamp longLongValue]);
        }
    }
    void accountLoginComplete(NSNumber* type, NSString* account_id)
    {
        if(_delegate)
        {
            _delegate->accountLoginComplete(static_cast<kFuseAccountType>([type intValue]), [account_id UTF8String]);
        }
    }
    void accountLoginError(NSNumber* error, NSString* account_id)
    {
        if(_delegate)
        {
            _delegate->accountLoginError(static_cast<kFuseLoginErrors>([error intValue]), [account_id UTF8String]);
        }
    }
    void notificationAction(NSString* action)
    {
        if(_delegate)
        {
            _delegate->notificationAction([action UTF8String]);
        }
    }
    void notficationWillClose()
    {
        if(_delegate)
        {
            _delegate->notficationWillClose();
        }
    }
    void gameConfigurationReceived()
    {
        NSDictionary* configuration = [::FuseAPI getGameConfiguration];
        _gameConfiguration.clear();
        [configuration enumerateKeysAndObjectsUsingBlock: ^(id key, id obj, BOOL *stop) {
            _gameConfiguration[[key UTF8String]] = [obj UTF8String];
        }];
        if(_delegate)
        {
            _delegate->gameConfigurationReceived();
        }
    }
    void friendAdded(NSString* fuse_id, NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->friendAdded([fuse_id UTF8String], static_cast<kFuseFriendErrors>([error intValue]));
        }
    }
    void friendRemoved(NSString* fuse_id, NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->friendRemoved([fuse_id UTF8String], static_cast<kFuseFriendErrors>([error intValue]));
        }
    }
    void friendAccepted(NSString* fuse_id, NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->friendAccepted([fuse_id UTF8String], static_cast<kFuseFriendErrors>([error intValue]));
        }
    }
    void friendRejected(NSString* fuse_id, NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->friendRejected([fuse_id UTF8String], static_cast<kFuseFriendErrors>([error intValue]));
        }
    }
    void friendsMigrated(NSString* fuse_id, NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->friendsMigrated([fuse_id UTF8String], static_cast<kFuseFriendErrors>([error intValue]));
        }
    }
    void friendsListUpdated(NSDictionary* friendsList)
    {
        _friends.clear();
        [friendsList enumerateKeysAndObjectsUsingBlock: ^(id key, id obj, BOOL *stop) {
            auto &item = _friends[[key UTF8String]];
            item.pending = [[obj objectForKey:@"pending"] intValue];
            item.alias = [[obj objectForKey:@"alias"] UTF8String];
            item.fuse_id = [[obj objectForKey:@"fuse_id"] UTF8String];
        }];
        if(_delegate)
        {
            _delegate->friendsListUpdated(_friends);
        }
    }
    void friendsListError(NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->friendsListError(static_cast<kFuseFriendsListErrors>([error intValue]));
        }
    }
    void chatListReceived(NSDictionary* messages, NSString* fuse_id)
    {
        _chatList.clear();
        [messages enumerateKeysAndObjectsUsingBlock: ^(id key, id obj, BOOL *stop) {
            auto &item = _chatList[[key UTF8String]];
            item.timestamp = [[obj objectForKey:@"timestamp"] intValue];
            item.alias = [[obj objectForKey:@"alias"] UTF8String];
            item.message = [[obj objectForKey:@"message"] UTF8String];
        }];
        if(_delegate)
        {
            _delegate->chatListReceived(_chatList, [fuse_id UTF8String]);
        }
    }
    void chatListError(NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->chatListError(static_cast<kFuseChatErrors>([error intValue]));
        }
    }
    void purchaseVerification(NSNumber* verified, NSString* transactionID, NSString* originalTransactionID)
    {
        if(_delegate)
        {
            _delegate->purchaseVerification([verified intValue], [transactionID UTF8String], [originalTransactionID UTF8String]);
        }
    }
    void mailListRecieved(NSDictionary* messages, NSString* fuse_id)
    {
        _mailList.clear();
        [messages enumerateKeysAndObjectsUsingBlock: ^(id key, id obj, BOOL *stop) {
            auto &item = _mailList[[key UTF8String]];
            item.timestamp = [[obj objectForKey:@"timestamp"] intValue];
            item.alias = [[obj objectForKey:@"alias"] UTF8String];
            item.message = [[obj objectForKey:@"message"] UTF8String];
            item.gift_id = [[obj objectForKey:@"gift_id"] intValue];
            item.gift_name = [[obj objectForKey:@"gift_name"] UTF8String];
            item.gift_amount = [[obj objectForKey:@"gift_amount"] intValue];
        }];
        if(_delegate)
        {
            _delegate->mailListRecieved(_mailList, [fuse_id UTF8String]);
        }
    }
    void mailListError(NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->mailListError(static_cast<kFuseMailErrors>([error intValue]));
        }
    }
    void mailAcknowledged(NSNumber* message_id, NSString* fuse_id, NSNumber* request_id)
    {
        if(_delegate)
        {
            _delegate->mailAcknowledged([message_id intValue], [fuse_id UTF8String], [request_id intValue]);
        }
    }
    void mailError(NSNumber* error, NSNumber* request_id)
    {
        if(_delegate)
        {
            _delegate->mailError(static_cast<kFuseMailErrors>([error intValue]), [request_id intValue]);
        }
    }
    void adAvailabilityResponse(NSNumber* available, NSNumber* error)
    {
        if(_delegate)
        {
            _delegate->adAvailabilityResponse([available boolValue], static_cast<kFuseAdErrors>([error intValue]));
        }
    }
    void rewardedVideoCompleted(NSString* &zoneID)
    {
        if(_delegate)
        {
            _delegate->rewardedVideoCompleted([zoneID UTF8String]);
        }
    }
    
private:
    IOSFuseDelegate *_nativeDelegate;
    FuseDelegate* _delegate;
           
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

@implementation IOSFuseDelegate
-(void) sessionStartReceived
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).sessionStartReceived();
}
-(void) sessionLoginError:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).sessionLoginError(_error);
}

-(void) timeUpdated:(NSNumber*)_utcTimeStamp
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).timeUpdated(_utcTimeStamp);
}
-(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).accountLoginComplete(_type, _account_id);
}

-(void) accountLoginError:(NSNumber*)_error Account:(NSString*)_account_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).accountLoginError(_error, _account_id);
}

-(void) notificationAction:(NSString*)_action
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).notificationAction(_action);
}

-(void) notficationWillClose
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).notficationWillClose();
}

-(void) gameConfigurationReceived
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).gameConfigurationReceived();
}

-(void) friendAdded:(NSString*)_fuse_id Error:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendAdded(_fuse_id, _error);
}

-(void) friendRemoved:(NSString*)_fuse_id Error:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendRemoved(_fuse_id, _error);
}

-(void) friendAccepted:(NSString*)_fuse_id Error:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendAccepted(_fuse_id, _error);
}

-(void) friendRejected:(NSString*)_fuse_id Error:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendRejected(_fuse_id, _error);
}

-(void) friendsMigrated:(NSString*)_fuse_id Error:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendsMigrated(_fuse_id, _error);
}

-(void) friendsListUpdated:(NSDictionary*)_friendsList
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendsListUpdated(_friendsList);
}

-(void) friendsListError:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).friendsListError(_error);
}

-(void) chatListReceived:(NSDictionary*)_messages User:(NSString*)_fuse_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).chatListReceived(_messages, _fuse_id);
}

-(void) chatListError:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).chatListError(_error);
}

-(void) purchaseVerification:(NSNumber*)_verified TransactionID:(NSString*)_tx_id OriginalTransactionID:(NSString*)_o_tx_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).purchaseVerification(_verified, _tx_id, _o_tx_id);
}

-(void) mailListRecieved:(NSDictionary*)_messages User:(NSString*)_fuse_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).mailListRecieved(_messages, _fuse_id);
}

-(void) mailListError:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).mailListError(_error);
}

-(void) mailAcknowledged:(NSNumber*)_message_id User:(NSString*)_fuse_id RequestID:(NSNumber*)_request_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).mailAcknowledged(_message_id, _fuse_id, _request_id);
}

-(void) mailError:(NSNumber*)_error RequestID:(NSNumber*)_request_id
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).mailError(_error, _request_id);
}

-(void) adAvailabilityResponse:(NSNumber*)_available Error:(NSNumber*)_error
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).adAvailabilityResponse(_available, _error);
}

-(void) rewardedVideoCompleted:(NSString*) _zoneID
{
    static_cast<avalon::IOSFuseAPI&>(avalon::FuseAPI::getInstance()).rewardedVideoCompleted(_zoneID);
}

@end

@implementation IOSFuseAdDelegate
- (id) initWithDelegate:(avalon::FuseAdDelegate*)delegate
{
    self = [super init];
    if(self)
        _delegate = delegate;
    return self;
}

-(void) adWillClose
{
    if(_delegate)
        _delegate->adWillClose();
}
@end

@implementation IOSFuseOverlayDelegate
- (id) initWithDelegate:(avalon::FuseOverlayDelegate*)delegate
{
    self = [super init];
    if(self)
        _delegate = delegate;
    return self;
}

-(void) overlayWillClose
{
    if(_delegate)
        _delegate->overlayWillClose();
}
@end
