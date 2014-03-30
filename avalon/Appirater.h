#ifdef AVALON_CONFIG_APPIRATER_ENABLED

#ifndef AVALON_APPIRATER_H
#define AVALON_APPIRATER_H

namespace avalon {

class Appirater;
    
class AppiraterDelegate
{
    virtual void appiraterDidDisplayAlert(Appirater *appirater) {};
    virtual void appiraterDidDeclineToRate(Appirater *appirater) {};
    virtual void appiraterDidOptToRate(Appirater *appirater) {};
    virtual void appiraterDidOptToRemindLater(Appirater *appirater) {};
    virtual void appiraterWillPresentModalView(Appirater *appirater, bool animated) {};
    virtual void appiraterDidDismissModalView(Appirater *appirater, bool animated) {};
};

class Appirater
{
public:
    static Appirater* getInstance();

    /*
     Set your Apple generated software id here.
     */
    void setAppId(const char *appName);

    /*
     Tells Appirater that the app has launched, and on devices that do NOT
     support multitasking, the 'uses' count will be incremented. You should
     call this method at the end of your application delegate's
     application:didFinishLaunchingWithOptions: method.
     
     If the app has been used enough to be rated (and enough significant events),
     you can suppress the rating alert
     by passing NO for canPromptForRating. The rating alert will simply be postponed
     until it is called again with YES for canPromptForRating. The rating alert
     can also be triggered by appEnteredForeground: and userDidSignificantEvent:
     (as long as you pass YES for canPromptForRating in those methods).
     */
    void appLaunched(bool canPromptForRating);
    
    /*
     Tells Appirater that the app was brought to the foreground on multitasking
     devices. You should call this method from the application delegate's
     applicationWillEnterForeground: method.
     
     If the app has been used enough to be rated (and enough significant events),
     you can suppress the rating alert
     by passing NO for canPromptForRating. The rating alert will simply be postponed
     until it is called again with YES for canPromptForRating. The rating alert
     can also be triggered by appLaunched: and userDidSignificantEvent:
     (as long as you pass YES for canPromptForRating in those methods).
     */
    void appEnteredForeground(bool canPromptForRating);
    
    /*
     Tells Appirater that the user performed a significant event. A significant
     event is whatever you want it to be. If you're app is used to make VoIP
     calls, then you might want to call this method whenever the user places
     a call. If it's a game, you might want to call this whenever the user
     beats a level boss.
     
     If the user has performed enough significant events and used the app enough,
     you can suppress the rating alert by passing NO for canPromptForRating. The
     rating alert will simply be postponed until it is called again with YES for
     canPromptForRating. The rating alert can also be triggered by appLaunched:
     and appEnteredForeground: (as long as you pass YES for canPromptForRating
     in those methods).
     */
    void userDidSignificantEvent(bool canPromptForRating);
    
    /*
     Tells Appirater to show the prompt (a rating alert). The prompt will be showed
     if there is connection available, the user hasn't declined to rate
     or hasn't rated current version.
     
     You could call to show the prompt regardless Appirater settings,
     e.g., in case of some special event in your app.
     */
    void showPrompt();
    
    /*
     Tells Appirater to open the App Store page where the user can specify a
     rating for the app. Also records the fact that this has happened, so the
     user won't be prompted again to rate the app.
     
     The only case where you should call this directly is if your app has an
     explicit "Rate this app" command somewhere.  In all other cases, don't worry
     about calling this -- instead, just call the other functions listed above,
     and let Appirater handle the bookkeeping of deciding when to ask the user
     whether to rate the app.
     */
    void rateApp();
    
    /*
     Tells Appirater to immediately close any open rating modals (e.g. StoreKit rating VCs).
     */
    void closeModal();
    
    /*
     Users will need to have the same version of your app installed for this many
     days before they will be prompted to rate it.
     */
    void setDaysUntilPrompt(double value);
    
    /*
     An example of a 'use' would be if the user launched the app. Bringing the app
     into the foreground (on devices that support it) would also be considered
     a 'use'. You tell Appirater about these events using the two methods:
     [Appirater appLaunched:]
     [Appirater appEnteredForeground:]
     
     Users need to 'use' the same version of the app this many times before
     before they will be prompted to rate it.
     */
    void setUsesUntilPrompt(int value);
    
    /*
     A significant event can be anything you want to be in your app. In a
     telephone app, a significant event might be placing or receiving a call.
     In a game, it might be beating a level or a boss. This is just another
     layer of filtering that can be used to make sure that only the most
     loyal of your users are being prompted to rate you on the app store.
     If you leave this at a value of -1, then this won't be a criterion
     used for rating. To tell Appirater that the user has performed
     a significant event, call the method:
     [Appirater userDidSignificantEvent:];
     */
    void setSignificantEventsUntilPrompt(int value);
    
    
    /*
     Once the rating alert is presented to the user, they might select
     'Remind me later'. This value specifies how long (in days) Appirater
     will wait before reminding them.
     */
    void setTimeBeforeReminding(double value);
    
    /*
     'YES' will show the Appirater alert everytime. Useful for testing how your message
     looks and making sure the link to your app's review page works.
     */
    void setDebug(bool debug);
    
    /*
     Set the delegate if you want to know when Appirater does something
     */
    void setDelegate(AppiraterDelegate *delegate);
    
    /*
     Set whether or not Appirater uses animation (currently respected when pushing modal StoreKit rating VCs).
     */
    void setUsesAnimation(bool animation);
    
    /*
     If set to YES, Appirater will open App Store link (instead of SKStoreProductViewController on iOS 6). Default NO.
     */
    void setOpenInAppStore(bool openInAppStore);
    
    /*
     If set to YES, the main bundle will always be used to load localized strings.
     Set this to YES if you have provided your own custom localizations in AppiraterLocalizable.strings
     in your main bundle.  Default is NO.
     */
    void setAlwaysUseMainBundle(bool useMainBundle);
    
private:
    Appirater();
};

} // namespace avalon

#endif /* AVALON_APPIRATER_H */

#endif /* AVALON_CONFIG_APPIRATER_ENABLED */
