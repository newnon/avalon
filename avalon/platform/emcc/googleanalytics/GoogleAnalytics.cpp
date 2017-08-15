#include "avalon/GoogleAnalytics.h"
#include <emscripten/emscripten.h>
#include <vector>

namespace avalon {

    
class WinGAITracker: public GoogleAnalyticsTracker
{
public:
    virtual void setParameter(const std::string &name, const std::string &value) override
    {
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var name = Pointer_stringify($1);
            var value = Pointer_stringify($2);
            
            ga(trackId + '.set', name, value);
        }, _trackerIndex, name.c_str(), value.c_str());
    }
    
    virtual std::string getParameter(const std::string &name) const override
    {
        const std::string &script = "ga(function(tracker) { var param = tracker.get('" + name + "'); console.log('!!!! getParameter ' + param); }";
        const std::string &param = emscripten_run_script_string(script.c_str());
        return param;
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
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var name = Pointer_stringify($1);
            
            ga(trackId + '.set', 'screenName', name);
        }, _trackerIndex, name.c_str());
    }
    
    virtual void setCustomDimension(int index, const std::string &value) override
    {
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var value = Pointer_stringify($2);
            
            ga(trackId + '.set', 'dimension' + $1, value);
        }, _trackerIndex, index, value.c_str());
    }
    
    virtual void setCustomMetric(int index, double value) override
    {
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            ga(trackId + '.set', 'metric' + $1, $2);
        }, _trackerIndex, index, value);
    }
    
    virtual void setNewSession() override
    {
    }
    
    static void mapToArrays(const std::map<std::string, std::string> &params, std::vector<const char*> &keys, std::vector<const char*> &values)
    {
        keys.reserve(params.size());
        keys.reserve(params.size());
        
        int i=0;
        for(const auto &pair:params)
        {
            keys[i] = pair.first.c_str();
            values[i] = pair.second.c_str();
            ++i;
        }
    }
    
    virtual void sendAppView(const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            
            var hit = {};
            hit.hitType = 'screenview';
            
            for(var i=0; i<$3; ++i)
            {
                var key = Pointer_stringify(getValue($1 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($2 + i*4, 'i8*'));
                hit[key] = value; 
            }
            
            ga(trackId + '.send', hit);
        }, _trackerIndex, keys.data(), values.data(), params.size());
    }
    
    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value, const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var category = Pointer_stringify($1);
            var action = Pointer_stringify($2);
            var label = Pointer_stringify($3);
            
            var hit = {};
            hit.hitType = 'event';
            hit.eventCategory = category;
            hit.eventAction = action;
            hit.eventLabel = label;
            hit.eventValue = $4;
            
            for(var i=0; i<$7; ++i)
            {
                var key = Pointer_stringify(getValue($5 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($6 + i*4, 'i8*'));
                hit[key] = value; 
            }
            
            ga(trackId + '.send', hit);
        }, _trackerIndex, category.c_str(), action.c_str(), label.c_str(), value, keys.data(), values.data(), params.size());
    }
    
    virtual void sendException(const std::string &description, bool fatal, const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var description = Pointer_stringify($1);
            
            var hit = {};
            hit.hitType = 'exception';
            hit.exDescription = description;
            hit.exFatal = $2;
            
            for(var i=0; i<$5; ++i)
            {
                var key = Pointer_stringify(getValue($3 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($4 + i*4, 'i8*'));
                hit[key] = value; 
            }
            
            ga(trackId + '.send', hit);
        }, _trackerIndex, description.c_str(), fatal, keys.data(), values.data(), params.size());
    }

    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode, const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var transactionId = Pointer_stringify($1);
            var name = Pointer_stringify($2);
            var sku = Pointer_stringify($3);
            var category = Pointer_stringify($4);
            
            var hit = {};
            hit.id = transactionId;
            hit.name = name;
            hit.category = category;
            hit.price = $5;
            hit.quantity = $6;
            
            for(var i=0; i<$9; ++i)
            {
                var key = Pointer_stringify(getValue($7 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($8 + i*4, 'i8*'));
                hit[key] = value; 
            }
            
            ga(trackId + '.ec:addProduct', hit);
        }, _trackerIndex, transactionId.c_str(), name.c_str(), sku.c_str(), category.c_str(), price, quantity, keys.data(), values.data(), params.size());
    }

    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target, const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var network = Pointer_stringify($1);
            var action = Pointer_stringify($2);
            var target = Pointer_stringify($3);
            
            var hit = {};
            hit.hitType = 'social';
            hit.socialNetwork = network;
            hit.socialAction = action;
            hit.socialTarget = target;
            
            for(var i=0; i<$6; ++i)
            {
                var key = Pointer_stringify(getValue($4 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($5 + i*4, 'i8*'));
                hit[key] = value; 
            }

            ga(trackId + '.send', hit);
        }, _trackerIndex, network.c_str(), action.c_str(), target.c_str(), keys.data(), values.data(), params.size());
    }

    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label, const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var category = Pointer_stringify($1);
            var name = Pointer_stringify($3);
            var label = Pointer_stringify($4);
            
            var hit = {};
            hit.hitType = 'timing';
            hit.timingCategory = category;
            hit.timingValue = $2;
            hit.timingVar = name;
            hit.timingLabel = label;
            
            for(var i=0; i<$7; ++i)
            {
                var key = Pointer_stringify(getValue($5 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($6 + i*4, 'i8*'));
                hit[key] = value; 
            }
            
            ga(trackId + '.send', hit);
        }, _trackerIndex, category.c_str(), intervalMillis, name.c_str(), label.c_str(), keys.data(), values.data(), params.size());
    }

    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode, const std::map<std::string, std::string> &params) override
    {
        std::vector<const char*> keys;
        std::vector<const char*> values;
        mapToArrays(params, keys, values);
        
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var transactionId = Pointer_stringify($1);
            var affiliation = Pointer_stringify($2);
            
            var hit = {};
            hit.id = transactionId;
            hit.affiliation = affiliation;
            hit.revenue = $3;
            hit.tax = $4;
            hit.shipping = $5;
            
            for(var i=0; i<$8; ++i)
            {
                var key = Pointer_stringify(getValue($6 + i*4, 'i8*'));
                var value = Pointer_stringify(getValue($7 + i*4, 'i8*'));
                hit[key] = value; 
            }
            
            ga(trackId + '.ec:setAction', 'purchase', hit);
        }, _trackerIndex, transactionId.c_str(), affiliation.c_str(), revenue, tax, shipping, keys.data(), values.data(), params.size());
    }

    WinGAITracker(const std::string &trackerId, int trackerIndex):_trackerId(trackerId), _trackerIndex(trackerIndex)
    {
        EM_ASM_({
            var trackId = 'tracker_index_' + $0;
            var trackerId = Pointer_stringify($1);
            
            ga('create', trackerId, 'auto', trackId);
            ga(trackId + '.require', 'ec');
            ga(trackId + '.set', 'checkProtocolTask', null);
            ga(trackId + '.set', '&dm', 'web');
//            ga(trackId + '.set', '&ua', navigator.userAgent);
        }, _trackerIndex, _trackerId.c_str());
    }
    
    ~WinGAITracker()
    {
        
    }
    
private:
    std::string _trackerId;
    int _trackerIndex = 0;
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
    static int trackerId = 0;
    auto it = _trackers.find(trackingId);
    if(it != _trackers.end())
        return it->second;
    WinGAITracker *ret = new WinGAITracker(trackingId, trackerId++);
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
    EM_ASM({
        (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
            (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
            m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
        })(window,document,'script','//www.google-analytics.com/analytics.js','ga');
    });
}
    
GoogleAnalytics::~GoogleAnalytics()
{
    
}

}
