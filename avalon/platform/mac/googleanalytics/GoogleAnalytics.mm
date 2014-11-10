#include "avalon/GoogleAnalytics.h"
#import "KKGAI.h"
#import "KKGAIDictionaryBuilder.h"
#import "KKGAIFields.h"

namespace avalon {

    
class MacGAITracker: public GoogleAnalyticsTracker
{
public:
    virtual void setParameter(const std::string &name, const std::string &value) override
    {
        [_tracker set:NSStringFromString(name) value:NSStringFromString(value)];
    }
    
    virtual std::string getParameter(const std::string &name) const override
    {
        NSString *ret = [_tracker get:NSStringFromString(name)];
        if(ret)
            return [ret cStringUsingEncoding:NSUTF8StringEncoding];
        else
            return "";
    }
    
    virtual void setSampleRate(float value) override
    {
    }
    
    virtual float getSampleRate() const override
    {
        return 100.0;
    }
    
    virtual const std::string &getTrackerId() const override
    {
        return _trackerId;
    }

    virtual void setScreenName(const std::string &name) override
    {
        [_tracker set:kGAIScreenName value:NSStringFromString(name)];
    }
    
    virtual void setCustomDimension(int index, const std::string &value) override
    {
        [_tracker set:[GAIFields customDimensionForIndex:index] value:NSStringFromString(value)];
    }
    
    virtual void setCustomMetric(int index, float value) override
    {
        [_tracker set:[GAIFields customMetricForIndex:index] value:[NSString stringWithFormat:@"%f",value]];
    }
    
    virtual void setNewSession() override
    {
        [_tracker set:@"start" value:kGAISessionControl];
    }
    
    virtual void sendAppView() override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createScreenView];
        [_tracker send:[builder build]];
    }

    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createEventWithCategory:NSStringFromString(category)
                                                                                 action:NSStringFromString(action)
                                                                                  label:NSStringFromString(label)
                                                                                  value:[NSNumber numberWithLong:value]];
        [_tracker send:[builder build]];
    }

    virtual void sendException(const std::string &description, bool fatal) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createExceptionWithDescription:NSStringFromString(description) withFatal:[NSNumber numberWithBool:fatal]];
        [_tracker send:[builder build]];
    }

    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createItem:NSStringFromString(transactionId)
                                                                    name:NSStringFromString(name)
                                                                     SKU:NSStringFromString(sku)
                                                               cateogory:NSStringFromString(category)
                                                                   price:[NSNumber numberWithDouble:price]
                                                                quantity:[NSNumber numberWithLong:quantity]
                                                            currencyCode:NSStringFromString(currencyCode)];
        [_tracker send:[builder build]];
    }

    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createSocialWithNetwork:NSStringFromString(network)
                                                                               action:NSStringFromString(action)
                                                                               target:NSStringFromString(target)];
        [_tracker send:[builder build]];
    }

    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createTimingWithCategory:NSStringFromString(category)
                                                                              interval:[NSNumber numberWithLong:intervalMillis]
                                                                                  name:NSStringFromString(name)
                                                                                 label:NSStringFromString(label)];
        [_tracker send:[builder build]];
    }

    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createTransaction:NSStringFromString(transactionId)
                                                                     affliation:NSStringFromString(affiliation)
                                                                        revenue:[NSNumber numberWithDouble:revenue]
                                                                       shipping:[NSNumber numberWithDouble:tax]
                                                                            tax:[NSNumber numberWithDouble:shipping]
                                                                   currencyCode:NSStringFromString(currencyCode)];
        [_tracker send:[builder build]];
    }

    MacGAITracker(const std::string &trackerId, id<GAITracker> tracker):_tracker(tracker),_trackerId(trackerId)
    {
        [_tracker retain];
    }
    ~MacGAITracker()
    {
        [_tracker release];
    }
    
private:
    static NSString *NSStringFromString(const std::string &string)
    {
        return [NSString stringWithCString:string.c_str() encoding:NSUTF8StringEncoding];
    }
    std::string _trackerId;
    id<GAITracker> _tracker;
};
    
void GoogleAnalytics::setDispatchInterval(int value)
{
    [GAI sharedInstance].dispatchInterval = value;
}
    
int GoogleAnalytics::getDispatchInterval() const
{
    return [GAI sharedInstance].dispatchInterval;
}
    
void GoogleAnalytics::setTrackUncaughtExceptions(bool value)
{
    [GAI sharedInstance].trackUncaughtExceptions = value;
}
    
bool GoogleAnalytics::getTrackUncaughtExceptions() const
{
    return [GAI sharedInstance].trackUncaughtExceptions;
}

void GoogleAnalytics::setDryRun(bool value)
{
    [GAI sharedInstance].dryRun = value;
}
    
bool GoogleAnalytics::getDryRun() const
{
    return [GAI sharedInstance];
}

void GoogleAnalytics::setOptOut(bool value)
{
}
bool GoogleAnalytics::getOptOut() const
{
    return false;
}

GoogleAnalytics *GoogleAnalytics::getInstance()
{
    static GoogleAnalytics* instance = new GoogleAnalytics();
    return instance;
}

GoogleAnalyticsTracker* GoogleAnalytics::getTracker(const std::string &trackingId)
{
    auto it = _trackers.find(trackingId);
    if(it != _trackers.end())
        return it->second;
    id<GAITracker> traker =  [[GAI sharedInstance] trackerWithTrackingId:[NSString stringWithCString:trackingId.c_str() encoding:NSUTF8StringEncoding]];
    MacGAITracker *ret = new MacGAITracker(trackingId, traker);
    _trackers.insert(std::make_pair(trackingId, ret));
    if(!_defaultTracker)
        _defaultTracker = ret;
    return ret;
}
void GoogleAnalytics::removeTracker(GoogleAnalyticsTracker *tracker)
{
    if(tracker)
    {
        const std::string &trackerId = tracker->getTrackerId();
        _trackers.erase(trackerId);
        delete tracker;
    }
}
    
GoogleAnalyticsTracker* GoogleAnalytics::getDefaultTracker()
{
    return _defaultTracker;
}
    
void GoogleAnalytics::setDefaultTracker(GoogleAnalyticsTracker *tracker)
{
    _defaultTracker = tracker;
}

void GoogleAnalytics::dispatch()
{
    [[GAI sharedInstance] dispatch];
}
    
bool GoogleAnalytics::dispatchAndWait(int maxTimeSeconds)
{
    NSDate *start = [NSDate date];
    __block bool finished;
    [[GAI sharedInstance] dispatchWithCompletionHandler:^(GAIDispatchResult) {
        finished = true;
    }];
    while(true)
    {
        if(finished)
        {
            NSLog(@"GoogleAnalytics succesfull dispatchAndWait");
            break;
        }
        else if(fabs([start timeIntervalSinceNow])>maxTimeSeconds)
        {
            NSLog(@"GoogleAnalytics failed dispatchAndWait");
            break;
        }
        [NSThread sleepForTimeInterval:0.1];
    }
    return true;
}
    
void GoogleAnalytics::setLogLevel(GoogleAnalyticsLogLevel logLevel)
{
    [[GAI sharedInstance] logger].logLevel = (GAILogLevel)logLevel;
}

GoogleAnalyticsLogLevel GoogleAnalytics::getLogLevel() const
{
    return static_cast<GoogleAnalyticsLogLevel>([[GAI sharedInstance] logger].logLevel);
}
    
void GoogleAnalytics::startSession()
{
        
}
    
void GoogleAnalytics::endSession()
{
        
}

GoogleAnalytics::GoogleAnalytics():_defaultTracker(nullptr)
{
    
}
GoogleAnalytics::~GoogleAnalytics()
{
    
}

}
