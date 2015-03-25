/****************************************************************************
 Copyright (c) 2014      Jesús Sánchez
 
 http://jesanchezgo.hostwp.es
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __AVALON_CHARTBOOST_H__
#define __AVALON_CHARTBOOST_H__

namespace avalon {

/*
 Defines standard locations to describe where Chartboost SDK features appear in game.
 For best performance, it is highly recommended to use standard locations.
 
 Benefits include:
 - Higher eCPMs.
 - Control of ad targeting and frequency.
 - Better reporting.
 */
typedef const char* C2DXCBLocation;

/*! "Startup" - Initial startup of game. */
extern C2DXCBLocation const C2DXCBLocationStartup;
/*! "Home Screen" - Home screen the player first sees. */
extern C2DXCBLocation const C2DXCBLocationHomeScreen;
/*! "Main Menu" - Menu that provides game options. */
extern C2DXCBLocation const C2DXCBLocationMainMenu;
/*! "Game Screen" - Game screen where all the magic happens. */
extern C2DXCBLocation const C2DXCBLocationGameScreen;
/*! "Achievements" - Screen with list of achievements in the game. */
extern C2DXCBLocation const C2DXCBLocationAchievements;
/*! "Quests" - Quest, missions or goals screen describing things for a player to do. */
extern C2DXCBLocation const C2DXCBLocationQuests;
/*!  "Pause" - Pause screen. */
extern C2DXCBLocation const C2DXCBLocationPause;
/*! "Level Start" - Start of the level. */
extern C2DXCBLocation const C2DXCBLocationLevelStart;
/*! "Level Complete" - Completion of the level */
extern C2DXCBLocation const C2DXCBLocationLevelComplete;
/*! "Turn Complete" - Finishing a turn in a game. */
extern C2DXCBLocation const C2DXCBLocationTurnComplete;
/*! "IAP Store" - The store where the player pays real money for currency or items. */
extern C2DXCBLocation const C2DXCBLocationIAPStore;
/*! "Item Store" - The store where a player buys virtual goods. */
extern C2DXCBLocation const C2DXCBLocationItemStore;
/*! "Game Over" - The game over screen after a player is finished playing. */
extern C2DXCBLocation const C2DXCBLocationGameOver;
/*! "Leaderboard" - List of leaders in the game. */
extern C2DXCBLocation const C2DXCBLocationLeaderBoard;
/*! "Settings" - Screen where player can change settings such as sound. */
extern C2DXCBLocation const C2DXCBLocationSettings;
/*! "Quit" - Screen displayed right before the player exits a game. */
extern C2DXCBLocation const C2DXCBLocationQuit;
/*! "Default" - Supports legacy applications that only have one "Default" location */
extern C2DXCBLocation const C2DXCBLocationDefault;


/*
 Returned to ChartboostDelegate methods to notify of Chartboost SDK errors.
 */
typedef const int C2DXCBLoadError;

/*! Unknown internal error. */
extern C2DXCBLoadError const C2DXCBLoadErrorInternal;
/*! Network is currently unavailable. */
extern C2DXCBLoadError const C2DXCBLoadErrorInternetUnavailable;
/*! Too many requests are pending for that location.  */
extern C2DXCBLoadError const C2DXCBLoadErrorTooManyConnections;
/*! Interstitial loaded with wrong orientation. */
extern C2DXCBLoadError const C2DXCBLoadErrorWrongOrientation;
/*! Interstitial disabled, first session. */
extern C2DXCBLoadError const C2DXCBLoadErrorFirstSessionInterstitialsDisabled;
/*! Network request failed. */
extern C2DXCBLoadError const C2DXCBLoadErrorNetworkFailure;
/*!  No ad received. */
extern C2DXCBLoadError const C2DXCBLoadErrorNoAdFound;
/*! Session not started. */
extern C2DXCBLoadError const C2DXCBLoadErrorSessionNotStarted;
/*! User manually cancelled the impression. */
extern C2DXCBLoadError const C2DXCBLoadErrorUserCancellation;
/*! No location detected. */
extern C2DXCBLoadError const C2DXCBLoadErrorNoLocationFound;
#if defined(ANDROID)
/*! Impression already visible. */
extern C2DXCBLoadError const C2DXCBLoadErrorImpressionAlreaadyVisible;
/*! No Host Activity. */
extern C2DXCBLoadError const C2DXCBLoadErrorNoHostActivity;
#endif

/*
 Returned to ChartboostDelegate methods to notify of Chartboost SDK errors.
*/
typedef const int C2DXCBClickError;

/*! Invalid URI. */
extern C2DXCBClickError const C2DXCBClickErrorUriInvalid;
/*! The device does not know how to open the protocol of the URI  */
extern C2DXCBClickError const C2DXCBClickErrorUriUnrecognized;
/*! User failed to pass the age gate. */
extern C2DXCBClickError const C2DXCBClickErrorAgeGateFailure;
/*! Unknown internal error */
extern C2DXCBClickError const C2DXCBClickErrorInternal;
#if defined(ANDROID)
/*! No Host Activity */
extern C2DXCBClickError const C2DXCBClickErrorNoHostActivity;
#endif

class ChartboostDelegate {
public:
    virtual ~ChartboostDelegate() {}
    
    /*********************************
     * Interstitial
     *********************************/
    
    // Called before requesting an interestitial from the back-end
    virtual bool shouldRequestInterstitial(C2DXCBLocation location) { return true; }
    
    // Called when an interstitial has been received, before it is presented on screen
    virtual bool shouldDisplayInterstitial(C2DXCBLocation location) { return true; }
    
    // Called after an interstitial has been displayed on the screen.
    virtual void didDisplayInterstitial(C2DXCBLocation location) {}
    
    // Called when an interstitial has been received and cached.
    virtual void didCacheInterstitial(C2DXCBLocation location) {}
    
    // Called when an interstitial has failed to come back from the server
    virtual void didFailToLoadInterstitial(C2DXCBLocation location, C2DXCBLoadError error) {}
    
    // Called after a click is registered, but the user is not fowrwarded to the IOS App Store.
    virtual void didFailToRecordClick(C2DXCBLocation location, C2DXCBClickError error) {}
    
    // Called when the user dismisses the interstitial
    virtual void didDismissInterstitial(C2DXCBLocation location) {}
    
    // Same as above, but only called when dismissed for a close
    virtual void didCloseInterstitial(C2DXCBLocation location) {}
    
    // Same as above, but only called when dismissed for a click
    virtual void didClickInterstitial(C2DXCBLocation location) {}
    
    // Whether Chartboost should show ads in the first session
    // Defaults to true
    virtual bool setShouldRequestInterstitialsInFirstSession() { return true; }
 
    /*********************************
     * Rewarded Videos
     *********************************/
    
    // Called before a rewarded video will be displayed on the screen.
    virtual bool shouldDisplayRewardedVideo(C2DXCBLocation location) { return true; }
    
    // Called after a rewarded video has been displayed on the screen.
    virtual void didDisplayRewardedVideo(C2DXCBLocation location) {}
    
    // Called after a rewarded video has been loaded from the Chartboost API
    // servers and cached locally.
    virtual void didCacheRewardedVideo(C2DXCBLocation location) {}
    
    // Called after a rewarded video has attempted to load from the Chartboost API
    // servers but failed.
    virtual void didFailToLoadRewardedVideo(C2DXCBLocation location, C2DXCBLoadError error) {}
    
    // Called after a rewarded video has been dismissed.
    virtual void didDismissRewardedVideo(C2DXCBLocation location) {}
    
    // Called after a rewarded video has been closed.
    virtual void didCloseRewardedVideo(C2DXCBLocation location) {}
    
    // Called after a rewarded video has been clicked.
    virtual void didClickRewardedVideo(C2DXCBLocation location) {}
    
    // Called after a rewarded video has been viewed completely and user is eligible for reward.
    virtual void didCompleteRewardedVideo(C2DXCBLocation location, int reward) {}
};

class Chartboost {
    
public:
    
    static Chartboost* getInstance();
    static void purgeChartboost();
    
    // Start session and set appId, appSignature
    void startWithAppId(const char * appId, const char * appSignature);

    /*********************************
     * Interstitial
     *********************************/

    // Cache an interstitial, optionally takes a location argument
    void cacheInterstitial(C2DXCBLocation location = C2DXCBLocationDefault);
    // Show an interstitial, optionally takes a location argument
    void showInterstitial(C2DXCBLocation location = C2DXCBLocationDefault);
    // Determine if a locally cached interstitial exists for the given CBLocation.
    bool hasInterstitial(C2DXCBLocation location = C2DXCBLocationDefault);
    // Decide if Chartboost SDK should show interstitials in the first session.
    // true if allowed to show interstitials in first session, false otherwise.
    // Default is true.
    void setShouldRequestInterstitialsInFirstSession(bool shouldRequest);
    
    /*********************************
     * Rewarded Videos
     *********************************/
    
    // Cache an rewarded video, optionally takes a location argument
    void cacheRewardedVideo(C2DXCBLocation location = C2DXCBLocationDefault);
    // Show a rewarded video, optionally takes a location argument
    void showRewardedVideo(C2DXCBLocation location = C2DXCBLocationDefault);
    // Determine if a locally cached revarded video exists for the given CBLocation.
    bool hasRewardedVideo(C2DXCBLocation location = C2DXCBLocationDefault);
    
    /*********************************
     * Delegate
     *********************************/

    void setDelegate(ChartboostDelegate* delegate) { m_delegate = delegate; }
    ChartboostDelegate* getDelegate() { return m_delegate; }
    
private:
    Chartboost()
    : m_delegate(nullptr)
    {
    }
    
    ChartboostDelegate* m_delegate;
};
    
}

#endif // End __AVALON_CHARTBOOST_H__
