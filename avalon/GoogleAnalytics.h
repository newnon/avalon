#ifndef __GOOGLEANALYTICS_H__
#define __GOOGLEANALYTICS_H__

#include <string>
#include <map>

namespace avalon {
    
class GoogleAnalyticsTracker
{
public:
    friend class GoogleAnalytics;
    virtual void setParameter(const std::string &name, const std::string &value) = 0;
    virtual std::string getParameter(const std::string &name) const = 0;
    
    virtual void setSampleRate(float value) = 0;
    virtual float getSampleRate() const = 0;
    
    virtual const std::string &getTrackerId() const = 0;
    
    virtual bool getAllowIDFACollection() const = 0;
    virtual void setAllowIDFACollection(bool value) = 0;
    
    virtual void setScreenName(const std::string &name) = 0;
    virtual void setCustomDimension(int index, const std::string &name) = 0;
    virtual void setCustomMetric(int index, double value) = 0;
    virtual void setNewSession() = 0;
    
    virtual void sendAppView(const std::map<std::string, std::string> &params = {}) = 0;
    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value, const std::map<std::string, std::string> &params = {}) = 0;
    virtual void sendException(const std::string &description, bool fatal, const std::map<std::string, std::string> &params = {}) = 0;
    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode, const std::map<std::string, std::string> &params = {}) = 0;
    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target, const std::map<std::string, std::string> &params = {}) = 0;
    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label, const std::map<std::string, std::string> &params = {}) = 0;
    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode, const std::map<std::string, std::string> &params = {}) = 0;
    
protected:
    virtual ~GoogleAnalyticsTracker() {}
};
    
//windows conflict
#ifdef ERROR
#undef ERROR
#endif
    
enum class GoogleAnalyticsLogLevel
{
    NONE = 0,
    ERROR,
    WARNING,
    INFO,
    VERBOSE,
};
    
    
class GoogleAnalytics
{
public:
    void setDispatchInterval(int value);
    int getDispatchInterval() const;
    
    void setTrackUncaughtExceptions(bool value);
    bool getTrackUncaughtExceptions() const;
    
    void setDryRun(bool value);
    bool getDryRun() const;
    
    void setOptOut(bool value);
    bool getOptOut() const;
    
    static GoogleAnalytics *getInstance();
    
    GoogleAnalyticsTracker* getTracker(const std::string &trackingId);
    void removeTracker(GoogleAnalyticsTracker *tracker);
    
    GoogleAnalyticsTracker* getDefaultTracker();
    void setDefaultTracker(GoogleAnalyticsTracker *tracker);
    
    void dispatch();
    bool dispatchAndWait(int maxTimeSeconds);
    void sendHitsInBackground();
    
    void setLogLevel(GoogleAnalyticsLogLevel logLevel);
    GoogleAnalyticsLogLevel getLogLevel() const;
    
    void startSession();
    void endSession();
    
    static std::string customDimension(int index)
    {
        return "dimension" + std::to_string(index);
    }
    static std::string customMetric(int index)
    {
        return "metric" + std::to_string(index);
    }
    
protected:
    GoogleAnalytics();
    ~GoogleAnalytics();
    
private:
    std::map<std::string, GoogleAnalyticsTracker*> _trackers;
    GoogleAnalyticsTracker *_defaultTracker;
    GoogleAnalytics(const GoogleAnalytics &) = delete;
    GoogleAnalytics &operator =(const GoogleAnalytics &) = delete;
    
};
    
}

#endif /* __GOOGLEANALYTICS_H__ */
