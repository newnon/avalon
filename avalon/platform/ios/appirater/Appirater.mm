#include "avalon/Appirater.h"

#import "avalon/platform/ios/appirater/Appirater/Appirater.h"

@interface IOSAppiraterDelegate : NSObject<AppiraterDelegate>
@end

@implementation IOSAppiraterDelegate
-(void)appiraterDidDisplayAlert:(Appirater *)appirater
{
    
}
-(void)appiraterDidDeclineToRate:(Appirater *)appirater
{
    
}
-(void)appiraterDidOptToRate:(Appirater *)appirater
{
    
}
-(void)appiraterDidOptToRemindLater:(Appirater *)appirater
{
    
}
-(void)appiraterWillPresentModalView:(Appirater *)appirater animated:(BOOL)animated
{
}
-(void)appiraterDidDismissModalView:(Appirater *)appirater animated:(BOOL)animated
{
}
@end

namespace avalon {
    
static IOSAppiraterDelegate *delegate = nil;

Appirater* Appirater::getInstance()
{
    static Appirater* instance = new Appirater();
    return instance;
}

void Appirater::setAppId(const char *appName)
{
    [::Appirater setAppId:[NSString stringWithUTF8String:appName]];
}

void Appirater::appLaunched(bool canPromptForRating)
{
    [::Appirater appLaunched:canPromptForRating];
}

void Appirater::appEnteredForeground(bool canPromptForRating)
{
    [::Appirater appEnteredForeground:canPromptForRating];
}

void Appirater::userDidSignificantEvent(bool canPromptForRating)
{
    [::Appirater userDidSignificantEvent:canPromptForRating];
}

bool Appirater::tryToShowPrompt()
{
    return [::Appirater tryToShowPrompt];
}
    
void Appirater::forceShowPrompt(bool displayRateLaterButton)
{
    [::Appirater forceShowPrompt:displayRateLaterButton];
}
    
void Appirater::rateApp()
{
    [::Appirater rateApp];
}

void Appirater::closeModal()
{
    [::Appirater closeModal];
}

void Appirater::setDaysUntilPrompt(double value)
{
    [::Appirater setDaysUntilPrompt:value];
}
void Appirater::setUsesUntilPrompt(int value)
{
    [::Appirater setUsesUntilPrompt:value];
}

void Appirater::setSignificantEventsUntilPrompt(int value)
{
    [::Appirater setSignificantEventsUntilPrompt:value];
}

void Appirater::setTimeBeforeReminding(double value)
{
    [::Appirater setTimeBeforeReminding:value];
}

void Appirater::setDebug(bool debug)
{
    [::Appirater setDebug:debug];
}

void Appirater::setDelegate(AppiraterDelegate *delegate)
{
    
}

void Appirater::setUsesAnimation(bool animation)
{
    [::Appirater setUsesAnimation:animation];
}

void Appirater::setOpenInAppStore(bool openInAppStore)
{
    [::Appirater setOpenInAppStore:openInAppStore];
}

void Appirater::setAlwaysUseMainBundle(bool useMainBundle)
{
    [::Appirater setAlwaysUseMainBundle:useMainBundle];
}

Appirater::Appirater()
{
    delegate = [[IOSAppiraterDelegate alloc] init];
    [::Appirater setDelegate:delegate];
}

} // namespace avalon
