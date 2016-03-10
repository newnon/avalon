#include "avalon/GoogleAnalytics.h"


namespace avalon {

    
class WinGAITracker: public GoogleAnalyticsTracker
{
public:
    virtual void setParameter(const std::string &name, const std::string &value) override
    {
    }
    
    virtual std::string getParameter(const std::string &name) const override
    {
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
    
    virtual bool getAllowIDFACollection() const override
    {
        return false;
    }
    
    virtual void setAllowIDFACollection(bool value) override
    {
    }
    virtual void setScreenName(const std::string &name) override
    {
    }
    
    virtual void setCustomDimension(int index, const std::string &value) override
    {
    }
    
    virtual void setCustomMetric(int index, double value) override
    {
    }
    
    virtual void setNewSession() override
    {
    }
    
    virtual void sendAppView() override
    {
    }

    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value) override
    {
    }

    virtual void sendException(const std::string &description, bool fatal) override
    {
    }

    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode) override
    {
    }

    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target) override
    {
    }

    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label) override
    {
    }

    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode) override
    {
    }

    WinGAITracker(const std::string &trackerId/*, id<GAITracker> tracker*/):/*_tracker(tracker),*/_trackerId(trackerId)
    {
        
    }
    ~WinGAITracker()
    {
        
    }
    
private:
    std::string _trackerId;
};
    
void GoogleAnalytics::setDispatchInterval(int value)
{
}

int GoogleAnalytics::getDispatchInterval() const
{
    return 0;
}
    
void GoogleAnalytics::setTrackUncaughtExceptions(bool value)
{
}

bool GoogleAnalytics::getTrackUncaughtExceptions() const
{
    return false;
}

void GoogleAnalytics::setDryRun(bool value)
{
}

bool GoogleAnalytics::getDryRun() const
{
    return false;
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
    WinGAITracker *ret = new WinGAITracker(trackingId);
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
}

bool GoogleAnalytics::dispatchAndWait(int maxTimeSeconds)
{
    return false;
}

void GoogleAnalytics::sendHitsInBackground() 
{
}
    
void GoogleAnalytics::setLogLevel(GoogleAnalyticsLogLevel logLevel)
{
}

GoogleAnalyticsLogLevel GoogleAnalytics::getLogLevel() const
{
    return GoogleAnalyticsLogLevel::NONE;
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
