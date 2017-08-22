#include "avalon/GoogleAnalytics.h"
#include "avalon/platform/ios/utils/utils.h"
#import "GAI.h"
#import "GAITracker.h"
#import "GAIFields.h"
#import "GAIDictionaryBuilder.h"


namespace avalon {

    
class IOSGAITracker: public GoogleAnalyticsTracker
{
public:
    virtual void setParameter(const std::string &name, const std::string &value) override
    {
        [_tracker set:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding] value:[NSString stringWithCString:value.c_str() encoding:NSUTF8StringEncoding]];
    }
    
    virtual std::string getParameter(const std::string &name) const override
    {
        NSString *ret = [_tracker get:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding]];
        if(ret)
            return [ret cStringUsingEncoding:NSUTF8StringEncoding];
        else
            return "";
    }
    
    virtual void setSampleRate(float value) override
    {
        [_tracker set:kGAISampleRate value:[NSString stringWithFormat:@"%f", value]];
    }

    virtual float getSampleRate() const override
    {
        NSString *ret = [_tracker get:kGAISampleRate];
        if(ret)
            return [ret floatValue];
        else
            return 100.0;
    }
    
    virtual const std::string &getTrackerId() const override
    {
        return _trackerId;
    }
    
    virtual bool getAllowIDFACollection() const override
    {
        return [_tracker allowIDFACollection];
    }
    
    virtual void setAllowIDFACollection(bool value) override
    {
        [_tracker setAllowIDFACollection:value];
    }

    virtual void setScreenName(const std::string &name) override
    {
        [_tracker set:kGAIScreenName value:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding]];
    }
    
    virtual void setCustomDimension(int index, const std::string &value) override
    {
        [_tracker set:[::GAIFields customDimensionForIndex:index] value:[NSString stringWithCString:value.c_str() encoding:NSUTF8StringEncoding]];
    }
    virtual void setCustomMetric(int index, double value) override
    {
        [_tracker set:[::GAIFields customMetricForIndex:index] value:[NSString stringWithFormat:@"%lf", value]];
    }
    
    virtual void setNewSession() override
    {
        [_tracker set:kGAISessionControl  value:@"start"];
    }
    
    virtual void sendAppView(const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createScreenView];
        
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value, const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createEventWithCategory:[NSString stringWithCString:category.c_str() encoding:NSUTF8StringEncoding]
                                                                               action:[NSString stringWithCString:action.c_str() encoding:NSUTF8StringEncoding]
                                                                                label:[NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding]
                                                                                value:[NSNumber numberWithLong:value]];
        
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    virtual void sendException(const std::string &description, bool fatal, const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createExceptionWithDescription:[NSString stringWithCString:description.c_str() encoding:NSUTF8StringEncoding]
                                                                                   withFatal:[NSNumber numberWithBool:fatal]];
        
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode, const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createItemWithTransactionId:[NSString stringWithCString:transactionId.c_str() encoding:NSUTF8StringEncoding]
                                                                                     name:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding]
                                                                                      sku:[NSString stringWithCString:sku.c_str() encoding:NSUTF8StringEncoding]
                                                                                 category:[NSString stringWithCString:category.c_str() encoding:NSUTF8StringEncoding]
                                                                                    price:[NSNumber numberWithDouble:price]
                                                                                 quantity:[NSNumber numberWithLong:quantity]
                                                                             currencyCode:[NSString stringWithCString:currencyCode.c_str() encoding:NSUTF8StringEncoding]];
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target, const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createSocialWithNetwork:[NSString stringWithCString:network.c_str() encoding:NSUTF8StringEncoding]
                                                                               action:[NSString stringWithCString:action.c_str() encoding:NSUTF8StringEncoding]
                                                                               target:[NSString stringWithCString:target.c_str() encoding:NSUTF8StringEncoding]];
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label, const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createTimingWithCategory:[NSString stringWithCString:category.c_str() encoding:NSUTF8StringEncoding]
                                                                              interval:[NSNumber numberWithLong:intervalMillis]
                                                                                  name:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding]
                                                                                 label:[NSString stringWithCString:label.c_str() encoding:NSUTF8StringEncoding]];
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode, const std::map<std::string, std::string> &params) override
    {
        GAIDictionaryBuilder *builder = [GAIDictionaryBuilder createTransactionWithId:[NSString stringWithCString:transactionId.c_str() encoding:NSUTF8StringEncoding]
                                                                          affiliation:[NSString stringWithCString:affiliation.c_str() encoding:NSUTF8StringEncoding]
                                                                              revenue:[NSNumber numberWithDouble:revenue]
                                                                                  tax:[NSNumber numberWithDouble:tax]
                                                                             shipping:[NSNumber numberWithDouble:shipping]
                                                                         currencyCode:[NSString stringWithCString:currencyCode.c_str() encoding:NSUTF8StringEncoding]];
        for(const auto &it:params)
            [builder set:[NSString stringWithUTF8String:it.second.c_str()] forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        
        [_tracker send:[builder build]];
    }

    IOSGAITracker(const std::string &trackerId, id<GAITracker> tracker):_tracker(tracker),_trackerId(trackerId)
    {
        
    }
    ~IOSGAITracker()
    {
        
    }
    
private:
    id< GAITracker > _tracker;
    std::string _trackerId;
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
    [GAI sharedInstance].optOut = value;
}
bool GoogleAnalytics::getOptOut() const
{
    return [GAI sharedInstance].optOut;
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
    IOSGAITracker *ret = new IOSGAITracker(trackingId, [[GAI sharedInstance] trackerWithTrackingId:[NSString stringWithCString:trackingId.c_str() encoding:NSUTF8StringEncoding]]);
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
        [[GAI sharedInstance] removeTrackerByName:[NSString stringWithCString:trackerId.c_str() encoding:NSUTF8StringEncoding]];
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
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    }
    return true;
}
    
static BOOL taskExpired = NO;
static UIBackgroundTaskIdentifier taskId = 0;
    
static void dispatchHandler(GAIDispatchResult result)
{
    if (result == kGAIDispatchGood && !taskExpired) {
        [[GAI sharedInstance] dispatchWithCompletionHandler:^(GAIDispatchResult result) { dispatchHandler(result); }];
    } else {
        [[UIApplication sharedApplication] endBackgroundTask:taskId];
    }
}

void GoogleAnalytics::sendHitsInBackground()
{
    taskExpired = NO;
    
    taskId = [[UIApplication sharedApplication] beginBackgroundTaskWithExpirationHandler:^{
        taskExpired = YES;
    }];
    
    if (taskId == UIBackgroundTaskInvalid) {
        return;
    }
    
    [[GAI sharedInstance] dispatchWithCompletionHandler:^(GAIDispatchResult result) { dispatchHandler(result); }];
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
