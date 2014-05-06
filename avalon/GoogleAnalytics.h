#ifndef __GOOGLEANALYTICS_H__
#define __GOOGLEANALYTICS_H__

#include <string>
#include <map>

namespace avalon {
    
class GAIFields{
public:
    static const std::string &getContentGroup(int index);
    static const std::string &getCustomDimension(int index);
    static const std::string &getCustomMetric(int index);
};
    
class GoogleAnalyticsTracker
{
public:
    virtual void setParameter(const std::string &name, const std::string &value) = 0;
    virtual std::string getParameter(const std::string &name) const = 0;
    
    virtual void setSampleRate(float value) = 0;
    virtual float GetSampleRate() const = 0;
    
    virtual const std::string &getTrackerId() const = 0;
    
    virtual void sendAppView(const std::string &name) = 0;
    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value) = 0;
    virtual void sendException(const std::string &description, bool fatal) = 0;
    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode) = 0;
    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target) = 0;
    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label) = 0;
    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode) = 0;
    
protected:
    ~GoogleAnalyticsTracker() {}
};
    
    
class GoogleAnalytics
{
public:
    void setDispatchInterval(double value);
    double getDispatchInterval() const;
    
    void setTrackUncaughtExceptions(bool value);
    bool getTrackUncaughtExceptions() const;
    
    void setDryRun(bool value);
    bool getDryRun() const;
    
    void setOptOut(bool value);
    bool getOptOut() const;
    
    static GoogleAnalytics *getInstance();
    
    GoogleAnalyticsTracker* getTracker(const std::string &trackingId);
    void removeTracker(GoogleAnalyticsTracker *tracker);
    
    void dispatch();
    
protected:
    GoogleAnalytics();
    ~GoogleAnalytics();
    
private:
    std::map<std::string, GoogleAnalyticsTracker*> _trackers;
    GoogleAnalytics(const GoogleAnalytics &) = delete;
    GoogleAnalytics &operator =(const GoogleAnalytics &) = delete;
    
};
    
}

#endif /* __GOOGLEANALYTICS_H__ */
