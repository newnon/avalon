#ifndef __FUSEBOXX_H__
#define __FUSEBOXX_H__

#include <string>
#include <vector>
#include <map>
#include <set>

namespace avalon {
    
enum class kFuseGender
{
    UNKNOWN = 0,        /// gender unknown
    MALE,               /// gender male
    FEMALE,             /// gender female
};

enum class kFuseGameDataErrors
{
    NONE = 0,         /// no error has occurred
    NO_ACCOUNT,       /// the user has not signed in to an online account
    NOT_CONNECTED,    /// the user is not connected to the internet
    REQUEST_FAILED,   /// there was an error in establishing a connection with the server
    XML_PARSE_ERROR,  /// data was received, but there was a problem parsing the xml
};

enum class kFuseErrors
{
    NONE = 0,            /// no error has occurred
    NOT_CONNECTED,       /// the user is not connected to the internet
    REQUEST_FAILED,      /// there was an error in establishing a connection with the server
    XML_PARSE_ERROR,     /// data was received, but there was a problem parsing the xml
};

enum class kFuseChatErrors
{
    NONE = 0,             /// no error has occurred
    NOT_CONNECTED,        /// the user is not connected to the internet
    REQUEST_FAILED,       /// there was an error in establishing a connection with the server
};

enum class kFuseFriendErrors
{
    NO_ERROR = 0,
    BAD_ID,
    NOT_CONNECTED,
    REQUEST_FAILED
};

enum class kFuseFriendsListErrors
{
    NO_ERROR = 0,
    SERVER_ERROR,
    NOT_CONNECTED,
    REQUEST_FAILED
};

enum class kFuseMailErrors
{
    NO_ERROR = 0,
    SERVER_ERROR,
    NOT_CONNECTED,
    REQUEST_FAILED
};

enum class kFuseAdErrors
{
    NO_ERROR = 0,
    NOT_CONNECTED,
    SESSION_FAILURE,
};

enum class kFuseEventErrors
{
    FUSE_EVENT_NO_ERROR =0,
    FUSE_EVENT_BAD_VALUE,
    FUSE_EVENT_NULL_PARAMETER
};

enum class kFuseLoginErrors
{
    NO_ERROR = 0,
    SERVER_ERROR,
    NOT_CONNECTED,
    REQUEST_FAILED,
    SESSION_FAILURE,
};
    
enum kFuseAccountType
{
    NONE = 0,
    GAMECENTER = 1,
    FACEBOOK = 2,
    TWITTER = 3,
    OPENFEINT = 4,
    USER = 5,
    EMAIL = 6,
    DEVICE_ID = 7,
    GOOGLE_PLAY = 8
};
    
struct FuseboxxFriendEntry
{
    bool pending;
    std::string alias;
    std::string fuse_id;
};
    
struct FuseboxxChatListEntry
{
    int id;
    std::string timestamp;
    std::string alias;
    std::string message;
};
    
struct FuseboxxMailListEntry
{
    int message_id;
    std::string timestamp;
    std::string alias;
    std::string message;
    int gift_id;
    std::string gift_name;
    int gift_amount;
};
    
class FuseDelegate
{
public:
    //optional
    /*!
     * @brief This method indicates when the server has acknowledged that a session has been established by the client device
     * @details When a delegate is registered with the FuseAPI::startSession:Delegate: method, and the acknowledgement of the connection has been received, this method is invoked.  This method is optional and is only intended to help in cases where this information is relevant to the application.
     * @see FuseAPI::startSession:Delegate: for more information on starting a session with a \<FuseDelegate\>
     */
    virtual void sessionStartReceived() {}
    
    /*!
     * @brief This method is invoked when an error has occurred when trying to start a session
     * @details To avoid cases where operations are continuing as if everything was being handled normally by the FuseAPI, this can be listened to and actions taken as a result.  This method is optional.
     * @param _error [NSNumber*] The error value corresponding to a value in kFuseErrors
     * @see FuseAPI::startSession:Delegate: for more information on starting a session with a \<FuseDelegate\>
     * @see kFuseErrors for more information on all possible error values
     */
    virtual void sessionLoginError(kFuseErrors error) {}
    
    /*!
     * @brief This method indicates that the server UTC time has been received by the client device
     * @details This method can be called both as a result of FuseAPI::utcTimeFromServer or automatically after a time event has been sent from the Fuse system (generally only happens when a session is started or resumed).  The time should only be treated a psuedo-accurate.  There are delays in propagating the time from the server to the device.
     
     @code
     
     -(void) timeUpdated:(NSNumber*)_utcTimeStamp
     {
     int utc_unix_timestamp = [utcTimeStamp intValue];
     }
     
     @endcode
     
     * @param _utcTimeStamp [int]
     * @see FuseAPI::utcTimeFromServer for more information on directly requesting the UTC time from the server
     * @see http://en.wikipedia.org/wiki/Unix_time for more information on Unix time
     * @see http://en.wikipedia.org/wiki/Coordinated_Universal_Time for more information on UTC time
     */
    virtual void timeUpdated(long long utcTimeStamp) {}
    
    /*!
     * @brief This method notifies the device that an account login request has been received by the server.
     * @details When a user logs in using one of the available FuseAPI account method, for instance FuseAPI::gameCenterLogin:, the server will send the client a notification once received.  This is to prevent any action being taken by the client before this has been received.
     * @param _type [(NSNumber*] Account type
     * @param _account_id [NSString*] The account ID of the user logged in
     * @see FuseAPI::gameCenterLogin: for a sample login method
     */
    virtual void accountLoginComplete(int type, const std::string &account_id) {}
    
    /*!
     * @brief This method notifies the device that an account login request has failed
     * @details When a user logs in using one of the available FuseAPI account method, for instance FuseAPI::gameCenterLogin:, the server will send the client a notification if there is any errors encountered.
     * @param _error [NSNumber*] The error value corresponding to a value in kFuseLoginErrors
     * @param _account_id [NSString*] The account ID of the user attempted to log in
     * @see FuseAPI::gameCenterLogin: for a sample login method
     * @since Fuse API version 1.29
     */
    virtual void accountLoginError(kFuseLoginErrors error, const std::string &account_id) {}
    
    /*!
     * @brief This method indicates that an action should be taken as a result of a notification being closed
     * @details To handle any possible action as a result of an in-game notification being displayed, a custom string can be configured as a part of the notification setup process in the Fuse dashboard.  This string can be any value that is recognized by the application.  For instance, a sample of this in action would be:
     
     @code
     
     -(void) notificationAction:(NSString*)_action
     {
     if ([_action isEqualToString:@"GO_TO_LEVEL_2"])
     {
     // Take the user to level 2 for instance
     }
     else if ([_action isEqualToString:@"GO_TO_SALE"])
     {
     // Take the user to the sale section of your store
     }
     }
     
     @endcode
     
     * @param _action [NSString*] The string value that indicates what action to take.  Custom and application specific.
     * @see FuseAPI::displayNotifications for more information on displaying in-game notifications
     */
    virtual void notificationAction(const std::string &action) {}
    
    /*!
     * @brief This method indicates that a visible notification is being closed
     
     * @see FuseAPI::displayNotifications for more information on displaying in-game notifications
     */
    virtual void notficationWillClose() {}
    
    /*!
     * @brief This method indicates that the game configuration values have been received by the client.
     * @details To avoid polling values before they are sent from the server, this callback can be used to determine when they are valid.  These values are update when a user starts or resumes a session.
     * @see FuseAPI::getGameConfigurationValue: for more information on reading game configuration values once valid
     */
    virtual void gameConfigurationReceived() {}
    
    /*!
     * @brief This method indicates the result of an addition of a friend to the Fuse friends list system
     * @details This method is optional, and only needs to be handled if the application needs to respond to errors in inviting friends. An example implementation would be:
     
     @code
     
     -(void) friendAdded:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     if ([_error intValue] == FUSE_ADD_FRIEND_NO_ERROR)
     {
     // no error has occurred in adding a friend
     }
     else
     {
     // an error has occurred
     }
     }
     
     @endcode
     
     @param _fuse_id [NSString*] The fuse ID of the account for which the friend was added to
     @param _error [int] The error value corresponding to the a value in kFuseAddFriendErrors
     @see kFuseAddFriendErrors for all possible error codes
     @see FuseAPI::addFriend: for more information on adding a friend
     @since Fuse API version 1.22
     */
    virtual void friendAdded(const std::string &fuse_id, kFuseFriendErrors error) {}
    
    /*!
     * @brief This method indicates the result of a removal of a friend to the Fuse friends list system
     * @details This method is optional, and only needs to be handled if the application needs to respond to errors in removing friends. An example implementation would be:
     
     @code
     
     -(void) friendRemoved:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     if ([_error intValue] == FUSE_REMOVE_FRIEND_NO_ERROR)
     {
     // no error has occurred in removing a friend
     }
     else
     {
     // an error has occurred
     }
     }
     
     @endcode
     
     @param _fuse_id [NSString*] The fuse ID of the account for which the friend was removed
     @param _error [int] The error value corresponding to the a value in kFuseRemoveFriendErrors
     @see kFuseRemoveFriendErrors for all possible error codes
     @see FuseAPI::removeFriend: for more information on removing a friend
     @since Fuse API version 1.22
     */
    virtual void friendRemoved(const std::string &fuse_id, kFuseFriendErrors error) {}
    
    /*!
     * @brief This method indicates the result of a acceptance of a friend to the Fuse friends list system
     * @details This method is optional, and only needs to be handled if the application needs to respond to errors in accepting friends. An example implementation would be:
     
     @code
     
     -(void) friendAccepted:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     if ([_error intValue] == FUSE_ACCEPT_FRIEND_NO_ERROR)
     {
     // no error has occurred in accepting a friend
     }
     else
     {
     // an error has occurred
     }
     }
     
     @endcode
     
     @param _fuse_id [NSString*] The fuse ID of the account for which the friend was accepted
     @param _error [int] The error value corresponding to the a value in kFuseAcceptFriendErrors
     @see kFuseAcceptFriendErrors for all possible error codes
     @see FuseAPI::acceptFriend: for more information on accepting a friend
     @since Fuse API version 1.22
     */
    virtual void friendAccepted(const std::string &fuse_id, kFuseFriendErrors error) {}
    
    /*!
     * @brief This method indicates the result of a rejection of a friend to the Fuse friends list system
     * @details This method is optional, and only needs to be handled if the application needs to respond to errors in rejecting friends. An example implementation would be:
     
     @code
     
     -(void) friendRejected:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     if ([_error intValue] == FUSE_REJECT_FRIEND_NO_ERROR)
     {
     // no error has occurred in accepting a friend
     }
     else
     {
     // an error has occurred
     }
     }
     
     @endcode
     
     @param _fuse_id [NSString*] The fuse ID of the account for which the friend was rejected
     @param _error [int] The error value corresponding to the a value in kFuseRejectFriendErrors
     @see kFuseRejectFriendErrors for all possible error codes
     @see FuseAPI::rejectFriend: for more information on rejecting a friend
     @since Fuse API version 1.22
     */
    virtual void friendRejected(const std::string &fuse_id, kFuseFriendErrors error) {}
    
    /*!
     * @brief This method indicates the result of a rejection of a request to migrate friends from one account to another
     * @details This method is optional, and only needs to be handled if the application needs to respond to errors in migrating friends. An example implementation would be:
     
     @code
     
     -(void) friendsMigrated:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     if ([_error intValue] == FUSE_MIGRATE_FRIENDS_NO_ERROR)
     {
     // no error has occurred in accepting a friend
     }
     else
     {
     // an error has occurred
     }
     }
     
     @endcode
     
     @param _fuse_id [NSString*] The fuse ID of the account for which the friend's are to be migrated from
     @param _error [int] The error value corresponding to the a value in kFuseMigrateFriendErrors
     @see kFuseMigrateFriendErrors for all possible error codes
     @see FuseAPI::migrateFriends: for more information on migrating friends
     @since Fuse API version 1.34.1
     */
    virtual void friendsMigrated(const std::string &fuse_id, kFuseFriendErrors error);
    
    /*!
     * @brief This method indicates when the friends list on the client has been updated from the server
     * @details Either on the first fetch or subsequent updates of the friends list, this method is called every time the friends list is updated on the client.  A friends list is requested from the server using FuseAPI::updateFriendsListFromServer and the process invokes this method with a dictionary of friends when complete.  A sample implementation would be:
     
     @code
     
     -(void) friendsListUpdated:(NSDictionary*)_friendsList
     {
     // The friends list has returned to the device
     
     // sample code to parse the dictionary
     NSArray *fuse_ids = [_friendsList allKeys];
     
     for (int i = 0; i < [fuse_ids count]; i++)
     {
     NSString *fuse_id = [fuse_ids objectAtIndex:i];
     
     NSDictionary *friendEntry = (NSDictionary*)[_friendsList objectForKey:fuse_id];
     
     NSLog(@"Friend is Pending: %d", [[friendEntry objectForKey:@"pending"] intValue]);
     NSLog(@"Friend Alias: %@", [friendEntry objectForKey:@"alias"]);
     NSLog(@"Friend Fuse ID: %@", [friendEntry objectForKey:@"fuse_id"]);
     }
     }
     
     @endcode
     
     * @param _friendsList
     * @see FuseAPI::updateFriendsListFromServer or more information on requesting the friends list from the server
     * @see FuseAPI::getFriendsList for more information on requesting the local copy of the friends list
     * @see friendsListError: to handle errors involved in requesting the friends list from the Fuse server
     * @since Fuse API version 1.22
     */
    virtual void friendsListUpdated(const std::map<std::string, FuseboxxFriendEntry> &friendsList) {}
    
    /*!
     * @brief This method indicates when an error has occurred in fetching the friends list from the server
     * @details This method is a result of an error originating from the invocation of FuseAPI::updateFriendsListFromServer and indicates an error has occurred somewhere in the process.
     
     @code
     
     -(void) friendsListError:(NSNumber*)_error
     {
     if ([_error intValue] != FUSE_FRIENDS_LIST_NO_ERROR)
     {
     // An error has occurred
     }
     }
     
     @endcode
     
     * @param _error [int] The error value corresponding to the a value in kFuseFriendsListErrors
     * @see FuseAPI::updateFriendsListFromServer or more information on requesting the friends list from the server
     * @since Fuse API version 1.22
     */
    virtual void friendsListError(kFuseFriendsListErrors error) {}
    
    /*!
     * @brief This method is called as a result of requesting a user's chat list from the server
     * @details When invoked, this method will include the data payload of the chat list in the form of a dictionary.  The structure of the dictionary is as follows:
     
     @code
     
     -(void) chatListReceived:(NSDictionary*)_messages User:(NSString*)_fuse_id
     {
     // Sample code for parsing the dictionary returned
     if (_messages != nil && [_messages count] > 0)
     {
     NSArray *keys = [_messages allKeys];
     NSArray *sortedKeys = [keys sortedArrayUsingSelector:@selector(compare:)];
     
     for (int i = 0; i < [sortedKeys count]; i++)
     {
     NSString *message_id = [sortedKeys objectAtIndex:i];
     NSMutableDictionary *entry = (NSMutableDictionary*)[_messages objectForKey:message_id];
     
     NSLog(@"%d [%@]: %@ -> %@", [message_id intValue], [entry objectForKey:@"timestamp"], [entry objectForKey:@"alias"], [entry objectForKey:@"message"]);
     }
     }
     }
     
     @endcode
     
     * @param _messages [NSDictionary*] The message list
     * @param _fuse_id [NSString*] The fuse ID for which the message list belongs
     * @see FuseAPI::getUserChatListFromServer: for more information on requesting the chat list and triggering this callback
     * @see chatListError: for more information on handling an error in requesting the chat list
     */
    virtual void chatListReceived(std::map<std::string,FuseboxxChatListEntry> &messages, const std::string &fuse_id) {}
    
    /*!
     @brief This method will be called in response to an error occurring for any \<FuseAPI\> chat list methods
     @param _error [NSNumber*] The error code corresponding to a value in kFuseChatErrors
     @see FuseAPI::postUserChatMessage:TargetUser: for more information on sending a chat message for a user
     @see kFuseChatErrors for more information on all of the error values
     */
    virtual void chatListError(kFuseChatErrors error) {}
    
    /*!
     * @brief This method indicates whether the registered in-app purchase has been validated by Apple's servers
     * @details This method is optional can indicates via the _verified bit if the in-app purchase was valid.  The Fuse servers initiate communication with Apple's in-app purchase verification servers once a call to FuseAPI::registerInAppPurchase: is invoked.  This callback is the result of that process.  If the _validated bit comes back as a '1', it can be safely concluded that the purchase is definitely valid.  However, if the bit comes back as '0', the purchase should only be treated as suspect as it is not definitive at this point whether it was invalid or an error occurred in the process.
     
     @code
     
     -(void) purchaseVerification:(NSNumber*)_verified TransactionID:(NSString*)_tx_id OriginalTransactionID:(NSString*)_o_tx_id
     {
     BOOL is_valid = [_verified boolValue];
     
     if (is_valid)
     {
     // valid transaction
     }
     else
     {
     // suspect transaction
     }
     }
     
     @endcode
     
     * @param _verified [NSSNumber*] This is the _verified bit: 1 indicates that the transaction was valid.  0 indicates that the transaction should be treated with suspicion.
     * @param _tx_id [NSString*] The transaction ID specified by Apple
     * @param _o_tx_id [NSString*] The original transaction ID specified by Apple (can be different than the transaction ID because the transaction could be a reinstatement of a previous purchase)
     * @see FuseAPI::registerInAppPurchase: for more information on how to invoke this process
     */
    virtual void purchaseVerification(int verified, const std::string &transactionID, const std::string &originalTransactionID) {}
    
    /*!
     * @brief This method is invoked when a mail/gift list is returned from the server
     * @details This method is called in response to a FuseAPI::getMailListFromServer or FuseAPI::getMailListFriendFromServer:.  To handle the returned list:
     
     @code
     
     -(void) mailListRecieved:(NSDictionary*)_messages User:(NSString*)_fuse_id;
     {
     if (_messages != nil && [_messages count] > 0)
     {
     NSArray *keys = [_messages allKeys];
     NSArray *sortedKeys = [keys sortedArrayUsingSelector:@selector(compare:)];
     
     for (int i = 0; i < [sortedKeys count]; i++)
     {
     NSString *message_id = [sortedKeys objectAtIndex:i];
     NSMutableDictionary *entry = (NSMutableDictionary*)[_messages objectForKey:message_id];
     
     NSLog(@"%d [%@]: %@ -> %@  [Gift ID: %d, Gift Name: %@, Gift Amount: %d]", [message_id intValue], [entry objectForKey:@"timestamp"], [entry objectForKey:@"alias"], [entry objectForKey:@"message"], [[entry objectForKey:@"gift_id"] intValue], [entry objectForKey:@"gift_name"], [[entry objectForKey:@"gift_amount"] intValue]);
     
     // Optional
     [FuseAPI setMailAsReceived:[message_id intValue]];
     }
     }
     }
     
     @endcode
     
     @param _messages [NSDictionary*] This is the list of messages/gifts
     @param _fuse_id [NSString*] The fuse ID for which the mail/gift list belongs
     @see FuseAPI::getMailListFromServer for more information on retrieving the mail list for the currently signed in user from the server
     @see FuseAPI::getMailListFriendFromServer: for more information on retrievinf the mail list for any user from the server
     @since FuseAPI version 1.25
     */
    virtual void mailListRecieved(std::map<std::string,FuseboxxMailListEntry> &messages, const std::string &fuse_id) {}
    
    /*!
     * @brief This method is called when an error has occurred fetching the list of mail/gift messages
     * @details If any error occurs while fetching the mail list from the server, this method is involed with an error code indicating the type of error.
     
     @code
     
     -(void) mailListError:(NSNumber*)_error
     {
     if ([_error intValue] != FUSE_MAIL_NO_ERROR)
     {
     // An error has occurred
     }
     }
     
     @endcode
     
     @param _error [NSNumber*] The error code corresponding to a value in kFuseMailErrors
     @see FuseAPI::getMailListFromServer for more information on retrieving the mail list for the currently signed in user from the server
     @see FuseAPI::getMailListFriendFromServer: for more information on retrievinf the mail list for any user from the server
     @since Fuse API version 1.25
     */
    virtual void mailListError(kFuseMailErrors error) {}
    
    /*!
     * @brief This method is called to acknowledge a successful sending of a mail/gift to another user
     * @details The method is called as a resulf of either FuseAPI::sendMail:Message: or FuseAPI::sendMailWithGift:Messge:GiftID:GiftAmount:.
     
     @param _message_id [NSString *] The ID of the mail message
     @param _fuse_id [NSString *] The Fuse ID to which the mail message was sent
     @param _request_id [NSNumber *] The request ID that was provided when the message was sent
     @see FuseAPI::sendMail:Message: for more information on sending a mail message
     @see FuseAPI::sendMailWithGift:Messge:GiftID:GiftAmount: for more information on sending a mail message with a gift
     @since Fuse API version 1.25
     */
    virtual void mailAcknowledged(int message_id, const std::string &fuse_id, int request_id) {}
    
    /*!
     * @brief This method is called when an error has occurred sending a mail messages
     * @details If any error occurs while sending a mail message, this method is involed with an error code indicating the type of error.
     
     @code
     
     -(void) mailError:(NSNumber*)_error RequestID:(NSNumber*)_request_id
     {
     if ([_error intValue] != FUSE_MAIL_NO_ERROR)
     {
     // An error has occurred
     }
     }
     
     @endcode
     
     @param _error [NSNumber*] The error code corresponding to a value in kFuseMailErrors
     @param _request_id [NSNumber *] The request ID that was provided when the message was sent
     @see FuseAPI::sendMail:Message: for more information on sending a mail message
     @see FuseAPI::sendMailWithGift:Messge:GiftID:GiftAmount: for more information on sending a mail message with a gift
     @since Fuse API version 1.25
     */
    virtual void mailError(kFuseMailErrors error, int request_id) {}
    
    /*!
     * @brief This method is called in response to a request to check for an ad in the Fuse system
     * @details As a result of the checkAdAvailable method, this method is invoked when the status of whether an ad is available is known.  To handle this response:
     
     @code
     
     -(void) adAvailabilityResponse:(NSNumber*)_available Error:(NSNumber*)_error
     {
     BOOL isAvailable = [_available boolValue];
     int error = [_error intValue];
     
     if (error != FUSE_AD_NO_ERROR)
     {
     // An error has occurred checking for the ad
     }
     else
     {
     if (isAvailable)
     {
     // An ad is available
     }
     else
     {
     // An ad is not available
     }
     }
     }
     
     @endcode
     
     * @param _available [NSNumber *] This indicates whether an ad is available (boolean)
     * @param _error [NSNumber *] This indicates whether an error has occurred and corresponds to values in kFuseAdErrors
     * @see checkAdAvailable for more information on how to invoke the process of checking for an ad
     * @since Fuse API version 1.26
     */
    virtual void adAvailabilityResponse(bool available, kFuseAdErrors error) {}
    
    
    /*!
     * @brief This method is called to acknowledge a successful rewarded Video Watch
     
     @param _zoneID [NSString *] The zone in which a rewarded video was watched in
     @since Fuse API version 1.37.0
     */
    virtual void rewardedVideoCompleted(const std::string &zoneID) {}
};
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * @brief This is Fuse API overlay delegate
 * @details This delegate handles any event from a fuse overlay.  Most commonly, an overlay will occur in the form of the "More Games" section.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FuseOverlayDelegate
{
    //@optional
    /*!
     * @brief This method indicates when a Fuse overlay will close
     * @details When an overlay is being displayed by the application, most commonly the "More Games" overlay, and has been dismissed by the user, this method will be called to indicate that the application can continue execution of the user flow or application (if applicable).  Often, the "More Games" section will be displayed over a menu, so the when the overlay is dismissed the menu will still be showing, thus not requiring the implementation of this method.
     * @see FuseAPI::displayMoreGames: for more information on calling the More Games overlay
     */
    virtual void overlayWillClose() {}
};
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * @brief This is Fuse API ad delegate
 * @details Handles all notifications involved with showing an ad and responding to an ad closing.
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FuseAdDelegate
{
    //@required
    /*!
     * @brief This method indicates when a full-screen (interstitial) ad is closing
     * @details When an ad is being dismissed by the user and control is to be returned to the application, this method will be called.  Once called, the application can continue execution of the user flow or application.
     * @see FuseAPI::showAdWithDelegate: for more information on displaying an ad with a \<FuseAdDelegate\>
     * @since FuseAPI version 1.12
     */
    virtual void adWillClose() = 0;
};
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * @brief This is the game data delegate which receives notifications from getting and setting user data.
 * @details Handles all notification events involved in sending and receiving per-user game data.  This is not required unless the methods FuseAPI::setGameData:Delegate: or FuseAPI::getGameData:Delegate: (or others of the same family) are implemented
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FuseGameDataDelegate
{
    //@required
    
    /*!
     * @brief This method is called when the request for game data has returned to the device.  This method does not receieve the request ID in the callback, unlike gameDataReceived:ForKey:Data:RequestID: which can be optionally used instead (if required).
     * @details The requested data is given as an input to the method.  This
     * @param _fuse_id [NSString*] The Fuse ID of the user for which the data was requested.  Can be different that the user signed in on the device if the data requested was for a friend or other user.
     * @param _key [NSString*] The master object key (if specified) for the data returning.  Can be 'nil'.
     * @param _data [NSMutableDictionary*] The data payload.
     * @see FuseAPI::getGameData:Delegate: for more information on retrieving game data
     * @see gameDataError: for more information on error cases involved with retrieving game data
     */
    virtual void gameDataReceived(const std::string &fuse_id, const std::string &key, const std::map<std::string,std::string> &data) = 0;
    
    //@optional
    
    /*!
     * @brief This method is called when the request for game data has returned to the device.
     * @details The requested data is given as an input to the method.  This method is the same as gameDataReceived:ForKey:Data: except that it also passes back the request ID that was given to the client when the request was made.  You can optionally use this method or gameDataReceived:ForKey:Data:.
     * @param _fuse_id [NSString*] The Fuse ID of the user for which the data was requested.  Can be different that the user signed in on the device if the data requested was for a friend or other user.
     * @param _key [NSString*] The master object key (if specified) for the data returning.  Can be 'nil'.
     * @param _data [NSMutableDictionary*] The data payload.
     * @param _request_id [NSNubmber*] The request ID
     * @see FuseAPI::getGameData:Delegate: for more information on retrieving game data
     * @see gameDataError: for more information on error cases involved with retrieving game data
     */
    virtual void gameDataReceived(const std::string &fuse_id, const std::string &key, const std::map<std::string,std::string> &data, int request_id) {}
    
    /*!
     * @brief This method indicates when an error has occurred when sending or receiving per-user game data information.
     * @details When an error occurs when trying to set or retrieve game data, this method will be thrown with an error code.  Either this method or gameDataError:RequestID: should be implemented to catch an error.  This method does not provide the request ID.
     * @param _error [NSNumber*] The error number corresponding to a value in kFuseGameDataErrors
     * @see FuseAPI::setGameData:Delegate: for more information on setting game data
     * @see FuseAPI::getGameData:Delegate: for more information on retrieving game data
     * @see kFuseGameDataErrors for information on all of the possible error cases
     */
    virtual void gameDataError(kFuseGameDataErrors error) {}
    
    /*!
     * @brief This method indicates when an error has occurred when sending or receiving per-user game data information.
     * @details When an error occurs when trying to set or retrieve game data, this method will be thrown with an error code.  Either this method or gameDataError:RequestID: should be implemented to catch an error.  This method provides the request ID.
     * @param _error [NSNumber*] The error number corresponding to a value in kFuseGameDataErrors
     * @param _request_id [NSNumber*] The request ID
     * @see FuseAPI::setGameData:Delegate: for more information on setting game data
     * @see FuseAPI::getGameData:Delegate: for more information on retrieving game data
     * @see kFuseGameDataErrors for information on all of the possible error cases
     */
    virtual void gameDataError(kFuseGameDataErrors error, int request_id) {}
    
    /*!
     * @brief This method indicates the server has acknowledged a save of per-user game data
     * @details When this method is called, it indicates that the server has successfully saved data sent from the client device.  
     * @param _request_id [int] The request ID that corresponds to the ID returned by a FuseAPI::setGameData:Delegate: method variant.
     * @see FuseAPI::setGameData:Delegate: for more information on setting game data
     */
    virtual void gameDataSetAcknowledged(int request_id) {}
};
    
class FuseAPI
{
public:

    #pragma mark Session Creation
    /*!
     * @brief This method is used to initiate all communication with the Fuse system (and register a \<FuseDelegate\>)
     * @details The startSession method is used to bootstrap all communications with the Fuse system. This should be called early in the applicationDidFinishLaunching method in your application delegate.  There is a second version of this method, startSession:Delegate: which performs the same operation as this method except it does not register a \<FuseDelegate\> delegate.
     
     An example call would be:
     
     @code
     - (void)applicationDidFinishLaunching:(UIApplication *)application
     {
     [FuseAPI startSession: @"YOUR API KEY" Delegate:DELEGATE_REFERENCE];
     
     ...
     }
     @endcode
     
     When a session has been established by Fuse system, a callback will be sent to the registered \<FuseDelegate\> object using the following method:
     
     @code
     -(void) sessionStartReceived;
     @endcode
     
     * @param _game_id [NSString*] This is the 36-character API key assigned by the Fuse system.  Your API key is generated when you add your App to the Fuse dashboard system.  It can be found in the configuration tab in a specific game, or in the "Integrate API" section of the dashboard.  The API key is a 36-digit unique ID of the form 'aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee'.
     * @param _delegate [id] The \<FuseDelegate\> object to be registered to receive protocol callbacks (optional - see startSession: without this parameter if registering a delegate is not needed)
     * @see FuseDelegate::sessionStartReceived for more information on the delegate method
     * @see FuseDelegate::sessionLoginError: for more information on handling errors occurred when trying to start a session
     * @param _registerForPush [BOOL] set whether startSession automatically registers for a push notification token. If _registerForPush is set to NO, you must manually call [FuseAPI registerForPushToken]
     */
    virtual void startSession(const std::string &game_id, FuseDelegate *delegate = nullptr, bool autoRegisterForPush = false) = 0;

    /*!
     * @brief This method is used to describe the platform the API is running on
     * @details The setPlatform method is an optional start up call for setting the platform the game is running on.
     
     An example call would be:
     
     @code
     - (void)applicationDidFinishLaunching:(UIApplication *)application
     {
     ...
     [FuseAPI startSession: @"YOUR API KEY"];
     [FuseAPI setPlatform: @"unity-ios"];
     
     ...
     }
     @endcode
     
     * @param _game_Platform [NSString*] platform string to identify the platform represented, - expected platforms unity , air , marmalade , native
     */

    virtual void setPlatform(const std::string &game_Platform) = 0;

    /*!
     * @brief This method is used to register an named event in the Fuse system.
     * @details This method logs the time and frequency of an event within a given game session.  The input string can be anything that is relevant to the design of the game but should be easily understandable when read by users in the Fuse system.
     
     It is advisable to avoid recording events at a high rate as this could negatively impact both application and server performance.  For example, a good practice would be to issue an event at the start of a level (i.e. 'Level 1') or when a purchase is made.  It would be unadvisable to issue any event in each draw loop as this would create a tremendous amount of overhead and server traffic.
     
     The maximum length of a registered event is 256 characters, and each application is limited to a maximum 1,000 separate named events.
     
     An example call would be:
     
     @code
     [FuseAPI registerEvent:@"Level 1 Started" withDict:myValues];
     @endcode
     
     * @param _message [NSString*] The event name to be logged
     * @param _dict [NSDictionary*] A dictionary of values associated with the event
     * @retval [int] Indicates whether the event information is valid.  Corresponds to kFuseEventErrors.
     */
    virtual int registerEvent(const std::string &message, const std::map<std::string,std::string> &dict) = 0;

    /*!
     * @brief This method will send a named event (with values) to the Fuse system for tracking
     * @details To log a named event in the fuse system, you can make the following method calls.  Note that any variable value sent will be summed in the Fuse system, while the other parameters will be counted.
     *
     * @code
     
     // with a dictionary
     NSDictionary *dict = [[NSDictionary alloc] initWithObjectsAndKeys:[NSNumber numberWithInt:256], @"Coins",
     [NSNumber numberWithInt:1000], @"XP",
     [NSNumber numberWithFloat:20.5f], @"Frame Rate",
     nil];
     
     [FuseAPI registerEvent:@"Levels" ParameterName:@"Level" ParameterValue:@"1" Variables:dict];
     
     // with no dictionary
     [FuseAPI registerEvent:@"System" ParameterName:@"Tutorial Level Reached" ParameterValue:@"2" Variables:nil];
     
     // with no parameters
     [FuseAPI registerEvent:@"Tutorial Finished" ParameterName:nil ParameterValue:nil  Variables:nil];
     
     * @endcode
     *
     * The maximum length of a registered event is 256 characters, and each application is limited to a maximum 1,000 separate named events.
     *
     * @param _name [NSString*] The event group name (i.e. "Levels")
     * @param _param_name [NSString*] The event parameter name (i.e. "Level")
     * @param _param_value [NSString*] The event parameter value (i.e. "1")
     * @param _variables [NSDictionary*] A list of key value pairs of variable names and values
     * @retval [int] Indicates whether the event information is valid.  Corresponds to kFuseEventErrors.
     * @since FuseAPI version 1.26
     */
    virtual int registerEvent(const std::string &name,const std::string &param_name, const std::string &param_value, const std::map<std::string,std::string> &variables) = 0;

    #pragma mark Game Crash Registration
    /*!
     * @brief This method is used to catch crashes within an app.
     * @details The registerCrash method should be used within a declare exception handler to send information in the case of a crash.  This method will only be able to catch certain types of failures, as it is dependent upon where the crash happens whether the application will use this handler.
     
     Add the following line to register an uncaught exception listener in your application delegate:
     
     @code
     -(void) applicationDidFinishLaunching:(UIApplication *)application
     {
     ...
     
     NSSetUncaughtExceptionHandler(&uncaughtExceptionHandler);
     
     ...
     }
     @endcode
     
     Now create a method that corresponds to the registered uncaught exception listener and put it at the top of the application delegate (outside of the class declaration).
     
     @code
     void uncaughtExceptionHandler(NSException *exception)
     {
     [FuseAPI registerCrash:exception];
     }
     @endcode
     
     @param _exception [NSException *] The exception object passed to the exception handler method by the system.
     
     */
    virtual void registerCrash(const std::string &name, const std::string &reason) = 0;


    #pragma mark In-App Purchase Logging

    /*!
     @brief This method records in-app purchases in the Fuse system without using the SKPaymentTransaction data type.
     
     @details Call this method directly after an in-app purchase is made once it has been confirmed that the transaction has occurred successfully.  Optimally, this should be done in the recordTransaction method in your SKPaymentTransactionObserver delegate.  However, since this version does not use the SKPaymentTransaction object, call this at the appropriate point just after a transaction has been completed by the user.  If SKPaymentTransaction is available, this function should use the registerInAppPurchase: method (that function is used in conjuntion with registerInAppPurchaseList: to automatically select the price and currency).  A callback is sent to the \<FuseDelegate\> delegate indicating whether the transaction was confirmed by Apple's in-app purchase system or whether it should be treated as suspect (optional).
     
     @code
     -(void) recordTransaction:(SKPaymentTransaction *)transaction
     {
     [FuseAPI registerInAppPurchase:transaction.transactionReceipt TxState:transaction.transactionState Price:@"10.99" Currency:@"USD" ProductID:transaction.payment.productIdentifier];
     
     ...
     }
     @endcode
     
     @param _receipt_data [NSData *] The data payload associated with the purchase.  This corresponds to the transactionReceipt member of the SKPaymentTransaction class.
     @param _tx_state [NSInteger] The transaction state of the purchase.  This corresponds to the transactionState member of the SKPaymentTransaction class.
     @param _price [NSString *] The price, without the currency symbol. (i.e. "1.99")
     @param _currency [NSString *] The currency of the transaction.  This must be of the form "USD" or "CAD" (for example) which correspond to ISO 4217 specifications.
     @param _product_id [NSString *] The product ID of the transaction.  This corresponds to the payment.productIdentifier field of the SKPaymentTransaction class.
     @param _tx_id [NSString *] The transaction ID of the purchase.  This corresponds to the transactionIdentifier member of the SKPaymentTransaction class.
     @see FuseDelegate::purchaseVerification:TransactionID:OriginalTransactionID: for more information on the \<FuseDelegate\> callback indicating whether the transaction was verified by Apple's servers
     @see http://en.wikipedia.org/wiki/ISO_4217 for more information on ISO 4217 currency codes.
     @see registerInAppPurchase: for more information on calling this function with the SKPaymentTransaction object.
     @since Fuse API version 1.29
     */
    virtual void registerInAppPurchase(const std::vector<uint8_t> &receipt_data, int tx_state, const std::string &price, const std::string &currency, const std::string &product_id, const std::string &tx_id) = 0;


    #pragma mark Fuse Interstitial Ads
    /*!
     @brief This method is used to display a Fuse interstitial ad within the application.
     @details The Fuse interstitial ad system can be used to deliver high-quality, full-screen ads within your application.  To call an ad, simply call the following method:
     
     @code
     [FuseAPI showAdWithDelegate:FuseAdDelegate_Reference];
     @endcode
     
     The object passed to the method must be an object that is subscribed to the \<FuseAdDelegate\> protocol.  This object will receive a callback when the ad is closing, signalling to the application that it has been handed back control.  To create a \<FuseAdDelegate\> object, change the interface declaration to add the \<FuseAdDelegate\> protocol:
     
     @code
     @interface YourAdObject : NSObject <FuseAdDelegate>
     {
     }
     @end
     
     @endcode
     
     In the implementation of the delegate object, add this delegate method:
     
     @code
     
     @implementation YourAdObject
     
     -(void) adWillClose
     {
     // Continue execution flow of your application
     }
     
     @end
     
     @endcode
     
     Note that it is best to not perform other actions that require UI access while an ad is being displayed.  This includes:
     
     - Signing in to Game Center
     - Displaying a Fuse in-game notification
     
     See the Fuse dashboard to configure advertisements for your game.
     
     @param _delegate [id] The \<FuseAdDelegate\> object that will handle receiving callbacks in response to ad actions.
     */
    virtual void showAdWithDelegate(FuseAdDelegate *delegate) = 0;

    /*!
     @brief This method displays a Fuse interstitial ad for a given ad zone.  Different ad zones can be configured via the Fuse Dashboard
     
     @code
     [FuseAPI showAdWithDelegate:FuseAdDelegate_Reference adZone:@"Level Completed"];
     @endcode
     
     The object passed to the method must be an object that is subscribed to the \<FuseAdDelegate\> protocol.  This object will receive a callback when the ad is closing, signalling to the application that it has been handed back control.  To create a \<FuseAdDelegate\> object, change the interface declaration to add the \<FuseAdDelegate\> protocol:
     
     @code
     @interface YourAdObject : NSObject <FuseAdDelegate>
     {
     }
     @end
     
     @endcode
     
     In the implementation of the delegate object, add this delegate method:
     
     @code
     
     @implementation YourAdObject
     
     -(void) adWillClose
     {
     // Continue execution flow of your application
     }
     
     @end
     
     @endcode
     
     @param _delegate [id] The \<FuseAdDelegate\> object that will handle receiving callbacks in response to ad actions.
     @param _zone [NSString*] The name of the ad zone to display.  You can configure different zones via the Fuse Dashboard.
     */
    virtual void showAdWithDelegate(FuseAdDelegate *delegate, const std::string &zone) = 0;


    /*!
     * @brief This method indicates whether an ad is available to be shown to the user
     * @details This method is optional and can be used to test if an ad is available for the last specified ad zone (or the default ad zone if one was not specified before) in the Fuse system before attempting to show an ad to the user.  If an ad is shown (using showAdWithDelegate:) without an ad unit available, the window will be dismissed.  To call this method:
     
     @code
     [FuseAPI checkAdAvailable];
     @endcode
     
     The response to this method is sent using the \<FuseDelegate\> protocol method adAvailabilityResponse:Error.  Note that a \<FuseDelegate\> object must be registered using startSession: to receive this callback.
     
     * @see FuseDelegate::adAvailabilityResponse:Error: for more information on handling the callback response
     * @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     * @since Fuse API version 1.26
     */
    virtual void checkAdAvailable() = 0;

    /*!
     * @brief This method indicates whether an ad is available to be shown to the user
     * @details This method is optional and can be used to test if an ad is available in the Fuse system before attempting to show an ad to the user.  If an ad is shown (using showAdWithDelegate:) without an ad unit available, the window will be dismissed.  This method differs from checkAdAvailable in that a delegate object can optionally be specified to allow for the callback to go to any object, not the specified FuseDelegate as done in checkAdAvailable.  To call this method:
     
     @code
     
     [FuseAPI checkAdAvailable:YourAdObject_instance];
     
     @endcode
     
     In the implementation of the object receiving the callback, add this method:
     
     @code
     
     @implementation YourAdObject
     
     -(void) adAvailabilityResponse:(NSNumber*)_available Error:(NSNumber*)_error
     {
     BOOL isAvailable = [_available boolValue];
     int error = [_error intValue];
     
     if (error != FUSE_AD_NO_ERROR)
     {
     // An error has occurred
     }
     else
     {
     if (isAvailable)
     {
     // ad is available, and now cached
     }
     }
     
     @end
     
     @endcode
     
     The response to this method is sent to the object and does not need to conform to any delegate protocol.
     
     * @since Fuse API version 1.27
     */
    virtual void checkAdAvailableWithDelegate(FuseAdDelegate *delegate) = 0;

    /*!
     * @brief This method indicates whether an ad is available to be shown to the user for the specified ad zone
     * @details This method is optional and can be used to test if an ad is available in the Fuse system before attempting to show an ad to the user.  If an ad is shown (using showAdWithDelegate:) without an ad unit available, the window will be dismissed.  To call this method:
     
     @code
     [FuseAPI checkAdAvailableWithDelegate:delegate withAdZone:@"Level Completed"];
     @endcode
     
     The response to this method is sent using the \<FuseDelegate\> protocol method adAvailabilityResponse:Error.  Note that a \<FuseDelegate\> object must be registered using startSession: to receive this callback.
     
     * @param (id<FuseAdDelegate>)_delegate the FuseDelegate that receives the ad availability callbacks
     * @param _adZone (NSString*) the ad zone to check ad availability for.  Pass in nil to get the default ad zone.
     * @see FuseDelegate::adAvailabilityResponse:Error: for more information on handling the callback response
     * @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     * @since Fuse API version 1.36
     */
    virtual void checkAdAvailableWithDelegate(FuseAdDelegate *delegate, const std::string &adZone) = 0;

    /*!
     * @brief This method preloads an ad for a specified ad zone
     * @details This method is optional and can be used to ensure that an ad is shown to the user in a timely manner.  For example, preload ads for a "Level Complete" ad zone just before the result screen is shown.  The ad will load in the background and will appear when [FuseAPI showAdWithDelegate:(id)_delegate adZone:(NSString*)_zone] is called.
     
     @code
     [FuseAPI preLoadAdForZone:@"Level Complete"];
     @endcode
     
     * @param _adZone (NSString*)the name to the ad zone to preload.  Pass in nil to preload the default ad zone.
     * @see FuseDelegate::adAvailabilityResponse:Error: for more information on handling the callback response
     * @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     * @since Fuse API version 1.36
     */
    virtual void preLoadAdForZone(const std::string &adZone) = 0;

    #pragma mark Notifications
    /*!
     @brief This method is used to display in-game Fuse notifications
     @details The Fuse notification system can be used to deliver textual system notifications to your users, promoting features of your application for example or promoting another application.  In addition, the Fuse system automatically configures notifications to rate your application in the App Store as well as upgrade your application when a new version is released.  It is best to call this method early in the application flow of your game, preferably on your main menu.  Optionally, an action can be assigned to the closing of the dialog to notify the application that an internal action should be taken.  In this case, the FuseDelegate::notificationAction: method would be called when the dialog is closing (only if the affirmative button is pressed).
     
     To display notifications:
     
     @code
     [FuseAPI displayNotifications];
     @endcode
     
     @see FuseDelegate::notificationAction: for more information on handling internal actions
     */
    virtual void displayNotifications() = 0;

    /*!
     @brief This method returns whether a Fuse notification is available to be viewed
     */
    virtual bool isNotificationAvailable() = 0;

    #pragma mark More Games
    /*!
     @brief This method is use to display the "More Games" section
     @details The "More Games" section can be used to showcase your own games or all games within the Fuse network (including yours!).  To call the "More Games" overlay, simply call:
     
     @code
     [FuseAPI displayMoreGames:FuseOverlayDelegate_Reference];
     @endcode
     
     The delegate reference is optional - it is only required to be registered if you want to take action on a callback (specifically, when the section is closing).  Often, this overlay is displayed on top of a main menu screen, and once closed the main menu will already be present so no action on close is required.  To create a \<FuseOverlayDelegate\> object, change the interface declaration to add the \<FuseOverlayDelegate\> protocol:
     
     @code
     @interface YourMoreGamesObject : NSObject <FuseOverlayDelegate>
     {
     }
     @end
     
     @endcode
     
     In the implementation of the delegate object, add this delegate method:
     
     @code
     
     @implementation YourMoreGamesObject
     
     -(void) overlayWillClose
     {
     // more games overlay has closed
     }
     
     @end
     
     @endcode
     
     @param _delegate [id] The \<FuseAdDelegate\> object that will handle receiving callbacks in response to the more games section signalling the application [optional - pass 'nil' if not required]
     @see FuseOverlayDelegate::overlayWillClose for more information on the delegate method call
     */
    virtual void displayMoreGames(FuseOverlayDelegate *delegate) = 0;

    #pragma mark Gender
    /*!
     * @brief This method registers a gender for the user
     * @details If a gender is known or suspected for a user, call the following method to assign a gender to the user:
     
     @code
     
     [FuseAPI registerGender:FUSE_GENDER_FEMALE];
     
     // The enumerated type definition is as follows:
     enum kFuseGender
     {
     FUSE_GENDER_UNKNOWN = 0,
     FUSE_GENDER_MALE,
     FUSE_GENDER_FEMALE,
     };
     
     @endcode
     
     @param _gender [int] The enumerated gender of the user
     */
    virtual void registerGender(kFuseGender gender) = 0;

    #pragma mark Age
    /*!
     * @brief This method registers an age for the user
     * @details If an age is known for a user, call the following method to assign an age to the user:
     
     @code
     
     [FuseAPI registerAge:24];
     
     @endcode
     
     @param _age [int] The age of the user, in years
     */
    virtual void registerAge(int age) = 0;

    /*!
     * @brief This method registers a user's birthday
     * @details If the birthday of a user in known for a user, call the following method to assign their birthday:
     
     @code
     
     [FuseAPI registerBithday:1978 Month:7 Day:9];
     
     @endcode
     
     @param _year [int] The year in which the user was born
     @param _month [int] The month in which the user was born
     @param _day [int] The day on which the user was born
     */
    virtual void registerBirthday(int year, int month, int day) = 0;

    #pragma mark Account Login methods

    /*!
     @brief This method returns the public 'Fuse ID'.
     @details After a user has registered a login for one of the supported services (i.e. Game Center, etc), a 9-digit 'Fuse ID' is generated that uniquely identifies the user.  This ID can be passed between users as a public ID for the Fuse system so that user's can interact (i.e. invite as friends, etc.) without exposing confidential account information.
     
     @code
     
     NSString *my_fuse_id = [FuseAPI getFuseID];
     
     @endcode
     
     @see gameCenterLogin: for more information on how to register a login with a Game Center ID
     @see facebookLogin: for more information on how to register a login with a Facebook account ID
     @see twitterLogin: for more information on how to register a login with a Twitter account ID
     @see openFeintLogin: for more information on how to register a login with an OpenFeint account ID
     @see fuseLogin:Alias: for more information on how to register a login with a Fuse ID
     @retval [NSString*] The 9-digit Fuse ID.  This ID is strictly comprised of integers, but *do not* cast this value to an integer because a valid ID could have leading zeroes.
     @since Fuse API version 1.21
     */
    virtual const std::string& getFuseID() const = 0;

    /*!
     * @brief Game Center account registration
     * @details Uniquely track a user across devices by passing Game Center login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     To register the account information, pass the Game Center object as follows as soon as the user has been confirmed to have logged in.  This example shows the Fuse API method being called in sample Game Center login code:
     
     @code
     
     GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
     
     [localPlayer authenticateWithCompletionHandler:^(NSError *error)
     {
     if (localPlayer.isAuthenticated)
     {
     [FuseAPI gameCenterLogin:localPlayer];
     }
     }];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _lp [GKLocalPlayer*] This is the returned Game Center object from the Game Center completion handler
     @since Fuse API version 1.14
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     */
    virtual void gameCenterLogin() = 0;

    /*!
     * @brief Facebook account registration
     * @details Uniquely track a user across devices by passing Facebook login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     To call this method:
     
     @code
     
     [FuseAPI facebookLogin:@"facebook_id"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _facebook_id [NSString*] This is the account id of the user signed in to Facebook (e.g. 122611572)
     @param _name [NSString*] The first and last name of the user (i.e. "Jon Jovi").  Can be "" or nil if unknown.
     @param _accesstoken [NSString*] This is the access token generated if a user signs in to a facebook app on the device (can be "" or nil if not available)
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     @since Fuse API version 1.23
     */
    virtual void facebookLogin(const std::string &facebook_id, const std::string &name, const std::string &accesstoken) = 0;

    /*!
     * @brief Facebook account registration
     * @details Uniquely track a user across devices by passing Facebook login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.  Use this version if the gender of the player is known.
     
     To call this method:
     
     @code
     
     [FuseAPI facebookLogin:@"facebook_id", Name:"Jon Bon" Gender:2 withAccessToken:@"8971634a47d0b"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _facebook_id [NSString*] This is the account id of the user signed in to Facebook (e.g. 122611572)
     @param _name [NSString*] The first and last name of the user (i.e. "Jon Jovi").  Can be @"" or nil if unknown.
     @param _gender [int] The suspected gender of the user.  Please see kFuseGender for more information on the gender enumerated type.
     @param _accesstoken [NSString*] This is the access token generated if a user signs in to a facebook app on the device (can be @"" or nil if not available)
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     @since Fuse API version 1.23
     */
    virtual void facebookLogin(const std::string &facebook_id, const std::string &name, kFuseGender gender, const std::string &accesstoken) = 0;

    /*!
     * @brief Twitter account registration
     * @details Uniquely track a user across devices by passing Twitter login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     To call this method:
     
     @code
     
     [FuseAPI twitterLogin:@"twit_id"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _twitter_id [NSString*] This is the account id of the user signed in to Twitter
     @since Fuse API version 1.14
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     */
    virtual void twitterLogin(const std::string &twitter_id) = 0;

    /*!
     * @brief Twitter account registration with user name
     * @details Uniquely track a user across devices by passing Twitter login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     To call this method:
     
     @code
     
     [FuseAPI twitterLogin:@"twit_id" Name:@"JohnnyGoodUser"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _twitter_id [NSString*] This is the account id of the user signed in to Twitter
     @param _alias [NSString*] This is the alias of the user
     @since Fuse API version 1.14
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     */
    virtual void twitterLogin(const std::string &twitter_id, const std::string &alias) = 0;

    /*!
     * @brief OpenFeint account registration
     * @details Uniquely track a user across devices by passing OpenFeint login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     To call this method:
     
     @code
     
     [FuseAPI openFeintLogin:@"of_id"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _openfeint_id [NSString*] This is the account id of the user signed in to OpenFeint
     @since Fuse API version 1.14
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     */
    virtual void openFeintLogin(const std::string &openfeint_id) = 0;

    /*!
     * @brief Fuse account registration
     * @details Uniquely track a user across devices by passing Fuse login information of a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     The Fuse ID is a nine-digit numeric value that is unique to every signed-in player (but not unique to device).  Note that this method required UI elements to allow a user to provide credentials to log in, and is currently not implemented.
     
     To call this method:
     
     @code
     
     [FuseAPI fuseLogin:@"012345678"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _fuse_id [NSString*] This is the account id of the user signed in to the Fuse system
     @param _alias [NSString*] The alias or 'handle' of the user
     @since Fuse API version 1.14
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     @see getFuseID for more information on retrieving the user's Fuse ID once signed in
     */
    virtual void fuseLogin(const std::string &fuse_id, const std::string &alias) = 0;

    /*!
     * @brief Account registration using an email address
     * @details Uniquely track a user across devices by passing an email address for a user.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user information across devices.
     
     
     To call this method:
     
     @code
     
     [FuseAPI emailLogin:@"honky@gmail.com" Alais:@"Geronimo"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _email [NSString*] This is the email address of the user signed in to the Fuse system
     @param _alias [NSString*] The alias or 'handle' of the user
     @since Fuse API version 1.25
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     @see getFuseID for more information on retrieving the user's Fuse ID once signed in
     */
    virtual void emailLogin(const std::string &email, const std::string &alias) = 0;

    /*!
     * @brief Account registration using the unique device identifier
     * @details Uniquely track a user based upon their device identifier.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user. However, this system cannot track users across devices since it is tied to a device.  The main benefit to using this call to "log" a user in to the system is to avoid any other sign-in (like Facebook or Game Center).
     
     To call this method:
     
     @code
     
     [FuseAPI deviceLogin:@"Geronimo"];
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _alias [NSString*] The alias or 'handle' of the user
     @since Fuse API version 1.25
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     @see getFuseID for more information on retrieving the user's Fuse ID once signed in
     */
    virtual void deviceLogin(const std::string &alias) = 0;

    /*!
     * @brief Account registration using the google play login identifier
     * @details Uniquely track a user based upon their google play identifier.  This system can be used in conjunction with the 'set' and 'get' game data to persist per-user. When signing in to the Google Play system, the kGTLAuthScopePlusLogin scope must be specified to get the user's friends list:
     
     GPPSignIn *signIn = [GPPSignIn sharedInstance];
     signIn.clientID = kClientId;
     signIn.scopes = [NSArray arrayWithObjects: kGTLAuthScopePlusLogin, nil];
     
     To call this method:
     
     @code
     
     -(void)finishedWithAuth: (GTMOAuth2Authentication *)auth error: (NSError *)error
     {
     // Ensure we have no errors and we have a valid auth object
     if (error.code == 0 && auth) {
     
     [FuseAPI googlePlayLogin:@"TommyBoy" AccessToken:[auth accessToken]];
     
     ...
     
     } else {
     ...
     }
     }
     
     @endcode
     
     If required, a callback is sent to the \<FuseDelegate\> (if registered) indicating that the Fuse system has received the login information.
     
     @code
     
     -(void) accountLoginComplete:(NSNumber*)_type Account:(NSString*)_account_id;
     
     @endcode
     
     @param _alias [NSString*] The alias or 'handle' of the user
     @param _token [NSString*] The user's access token.  Used to retrieve friends lists
     @see startSession:Delegate: to see how to register a \<FuseDelegate\> object to receive the optional callback
     @see FuseDelegate::accountLoginComplete:Account: to see more information on the account complete callback
     @see getFuseID for more information on retrieving the user's Fuse ID once signed in
     @since Fuse API version 1.29
     */
    virtual void googlePlayLogin(const std::string &alias, const std::string &token) = 0;

    /*!
     * @brief Get the original account ID used to log in to the Fuse system that corresponds to the Fuse ID
     * @details This method returns the original parameter used to create the user account session.
     
     To call this method
     
     @code
     
     NSString *originalID = [FuseAPI getOriginalAccountID];
     
     @endcode
     
     * @retval [NSString*] The original account ID used to sign in to the fuse system (for instance 122611572 if the user is signed in using Facebook)
     * @see getOriginalAccountType to get the type associated with the account ID
     * @see getOriginalAccountAlias to get the alias associated with the account ID
     * @since Fuse API version 1.23
     */
    virtual std::string getOriginalAccountID() = 0;

    /*!
     * @brief Get the original account type used to log in to the Fuse system that corresponds to the Fuse ID
     * @details This method returns the type of account used to create the user account session.
     
     To call this method
     
     @code
     
     int type = [FuseAPI getOriginalAccountType];
     
     // where type corresponds to the following enum:
     
     enum kFuseAccountType
     {
     FUSE_ACCOUNT_NONE = 0,
     FUSE_GAMECENTER = 1,
     FUSE_FACEBOOK = 2,
     FUSE_TWITTER = 3,
     FUSE_OPENFEINT = 4,
     FUSE_USER = 5,
     FUSE_EMAIL = 6,
     FUSE_DEVICE_ID = 7,
     FUSE_GOOGLE_PLAY = 8
     };
     
     @endcode
     
     * @retval [int] The original account type used to sign in to the fuse system (for instance 4 if the user is signed in using Facebook)
     * @see getOriginalAccountID to get the ID associated with the account type
     * @see getOriginalAccountAlias to get the alias associated with the account ID
     * @since Fuse API version 1.23
     */
    virtual kFuseAccountType getOriginalAccountType() = 0;

    /*!
     * @brief Get the original account alias of the user used to log in to the Fuse system
     * @details This method returns the original user alias.
     
     To call this method
     
     @code
     
     NSString *alias = [FuseAPI getOriginalAccountAlias];
     
     @endcode
     
     * @retval [NSString*] The user's account alias (i.e. T-Bone300)
     * @see getOriginalAccountID to get the ID associated with the account type
     * @see getOriginalAccountType to get the type associated with the account ID
     * @since Fuse API version 1.29
     */
    virtual std::string getOriginalAccountAlias() = 0;

    #pragma mark Miscellaneous
    /*!
     * @brief This method returns the amount of times the user has opened the application
     * @details Call this method to get the number of times the application has been opened either from the Springboard of system tray (minimized)
     
     @code
     
     int played = [FuseAPI gamesPlayed];
     
     @endcode
     
     * @retval [int] The number of times the application has been opened
     */
    virtual int gamesPlayed() = 0;

    /*!
     * @brief This method returns the Fuse API version
     * @details Call this method if it is required to know the Fuse API version.
     
     @code
     
     NSString *api_ver = [FuseAPI libraryVersion];
     
     @endcode
     
     * @retval [NSString*] The API version of the form '1.22'
     */
    virtual const std::string& libraryVersion() = 0;


    /*!
     * @brief This method indicates whether the application is connected to the internet
     * @details This method indicates if the application is connected via wifi or cellular network and connected to the internet. To use this method:
     
     @code
     
     BOOL is_connected = [FuseAPI connected];
     
     @endcode
     
     @retval [BOOL] The connected status of the application
     */
    virtual bool connected() = 0;

    /*!
     * @brief This method gets the UTC time from the server
     * @details To help determine the psuedo-accurate real-world time (i.e. not device time), this method can be called to get the UTC time from the Fuse servers.  The date is returned in unix time format (i.e. seconds elapsed since January 1, 1970).  The returned value is only psuedo-accurate in that it does not account for request time and delays - so it is the time on the server when the request was received but not the time when the value returns to the device.  This is generally used to prevent time exploits in games where such situations could occur (by a user changing their device time).
     
     To get the time, it is a two step process.  First a request is made to the API:
     
     @code
     
     [FuseAPI utcTimeFromServer];
     
     @endcode
     
     Then, a callback is triggered in the \<FuseDelegate\> with the result:
     
     @code
     
     -(void) timeUpdated:(NSNumber*)_utcTimeStamp
     {
     int server_time = [utcTimeStamp intValue];
     }
     
     @endcode
     
     @see startSession: or startSession:Delegate: to see how to register a \<FuseDelegate\>
     @see FuseDelegate::timeUpdated: to understand more about the \<FuseDelegate\> callback
     @see http://en.wikipedia.org/wiki/Unix_time for more information on Unix time
     @see http://en.wikipedia.org/wiki/Coordinated_Universal_Time for more information on UTC time
     */
    virtual void utcTimeFromServer() = 0;

    /*!
     * @brief This method indicates whether the Fuse API has concluded all necessary work before being able to be closed.
     * @details
     
     @code
     
     BOOL is_not_done = [FuseAPI notReadyToTerminate];
     
     @endcode
     
     @retval [BOOL] Indicates wether the Fuse API is still working on sending out final requests.
     */
    virtual bool notReadyToTerminate() = 0;

    #pragma mark Data Opt In/Out
    /*!
     * @brief This method opts a user out of data being collected by the API.
     * @details In accordance with Apple's terms of service, a user should always have the option to not have data collected on their play usage.  To allow a user to opt out, call the following method:
     
     @code
     [FuseAPI disableData];
     @endcode
     
     While it is necessary to allow a user to opt in and out of data collection, the implementation of this method is optional as there is another way to allow a user to stop data collection.  By using a settings bundle, which appears in the "Settings" menu for the application, data collection can be toggled without adding any code in the binary.  Many developers find this an easier and less intrusive way to integrate this feature.  This file can be found on the dashboard in the "Integrate API" section, or at this link:
     
     [https://www.fuseboxx.com/api/Settings.bundle.zip](https://www.fuseboxx.com/api/Settings.bundle.zip)
     
     @see enableData to understand how to enable collecting data
     @see Download https://www.fuseboxx.com/api/Settings.bundle.zip to integrate the settings bundle and avoid having to implement this method
     */
    virtual void disableData() = 0;

    /*!
     * @brief This method opts a user in so that data is collected by the API.
     * @details To allow a user to opt in, call the following method:
     
     @code
     [FuseAPI enableData];
     @endcode
     
     @see disableData to understand how to disable collecting data and more information on using the settings bundle
     @see Download https://www.fuseboxx.com/api/Settings.bundle.zip to integrate the settings bundle and avoid having to implement this method
     */
    virtual void enableData() = 0;

    /*!
     * @brief This method indicates whether the user is opted-in to collecting data.
     * @details To see if a user has indicated whether they want data collected:
     
     @code
     BOOL is_opted_in = [FuseAPI dataEnabled];
     @endcode
     
     @retval [BOOL] Indicates if the user has enabled data to be collected
     */
    virtual bool dataEnabled() = 0;

    #pragma mark User Game Data
    /*!
     @brief This method is used to store per-user persistent game data on the Fuse servers while specifying a mater key value for the key->value pairs in the dictionary.
     @details User information is stored in key->value pairs.  A dictionary is used to store the key value pairs that are passed in to the method.  Keys in the dictionary must be strings, while value data types fall in to two categories: the first being variables that can be cast to strings (int, bool, float, string), while the second is binary data in the form of a NSData* object.  There is currently no restriction on the length of the value passed to the server, however if requests become so large they could create problems and this policy will have to be changed.  To receive information on whether the call was made successfully, a \<FuseGameDataDelegate\> delegate can be optionally passed to the method (pass 'nil' if not required).  An example of how to create a dictionary and send it to the Fuse system is as follows:
     
     @code
     
     NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:4];
     
     [dict setValue:[NSNumber numberWithInt:currentPoints] forKey:@"points"];
     [dict setValue:[NSNumber numberWithBool:hasPlayed] forKey:@"played"];
     [dict setValue:userName forKey:@"name"];
     
     NSData *binaryData = [[NSData alloc] initWithBytes:ptr length:1024];
     
     [dict setValue:binaryData forKey:@"binary_blob"];
     
     [FuseAPI setGameData:dict Delegate:self];
     
     @endcode
     
     To register a \<FuseGameDataDelegate\> delegate, create a class with the following interface:
     
     @code
     @interface YourDataObject : NSObject <FuseGameDataDelegate>
     {
     }
     @end
     
     @endcode
     
     In the implementation of the delegate object, add this delegate method to receive set data callbacks:
     
     @code
     
     @implementation YourDataObject
     
     -(void) gameDataError:(NSNumber*)_error
     {
     // An error has occurred in setting game data
     // see kFuseGameDataErrors for all error values
     int error = [_error intValue];
     }
     
     -(void) gameDataSetAcknowledged:(NSNumber*)_request_id
     {
     // An acknowledgement has been received by the device from the server indicating that the data has been updated
     int request_id = [_request_id intValue];
     }
     
     @end
     
     @endcode
     
     @param _data [NSMutableDictionary*] The data to be stored on the server
     @param _delegate [id] The \<FuseGameDataDelegate\> delegate to receive callbacks (optional - pass 'nil' if not required)
     @retval [int] Request number.  This is the request ID that can be correlated in gameDataSetAcknowledged: to know whether the request was successful.
     @see setGameData:Key:Delegate: for more information on setting data with a parent key defining the set of key->value pairs
     @see kFuseGameDataErrors for all possible game data error values
     @since Fuse API version 1.15
     */
    virtual int setGameData(const std::map<std::string,std::string> &data, FuseGameDataDelegate *delegate) = 0;

    /*!
     * @brief This method is used to store per-user persistent game data on the Fuse servers while specifying a master key value for the key->value pairs in the dictionary.
     * @details This method is similar to setGameData:Delegate: except that a parent key value can be sent along with the dictionary to group data.  For instance, if you have a game with cars and you need to send information on each car that a user has, a dictionary could be sent for each car using a differentiating parent key value to separate the dictionary from others.  In addition, this method *can not* be used to send binary data unlike  setGameData:Delegate:.
     
     A sample implementation of this method call would be:
     
     @code
     
     NSMutableDictionary *dict = [[NSMutableDictionary alloc] initWithCapacity:2];
     
     [dict setValue:[NSNumber numberWithInt:speed] forKey:@"max_speed"];
     [dict setValue:[NSNumber numberWithBool:power] forKey:@"total_power"];
     
     [FuseAPI setGameData:dict Key:@"car_3" Delegate:self];
     
     @endcode
     
     @param _data [NSMutableDictionary*] The data to be stored on the server
     @param _key [NSString*] The parent key of the data set
     @param _delegate [id] The \<FuseGameDataDelegate\> delegate to receive callbacks (optional - pass 'nil' if not required)
     @retval [int] Request number.  This is the request ID that can be correlated in gameDataSetAcknowledged: to know whether the request was successful.
     @see setGameData:Delegate: for more information on configuring a \<FuseGameDataDelegate\> delegate
     @see kFuseGameDataErrors for all possible game data error values
     @since Fuse API version 1.15
     */
    virtual int setGameData(const std::map<std::string,std::string> &data, const std::string &key, FuseGameDataDelegate *delegate) = 0;

    /*!
     * @brief This method is used to store per-user persistent game data on the Fuse servers while indicating whether the dictionary is a dictionary of dictionaries.
     * @details In the case where a great amount of data is to be sent at once, dictionaries can be grouped into a master dictionary that can be sent to the server in an atomic transaction.  This is useful in reducing server overhead as well as ensuring data is received by the server when the app is closing.
     
     A sample of this would be:
     
     @code
     
     NSMutableDictionary *dict1 = [[NSMutableDictionary alloc] initWithCapacity:2];
     
     [dict1 setValue:[NSNumber numberWithInt:speed] forKey:@"max_speed"];
     [dict1 setValue:[NSNumber numberWithBool:power] forKey:@"total_power"];
     
     NSMutableDictionary *dict2 = [[NSMutableDictionary alloc] initWithCapacity:2];
     
     [dict2 setValue:[NSNumber numberWithInt:level] forKey:@"level"];
     [dict2 setValue:[NSNumber numberWithBool:points] forKey:@"points"];
     
     NSMutableDictionary *masterDict = [[NSMutableDictionary alloc] initWithCapacity:2];
     
     [masterDict setValue:dict1 forKey:@"car_3"];
     [masterDict setValue:dict2 forKey:@"level_info"];
     
     [FuseAPI setGameData:masterDict Key:@"" Delegate:self IsCollection:YES];
     
     @endcode
     
     Note that this method is equavalent to setGameData:Key:Delegate: if IsCollection == NO.
     
     @param _data [NSMutableDictionary*] The data to be stored on the server
     @param _key [NSString*] The parent key of the data set - ignored if IsCollection == YES)
     @param _delegate [id] The \<FuseGameDataDelegate\> delegate to receive callbacks (optional - pass 'nil' if not required)
     @param _collection [BOOL] Indicates whether the dictionary being passed to the method is a dictionary of dictionaries (YES), or a dictionary of key->value pairs (NO)
     @retval [int] Request number.  This is the request ID that can be correlated in gameDataSetAcknowledged: to know whether the request was successful.
     @see setGameData:Delegate: for more information on configuring a \<FuseGameDataDelegate\> delegate
     @since Fuse API version 1.18
     */
    virtual int setGameData(const std::map<std::string,std::string> &data, const std::string &key, FuseGameDataDelegate *delegate, bool collection) = 0;

    /*!
     @brief This method is used to store per-user persistent game data on the Fuse servers for another user.
     @details Use this method to set data for another user, not the user currently logged in.  This is a rare case in terms of user data in that generally a game session would only set data for the user playing.  Use this method carefully.
     
     @param _data [NSMutableDictionary*] The data to be stored on the server
     @param _fuse_id [NSString*] The public Fuse ID of the user.
     @param _key [NSString*] The parent key of the data set
     @param _delegate [id] The \<FuseGameDataDelegate\> delegate to receive callbacks (optional - pass 'nil' if not required)
     @param _collection [BOOL] Indicates whether the dictionary being passed to the method is a dictionary of dictionaries (YES), or a dictionary of key->value pairs (NO)
     @retval [int] Request number.  This is the request ID that can be correlated in gameDataSetAcknowledged: to know whether the request was successful.
     @see setGameData:Delegate: for more information on configuring a \<FuseGameDataDelegate\> delegate
     @see setGameData:Key:Delegate:IsCollection: for information on how to use the method
     @since Fuse API version 1.15
     */
    virtual int setGameData(const std::map<std::string,std::string> &data, const std::string &fuse_id, const std::string &key, FuseGameDataDelegate *delegate, bool collection) = 0;

    /*!
     @brief This method is used to retrieve per-user persistent game data on the Fuse servers.
     @details This method will retrieve data that is not grouped by master key (see getGameData:Key:Delegate for how to get data grouped by master key).  Data can be retrieved in two different methods: by specifying an array of keys (i.e. a subset of the entire data set), or by passing 'nil' for the _keys value which will return the entire data set.  To request data, call the following method:
     
     @code
     
     // With specifying a subset of information
     [FuseAPI getGameData:[NSMutableArray arrayWithObjects:@"score", @"level", nil] Delegate:self];
     
     // Getting all data
     [FuseAPI getGameData:nil Delegate:self];
     
     @endcode
     
     The data is returned via a callback to the \<FuseGameDataDelegate\> object.  The delegate object *must* passed in to this method in order to get the data returned.  For information on how to create a delegate, please see the definition of setGameData:Delegate:.
     
     The delegate method called in the case of a callback is as follows:
     
     @code
     
     @implementation YourDataObject
     
     -(void) gameDataError:(NSNumber*)_error
     {
     // An error has occurred in getting game data
     // see kFuseGameDataErrors for all error values
     int error = [_error intValue];
     }
     
     -(void) gameDataReceived:(NSString*)_fuse_id ForKey:(NSString*)_key Data:(NSMutableDictionary*)_data
     {
     // Data has returned for the user specified by '_fuse_id'.
     // Key can optionally by 'nil' or an empty string
     }
     
     @end
     
     @endcode
     
     @param _keys [NSArray*] The subset of keys to request from the server.  If 'nil' all key->value pairs will be returned.
     @param _delegate [id] The \<FuseGameDataDelegate\> to which the callback will be made when data has returned to the device.
     @retval [int] This is the request ID.
     @see getGameData:Key:Delegate: for how to receive data grouped by master key
     @see setGameData:Delegate: for more information on configuring a \<FuseGameDataDelegate\> delegate
     @see gameDataReceived:ForKey:Data: for more information on the delegate callback
     @since Fuse API version 1.15
     */
    virtual int getGameData(const std::vector<std::string> &keys, FuseGameDataDelegate *delegate) = 0;

    /*!
     @brief This method is used to retrieve per-user persistent game data on the Fuse servers while specifying a master key value for the key->value pairs in the dictionary.
     @details Similar to getGameData:Delegate, this method allows the specification of a master key to get grouped data.
     
     @code
     
     // With specifying a subset of information with a key specified
     [FuseAPI getGameData:[NSMutableArray arrayWithObjects:@"max_speed", @"total_power", nil] Key:@"car_info" Delegate:self];
     
     // Getting all data without a key specified
     [FuseAPI getGameData:nil Key:@"car_info" Delegate:self];
     
     @endcode
     
     @param _keys [NSArray*] The subset of keys to request from the server.  If 'nil' all key->value pairs will be returned.
     @param _key [NSString*] This is the master key name.
     @param _delegate [id] The \<FuseGameDataDelegate\> to which the callback will be made when data has returned to the device.
     @retval [int] This is the request ID.
     @see getGameData:Delegate: for more information on make a request, setting up the delegate and receiving a request
     @since Fuse API version 1.15
     */
    virtual int getGameData(const std::vector<std::string> &keys, const std::string &key, FuseGameDataDelegate *delegate) = 0;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*!
     @brief This method retrieves persistent data for a friend of the current user from the Fuse server.
     @details Similar to getGameData:Delegate: except with a parameter to specify "Fuse ID", this method retrieves information on another user (not the user signed in on the device).  Typically, this is used for social features such as visiting another user's "world".
     
     @code
     
     [FuseAPI getGameData:[NSMutableArray arrayWithObjects:@"max_speed", @"total_power", nil] FuseID:@"012345678" Delegate:self];
     
     @endcode
     
     @param _keys [NSArray*] The subset of keys to request from the server.  If 'nil' all key->value pairs will be returned.
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the data is being fetched.
     @param _delegate [id] The \<FuseGameDataDelegate\> to which the callback will be made when data has returned to the device.
     @retval [int] This is the request ID.
     @see getGameData:Delegate: to understand how to use the game data aspects of the method, and how to register the \<FuseGameDataDelegate\> delegate.
     @since Fuse API version 1.15
     *///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual int getFriendGameData(const std::vector<std::string> &keys, const std::string &fuse_id, FuseGameDataDelegate *delegate) = 0;

    /*!
     @brief This method retrieves persistent data for a friend of the current user from the Fuse server while specifying a master key value for the key->value pairs in the dictionary.
     @details Similar to getFriendGameData:FuseID:Delegate:, this method allows for the specification of a master key.
     
     @code
     
     // Example with Key == nil to get all information for the "car_info" master key
     [FuseAPI getGameData:nil Key:@"car_info" FuseID:@"012345678" Delegate:self];
     
     @endcode
     
     @param _keys [NSArray*] The subset of keys to request from the server.  If 'nil' all key->value pairs will be returned.
     @param _key [NSString*] This is the master key name.
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the data is being fetched.
     @param _delegate [id] The \<FuseGameDataDelegate\> to which the callback will be made when data has returned to the device.
     @retval [int] This is the request ID.
     @see getGameData:Delegate: to understand how to use the game data aspects of the method, and how to register the \<FuseGameDataDelegate\> delegate.
     @see
     @since Fuse API version 1.15
     */
    virtual int getFriendGameData(const std::vector<std::string> &keys, const std::string &key, const std::string &fuse_id, FuseGameDataDelegate *delegate) = 0;

    #pragma mark Friends List Actions
    /*!
     * @brief Get a the user's friends list
     * @details Once a user has signed in with one of the supported account services (for instance Game Center using gameCenterLogin:), they will have a a friends list which is retrievable from the server.  This list is composed of any friends that they have invited.  This process is asynchronous in that it is required to go the the Fuse servers to retrieve the list.  Therefore, the result is returned in a callback (FuseDelegate::friendsListUpdated:) to the \<FuseDelegate\> delegate registered in startSession:Delegate:.
     
     To request the list:
     
     @code
     
     [FuseAPI updateFriendsListFromServer];
     
     @endcode
     
     The result is returned to the \<FuseDelegate\> callback:
     
     @code
     
     -(void) friendsListUpdated:(NSDictionary*)_friendsList
     {
     // The friends list has returned to the device
     
     // sample code to parse the dictionary
     NSArray *fuse_ids = [_friendsList allKeys];
     
     for (int i = 0; i < [fuse_ids count]; i++)
     {
     NSString *fuse_id = [fuse_ids objectAtIndex:i];
     
     NSDictionary *friendEntry = (NSDictionary*)[_friendsList objectForKey:fuse_id];
     
     NSLog(@"Friend is Pending: %d", [[friendEntry objectForKey:@"pending"] intValue]);
     NSLog(@"Friend Alias: %@", [friendEntry objectForKey:@"alias"]);
     NSLog(@"Friend Fuse ID: %@", [friendEntry objectForKey:@"fuse_id"]);
     }
     }
     
     -(void) friendsListError:(NSNumber*)_error
     {
     // An error has occurred in getting the friends list
     // See kFuseFriendsListErrors for more information on all of the possible error codes
     }
     
     @endcode
     
     @see getFriendsList for more information on getting the local version of the friends list
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see FuseDelegate::friendsListError: for more information of the delegate method
     @since Fuse API version 1.22
     */
    virtual void updateFriendsListFromServer() = 0;

    /*!
     * @brief This method returns the local friends list of the logged in user
     * @details Similar to updateFriendsListFromServer, this method merely returns the local copy of the friends list.  The local version of the list can differ from the server version in two ways.  Firstly, friends could accept an invite in another device, clearing both users to show up in each other's list.  This server updated is not signaled to the client devices.  Secondly, given that there are propagation delays and HTTP request ordering issues, any "action" (adding a friend, accepting or deleting) will take a few seconds to reach the server. A request to get the server version very close to one of these actions could result in the list not being representative of the final list.
     
     To get the local friends list:
     
     @code
     
     NSMutableDictionary *local_friends_list = [FuseAPI getFriendsList];
     
     @endcode
     
     @retval [NSMutableDictionary*] The local version of the user's friends list
     @see updateFriendsListFromServer for more information on retrieving the list from the Fuse servers and the format of the friends list dictionary
     @since Fuse API version 1.22
     */
    virtual const std::map<std::string, FuseboxxFriendEntry> &getFriendsList() = 0;

    /*!
     * @brief This method is used to invite (add) a friend to the logged in user's friends list
     * @details A friend is not added right away to the inviting user's list.  Instead, there is a handshaking mechanism whereby the invited user needs to agree to the invite (see acceptFriend:) before both users are shown in each others list.  If a \<FuseDelegate\> has been registered using startSession:Delegate:, a callback can be made once the friend is added (in case a notification is required by the application).
     
     To add a friend:
     
     @code
     
     [FuseAPI addFriend:@"012345678"];
     
     @endcode
     
     The (optional) callback to the \<FuseDelegate\> is as follows:
     
     @code
     
     -(void) friendAdded:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     // A friend has been added
     // If [error intValue] != 0, an error has occurred
     // Please see kFuseAddFriendErrors for more information on all of the possible error codes
     }
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player being invited
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see FuseDelegate::friendAdded:Error: for more information on the delegate method
     @since Fuse API version 1.22
     */
    virtual void addFriend(const std::string &fuse_id) = 0;

    /*!
     * @brief This method is used to delete a friend from the logged in user's friends list
     * @details Once a friend is removed by a user, both the target and source user will not show in each other's friends list.  If a \<FuseDelegate\> has been registered using startSession:Delegate:, a callback can be made once the friend is removed (in case a notification is required by the application).
     
     To remove a friend:
     
     @code
     
     [FuseAPI removeFriend:@"012345678"];
     
     @endcode
     
     The (optional) callback to the \<FuseDelegate\> is as follows:
     
     @code
     
     -(void) friendRemoved:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     // A friend has been removed
     // If [error intValue] != 0, an error has occurred
     // Please see kFuseRemoveFriendErrors for more information on all of the possible error codes
     }
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player being deleted
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see FuseDelegate::friendRemoved:Error: for more information on the delegate method
     @since Fuse API version 1.22
     */
    virtual void removeFriend(const std::string &fuse_id) = 0;

    /*!
     * @brief This method is used to accept a friend request
     * @details The inviting of a friend is a two-step process.  The first step is to actually invite the user (source user) using addFriend:, and the second step is the acceptance by the target user using this method.  If a \<FuseDelegate\> has been registered using startSession:Delegate:, a callback can be made once the friend is accepted (in case a notification is required by the application).
     
     To accept a user:
     
     @code
     
     [FuseAPI acceptFriend:@"012345678"];
     
     @endcode
     
     The (optional) callback to the \<FuseDelegate\> is as follows:
     
     @code
     
     -(void) friendAccepted:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     // A friend has been marked as accepted on the server
     // If [error intValue] != 0, an error has occurred
     // Please see kFuseAcceptFriendErrors for more information on all of the possible error codes
     }
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player being accepted
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see addFriend: for more information on adding a friend and the handshaking process
     @see FuseDelegate::friendAccepted:Error: for more information on the delegate method
     @since Fuse API version 1.22
     */
    virtual void acceptFriend(const std::string &fuse_id) = 0;

    /*!
     * @brief This method is used to reject a friend request.
     * @details In the second half of the inviting a friend process, the user can optionally accept or reject a friend request.  If this user decides that they do not want the source user as a friend, call this method to remove it from the list of pending requests.  If a \<FuseDelegate\> has been registered using startSession:Delegate:, a callback can be made once the friend is accepted (in case a notification is required by the application).  A friend is in the second phase of the process if it is indicated as 'pending' in the updateFriendsListFromServer callback friendsListUpdated:.
     
     To reject a friend:
     
     @code
     
     [FuseAPI rejectFriend:@"012345678"];
     
     @endcode
     
     The (optional) callback to the \<FuseDelegate\> is as follows:
     
     @code
     
     -(void) friendRejected:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     // A friend has been marked as rejected on the server
     // If [error intValue] != 0, an error has occurred
     // Please see kFuseRejectFriendErrors for more information on all of the possible error codes
     }
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player being rejected
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see addFriend: for more information on adding a friend and the handshaking process
     @see FuseDelegate::friendRejected:Error: for more information on the delegate method
     @since Fuse API version 1.22
     */
    virtual void rejectFriend(const std::string &fuse_id) = 0;

    /*!
     * @brief This method is used to migrate (one-way) friends from an existing account to the logged in user's friends list
     * @details In the case where a user is transferring between two different account types, this method allows the users's friends to be added to the target account. If a \<FuseDelegate\> has been registered using startSession:Delegate:, a callback can be made once the friends are migrated (in case a notification is required by the application). Please use this function with caution as it is one-way.  Friends from the previos account will be permanently moved and will not remain in the previous account.
     
     To migrate friends:
     
     @code
     
     [FuseAPI migrateFriends:@"012345678"];
     
     @endcode
     
     The (optional) callback to the \<FuseDelegate\> is as follows:
     
     @code
     
     -(void) friendsMigrated:(NSString*)_fuse_id Error:(NSNumber*)_error
     {
     // A friend has been added
     // If [error intValue] != 0, an error has occurred
     // Please see kFuseMigrateFriendErrors for more information on all of the possible error codes
     }
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player whose friends are being migrated to the logged in account
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see FuseDelegate::friendsMigrated:Error: for more information on the delegate method
     @since Fuse API version 1.34
     */
    virtual void migrateFriends(const std::string &fuse_id) = 0;

    #pragma mark Chat List Actions

    /*!
     * @brief Post a chat message to another user
     * @details Using this system, users can communicate with one another inside the game.  Messages are posted to the Fuse system and requested by game clients when needed.  The user must have logged in to one of the supported account services (for instance Game Center using gameCenterLogin:) in order to have an assigned Fuse ID.  A message can be a maximum of 256 characters.  This system would most likely be used in conjunction with another social tool, such as the friend's list (see getFriendsList), where a list of users and their associated Fuse IDs would be known. A friend is in the second phase of the process if it is indicated as 'pending' in the updateFriendsListFromServer callback friendsListUpdated:.
     
     To post a message:
     
     @code
     
     [FuseAPI postUserChatMessage:@"Hey - I am chatting to you right now" TargetUser:@"012345678"];
     
     @endcode
     
     
     @param _message [NSString*] The message to be sent to the user (max 256 characters)
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player to which the message will be sent
     @since Fuse API version 1.22
     */
    virtual void postUserChatMessage(const std::string &message, const std::string &fuse_id) = 0;

    /*!
     * @brief Post a chat message to another user indicating the user's level
     * @details Similar to postUserChatMessage:TargetUser:, this method also adds the ability to send the user's current level along with the data.  This would only be done if it is desired to show a user level along with the message.
     
     To post a message:
     
     @code
     
     [FuseAPI postUserChatMessage:@"Hey - I am chatting to you right now" TargetUser:@"012345678" Level:5];
     
     @endcode
     
     @param _message [NSString*] The message to be sent to the user (max 256 characters)
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player to which the message will be sent
     @param _level [int] The level of the user in the game
     @see postUserChatMessage:TargetUser: for more information on posting a message
     @since Fuse API version 1.22
     */
    virtual void postUserChatMessage(const std::string &message, const std::string &fuse_id, int level) = 0;

    /*!
     * @brief Get the authoritative chat list from the server.
     * @details This method ignores the current local chat list and returns the server-side version of the chat list.  When the list arrives to the client, the old local chat list will be overwritten.  This method is asynchronous in that a list will not be returned to the user until it is received by the client from the server, and at that point in the form of a callback to the \<FuseDelegate\>.
     
     To perform the request:
     
     @code
     
     // To get the chat list for the signed in user (local)
     [FuseAPI getUserChatListFromServer:[FuseAPI getFuseID]];
     
     // To get the chat list for another user
     [FuseAPI getUserChatListFromServer:@"0123456789"];
     
     @endcode
     
     This method will return the data in the form of a NSDictionary* to the chatListReceived:User: method on the registered \<FuseDelegate\>:
     
     @code
     
     -(void) chatListReceived:(NSDictionary*)_messages User:(NSString*)_fuse_id
     {
     // Sample code for parsing the dictionary returned
     if (_messages != nil && [_messages count] > 0)
     {
     NSArray *keys = [_messages allKeys];
     NSArray *sortedKeys = [keys sortedArrayUsingSelector:@selector(compare:)];
     
     for (int i = 0; i < [sortedKeys count]; i++)
     {
     NSString *message_id = [sortedKeys objectAtIndex:i];
     NSMutableDictionary *entry = (NSMutableDictionary*)[_messages objectForKey:message_id];
     
     NSLog(@"%d [%@]: %@ -> %@", [message_id intValue], [entry objectForKey:@"timestamp"], [entry objectForKey:@"alias"], [entry objectForKey:@"message"]);
     }
     }
     }
     
     -(void) chatListError:(NSNumber*)_error
     {
     // An error has occurred
     // Refer to kFuseChatErrors for information of the error cases possible
     }
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the chat list is desired
     @see getFuseID for more information on getting the local users Fuse ID
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see getUserChatList: for information on requesting the local copy of the chat list
     @see chatListError: for more information on the delegate method
     @since Fuse API version 1.22
     */
    virtual void getUserChatListFromServer(const std::string &fuse_id) = 0;

    /*!
     * @brief Get the local chat list stored in the client
     * @details Similar to getUserChatListFromServer:, this method directly returns the local version of the chat list.  The difference from the local and client version is twofold.  The first difference is that the client does not receive asynchronous update events to update it's list (i.e. if another user has messaged and that message has been recorded on the server, the server does not message the client about the update).  The second difference is that if a user posts to their chat list, the change takes a few seconds to be reflected on the server.  In this case, if a read is initiated directly after posting a message, due to the non-guaranteed order of HTTP requests, the returned list might exclude the recently posted message.  If getUserChatListFromServer: has not been called, there will be no local copy of the list.
     
     To request the local list:
     
     @code
     
     // To request the chat list for the signed in user (local)
     NSMutableDictionary* chat_list = [FuseAPI getUserChatList:[FuseAPI getFuseID]];
     
     // To request another user's chat list
     NSMutableDictionary* chat_list = [FuseAPI getUserChatList:@"0123456768"];
     
     @endcode
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the chat list is desired
     @retval [NSMutableDictionary*] The local copy of the chat list for the desired player
     @see getUserChatListFromServer: for information on requesting the local message list
     @see getFuseID for more information on getting the local users Fuse ID
     @since Fuse API version 1.22
     */
    virtual const std::map<std::string, FuseboxxChatListEntry>& getUserChatList(const std::string &fuse_id) = 0;

    /*!
     * @brief Delete a chat message.
     * @details This method will delete a chat message for a specified user.  The "message ID" must be specified (obtained when fetching a message list - see getUserChatListFromServer:).
     
     To delete a message:
     
     @code
     
     [FuseAPI deleteUserChatMessage:2 TargetUser:@"012345678"];
     
     @endcode
     
     @param _message_id [int] The ID of the message to be deleted
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the message will be deleted
     @see getUserChatListFromServer: for information of fetching a request list from the server
     @since Fuse API version 1.22
     */
    virtual void deleteUserChatMessage(int message_id, const std::string &fuse_id) = 0;

    #pragma mark User-to-User Push Notifications
    /*!
     * @brief Send an Apple push notification to another user
     * @details Use this method to send a push notification to another user, using their Fuse ID to each of their devices that have logged in with that ID (and that have registered to receive notifications).  This system is dependent upon both the sender and the recipient to have logged in with either a Game Center account (see gameCenterLogin:) or similar method.  This system would most likely be used in conjunction with another social tool, such as the friend's list (see getFriendsList), where a list of users and their associated Fuse IDs would be known.  Messages can be no longer than 256 characters in length.
     
     To send a push notification:
     
     @code
     
     [FuseAPI userPushNotification:@"012345678" Message:@"Test Push Message"];
     
     @endcode
     
     Please ensure that your game is registered to collect device tokens and receive push notifications to be compatible with this feature.  The can be up to a five minute delay or longer before a message is received by a user.
     
     @param _fuse_id [NSString*] This is the "Fuse ID" of the player to which the message will be sent
     @param _message [NSString*] The message to be sent to the user (max 256 characters)
     @see gameCenterLogin: for more information on how to register a login with a Game Center ID
     @see facebookLogin: for more information on how to register a login with a Facebook account ID
     @see twitterLogin: for more information on how to register a login with a Twitter account ID
     @see openFeintLogin: for more information on how to register a login with an OpenFeint account ID
     @see fuseLogin:Alias: for more information on how to register a login with a Fuse ID
     @since Fuse API version 1.22
     */
    virtual void userPushNotification(const std::string &fuse_id, const std::string &message) = 0;

    /*!
     * @brief Send an Apple push notification to a user's entire friends list
     * @details Similar to userPushNotification:Message:, this method sends the same message to each user in the source user's friends list.
     
     To send this type of push notification:
     
     @code
     
     [FuseAPI friendsPushNotification:@"Test Push Message Friends List"];
     
     @endcode
     
     @param _message [NSString*] The message to be sent to the user (max 256 characters)
     @see userPushNotification:Message: for more information on sending a push notification
     @since Fuse API version 1.22
     */
    virtual void friendsPushNotification(const std::string &message) = 0;

    #pragma mark Mail/Gifting

    /*!
     @brief Get the the list of messages (and gifts) sent to the currently signed in user.
     @details This method receives the server list of gifts granted to the user.  This method is asynchronous in that a list will not be returned to the user until it is received by the client from the server, and at that point in the form of the callback FuseDelegate::giftListRecieved: to the \<FuseDelegate\>. This method assumes the list being fetched is for a user already signed in using one of the available account services (Game Center, Facebook) in the Fuse API
     
     A sample of the callback would be as follows:
     
     @code
     
     -(void) mailListRecieved:(NSDictionary*)_messages User:(NSString*)_fuse_id
     {
     // Sample code for parsing the dictionary returned
     if (_messages != nil && [_messages count] > 0)
     {
     NSArray *keys = [_messages allKeys];
     
     for (int i = 0; i < [keys count]; i++)
     {
     NSString *message_id = [keys objectAtIndex:i];
     NSMutableDictionary *entry = (NSMutableDictionary*)[_messages objectForKey:message_id];
     
     // Note: message_id is not the identifier specifying the type of gift - it is a unique index given to the gift transaction in the DB
     
     NSLog(@"%d [%@]: %@ -> %@ %@ %d %@", [message_id intValue], [entry objectForKey:@"timestamp"], [entry objectForKey:@"alias"], [entry objectForKey:@"from_user"], [entry objectForKey:@"gift_name"], [[entry objectForKey:@"gift_amount"] intValue], [entry objectForKey:@"gift_url"]);
     }
     }
     }
     
     -(void) mailListError:(NSNumber*)_error
     {
     // An error has occurred
     // Refer to kFuseMailListErrors for information of the error cases possible
     }
     
     @endcode
     
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     @see gameCenterLogin: for more information on how to register a login with a Game Center ID
     @see facebookLogin: for more information on how to register a login with a Facebook account ID
     @see twitterLogin: for more information on how to register a login with a Twitter account ID
     @see openFeintLogin: for more information on how to register a login with an OpenFeint account ID
     @see fuseLogin:Alias: for more information on how to register a login with a Fuse ID
     @since Fuse API version 1.25
     */
    virtual void getMailListFromServer() = 0;

    /*!
     * @brief Get the list of messages (and gifts) sent to another user.
     * @details Similar to getMailListFromServer, this method will return the message list for another user (not the user currently signed in).
     *
     * @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the list is requested
     * @see getMailListFromServer for more information on how to handle the callback for this method
     * @since Fuse API version 1.25
     */
    virtual void getMailListFriendFromServer(const std::string &fuse_id) = 0;

    /*!
     * @brief Get the mail list of a user that has already been retrieved from the server
     * @details This list is the local copy, of any user that has already been fetched
     * @see getMailListFromServer and getMailListFriendFromServer: for more information on retrieving the mail/gift list from the server
     * @since Fuse API version 1.25
     */
    virtual const std::map<std::string, FuseboxxMailListEntry>& getMailList(const std::string &_fuse_id) = 0;

    /*!
     * @brief Mark a particular message (or gift) as receieved by the user
     * @details In order to facilitate the handshaking required, this method will mark a particular gift as "used". After this has been called, this message/gift will no longer appear in the list returned by the server. The method should only be called once it has been guaranteed by the client that the item has been credited to the user.
     
     This method can only be called for the user currently logged in (therefore the input "Fuse ID" is not able to be specified - assumed to be the same as getFuseID.
     
     * @param _message_id [int] The gift ID that has been consumed by the client
     * @see getFuseID for more information on retrieving the user's Fuse ID once signed in
     * @since Fuse API version 1.25
     */
    virtual void setMailAsReceived(int message_id) = 0;

    /*!
     * @brief Send a message to a user with a gift attached
     * @details This method facilitates gifting another user. A message can be specified along with the unique gift identifier as well as amount.  This method will return a callback to the \<FuseDelegate\> to indicate if the gifting process was successful or whether an error occurred.
     
     An example of the callback is as follows:
     
     @code
     
     -(void) mailAcknowledged:(NSNumber*)_message_id User:(NSString*)_fuse_id RequestID:(NSNumber*)_request_id
     {
     // The message was received successfully
     }
     
     -(void) mailError:(NSNumber*)_error RequestID:(NSNumber*)_request_id
     {
     // An error has occurred
     // Refer to kFuseMailErrors for information of the error cases possible
     }
     
     @endcode
     
     * @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the gift is destined
     * @param _message [NSString*] The message to be sent along with the gift (optional)
     * @param _gift_id [int] The unique gift index that identifies the gift uniquely to the game (should this be an INT?)
     * @param _amount [int] The quantity of the gift to be awarded to the user (associated with _gift_id)
     * @see FuseDelegate::giftAcknowledged for more information on the send gift callback
     * @see FuseDelegate::giftListError for more information on the error handling for this method
     * @since Fuse API version 1.25
     */
    virtual int sendMailWithGift(const std::string &fuse_id, const std::string &message, int gift_id, int amount) = 0;


    /*!
     * @brief Send a message to a user
     * @details This method facilitates messaging another user. This method will return a callback to the \<FuseDelegate\> to indicate if the messaging process was successful or whether an error occurred.
     
     An example of the callback is as follows:
     
     @code
     
     -(void) mailAcknowledged:(NSNumber*)_message_id User:(NSString*)_fuse_id RequestID:(NSNumber*)_request_id
     {
     // The message was received successfully
     }
     
     -(void) mailError:(NSNumber*)_error RequestID:(NSNumber*)_request_id
     {
     // An error has occurred
     // Refer to kFuseMailErrors for information of the error cases possible
     }
     
     @endcode
     
     * @param _fuse_id [NSString*] This is the "Fuse ID" of the player for which the gift is destined
     * @param _message [NSString*] The message to be sent along with the gift (optional)
     * @see FuseDelegate::mailAcknowledged:User:RequestID: for more information on the send gift callback
     * @see FuseDelegate::mailListError for more information on the error handling for this method
     * @since Fuse API version 1.25
     */
    virtual int sendMail(const std::string &fuse_id, const std::string &message) = 0;

    #pragma mark Specific Event Registration

    /*!
     * @brief Register the user's current level after they level-up
     * @details This method can specifically track user levels to more accurately measure application penetration
     *
     * @code
     
     [FuseAPI registerLevel:5];
     
     * @endcode
     *
     * @param _level [int] The player's new level
     * @since Fuse API version 1.25
     */
    virtual void registerLevel(int level) = 0;

    /*!
     * @brief Register a change in the current balances of the user's in-app currencies.
     * @details To better track the currency levels of your users, this method can be used to keep the system up-to-date as to the levels of currencies across your users.
     *
     * @code
     
     [FuseAPI registerCurrency:2 Balance:115];
     
     * @endcode
     *
     * @param _currencyType [int] Enter 1-4, representing up to four different in-app resources.  These values can be set specific to the application.
     * @param _balance [int] The updated balance of the user
     * @since Fuse API version 1.25
     */
    virtual void registerCurrency(int currencyType, int balance) = 0;

    /*!
     * @brief Register a view of a Flurry video
     * @details Track each time a user views a Flurry video.
     *
     * @code
     
     [FuseAPI registerFlurryView];
     
     * @endcode
     *
     * @since Fuse API version 1.25
     */
    virtual void registerFlurryView() = 0;

    /*!
     * @brief Register a click on a Flurry video
     * @details Track each time a user clicks a Flurry video.
     *
     * @code
     
     [FuseAPI registerFlurryClick];
     
     * @endcode
     *
     * @since Fuse API version 1.25
     */
    virtual void registerFlurryClick() = 0;

    /*!
     * @brief Register the receipt of a tapjoy reward to the user
     * @details Track each time a user is rewarded through an incentivized action using Flurry.
     *
     * @code
     
     [FuseAPI registerFlurryClick];
     
     * @endcode
     *
     * @param _amtCurrency [int] The total amount of the in-game currency that the user has been awarded by Tapjoy
     * @since Fuse API version 1.25
     */
    virtual void registerTapjoyReward(int amtCurrency) = 0;


    #pragma mark Game Configuration Data

    /*!
     @brief This method retrieves server configuration values.
     @details The Fuse API provides a method to store game configuration variables that are provided to the application on start.  These are different than "Game Data" values since they are stored on a per-game basis, and not a per-user basis.
     
     In the Fuse dashboard, navigate to the 'configuration' tab in your game view.  You can edit the "Game Data" section by adding keys and associated data values.  Values can be 256 characters in length and support UTF-8 characters.
     
     @code
     
     NSString *my_val = [FuseAPI getGameConfigurationValue:@"my_key"];
     
     if (my_val != nil)
     {
     // always check against 'nil' before using the value
     }
     
     @endcode
     
     Values are update in the client each time a session is started from the Springboard or system tray. To find out when values are valid in the device, you can use the following \<FuseDelegate\> callback method that indicates when the values are ready to be inspected.
     
     @code
     
     BOOL has_game_config_returned = NO;
     
     -(void) gameConfigurationReceived
     {
     has_game_config_returned = YES;
     
     // You can now access your updated server-side data, either here or somewhere else in your code
     NSString *funny_val = [FuseAPI getGameConfigurationValue:@"not_funny"];
     }
     
     @endcode
     
     It is recommended that a default value be present on the device in case the user has not or never connects to the Internet.
     
     @param _key [NSString*] This is the key for which the value is requested.
     @retval [NSString*] This is the value for the corresponding key.
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     */
    virtual const std::string* getGameConfigurationValue(const std::string &key) = 0;

    /*!
     @brief This method retrieves the entire server configuration value list.
     @details The Fuse API provides a method to store game configuration variables that are provided to the application on start.  These are different than "Game Data" values since they are stored on a per-game basis, and not a per-user basis.
     
     In the Fuse dashboard, navigate to the 'configuration' tab in your game view.  You can edit the "Game Data" section by adding keys and associated data values.  Values can be 256 characters in length and support UTF-8 characters.
     
     @code
     
     NSMutableDictionary *my_vals = [FuseAPI getGameConfiguration];
     
     if (my_vals != nil)
     {
     // always check against 'nil' before using the value
     }
     
     @endcode
     
     Values are update in the client each time a session is started from the Springboard or system tray. To find out when values are valid in the device, you can use the following \<FuseDelegate\> callback method that indicates when the values are ready to be inspected.
     
     @code
     
     BOOL has_game_config_returned = NO;
     
     -(void) gameConfigurationReceived
     {
     has_game_config_returned = YES;
     
     // You can now access your updated server-side data, either here or somewhere else in your code
     NSMutableDictionary *my_vals = [FuseAPI getGameConfiguration];
     
     if (my_vals != nil && [my_vals count] > 0)
     {
     NSArray *keys = [my_vals allKeys];
     
     for (int i = 0; i < [keys count]; i++)
     {
     NSString *key = [keys objectAtIndex:i];
     NSString *value = [my_vals objectForKey:key];
     
     NSLog(@"Key: %@, Value: %@", key, value);
     }
     }
     }
     
     @endcode
     
     It is recommended that a default value be present on the device in case the user has not or never connects to the Internet.
     
     @retval [NSMutableDictionary *] The game configuration exists in the returned NSMutableDictionary
     @see startSession:Delegate: for information on setting up the \<FuseDelegate\>
     */

    virtual const std::map<std::string,std::string>& getGameConfiguration() = 0;

    #pragma mark Public Methods
    /*!
     * @brief Singleton accessor for the FuseAPI class
     * @retval [FuseAPI*] The Fuse API singleton instance
     */
    static FuseAPI& getInstance();
};


}

#endif /* __FACEBOOKADS_H__ */
