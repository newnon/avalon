#include "avalon/AdMob.h"

namespace avalon {
    
class GADRequest;
    
class MACGADInterstitial:public GADInterstitial
{
public:
    
    MACGADInterstitial(GADRequest *request, const std::string &adUnitID, GADInterstitialDelegate *delegate):GADInterstitial(adUnitID)
    {
    }
    
    virtual bool isReady() const
    {
        return false;
    }
    
    virtual void show() override
    {
    }
    
    virtual ~MACGADInterstitial()
    {
    }
    
private:
};
    
    
class MACGADBannerView:public GADBannerView
{
public:
    MACGADBannerView(GADRequest *request, const std::string &adUnitID, GADAdSize size, CGRect rect, GADBannerViewDelegate *delegate):GADBannerView(adUnitID,size)
    {
    }
    
    virtual ~MACGADBannerView()
    {
    }
    
    virtual void setVisible(bool value)
    {
    }
    
    virtual bool isVisible()
    {
        return false;
    }
    
    virtual bool hasAutoRefreshed() const
    {
        return false;
    }
    
private:
};

    
    
class MACAdMob : public AdMob
{
public:
    
    MACAdMob(const std::string &version):AdMob(version),_gender(GADGender::Unknown),_tagForChildDirectedTreatment(false,false) {}
    
    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) override
    {
        _extras[network] = extras;
    }
    void setTestDevices(const std::vector<std::string>& devices) override
    {
        _testDevices = devices;
    }
    void setGender(GADGender gender) override
    {
        _gender = gender;
    }
    void setBirthDate(unsigned month, unsigned day, unsigned year) override
    {
        _birthDate.defined = true;
        _birthDate.month = month;
        _birthDate.day = day;
        _birthDate.year = year;
    }
    void setLocation(float latitude, float longitude, float accuracyInMeters) override
    {
        _location.defined = true;
        _location.latitude = latitude;
        _location.longitude = longitude;
        _location.accuracyInMeters = accuracyInMeters;
    }
    void setLocation(const std::string &location) override
    {
        _locationStr = location;
    }
    void setTagForChildDirectedTreatment(bool value) override
    {
        _tagForChildDirectedTreatment.first = true;
        _tagForChildDirectedTreatment.second = value;
    }
    void setKeywords(const std::vector<std::string>& keywords) override
    {
        _keywords = keywords;
    }
    
    std::shared_ptr<GADInterstitial> createIntestitial(const std::string &adUnitID, GADInterstitialDelegate *delegate) override
    {
        _interstitials.emplace_back(new MACGADInterstitial(createRequest(), adUnitID, delegate));
        return _interstitials.back();
    }
    std::shared_ptr<GADBannerView> createBannerView(const std::string &adUnitID, GADAdSize size, int x, int y, int width, int height, GADBannerViewDelegate *delegate) override
    {
        _bannerViews.emplace_back(new MACGADBannerView(createRequest(), adUnitID, size, CGRectMake(x, y, width, height), delegate));
        return _bannerViews.back();
    }
    
    virtual std::vector<std::shared_ptr<GADInterstitial>> getReadyInterstitials() const
    {
        std::vector<std::shared_ptr<GADInterstitial>> ret;
        for(auto &it : _interstitials)
        {
            if(it && it->isReady())
                ret.push_back(it);
            
        }
        return ret;
    }
    virtual std::vector<std::shared_ptr<GADBannerView>> getBannerViews() const
    {
        std::vector<std::shared_ptr<GADBannerView>> ret;
        for(auto &it : _bannerViews)
        {
            ret.push_back(it);
        }
        return ret;

    }
    
    GADRequest* createRequest() const
    {
        return nullptr;
    }
    
    virtual void removeBanner(const GADBannerView *bannerView) override
    {
        for (auto it = _bannerViews.begin();it!=_bannerViews.end();++it)
        {
            if(it->get() == bannerView)
            {
                _bannerViews.erase(it);
                break;
            }
        }
    }
    virtual void removeInterstitial(const GADInterstitial *interstitial) override
    {
        for (auto it = _interstitials.begin();it!=_interstitials.end();++it)
        {
            if(it->get() == interstitial)
            {
                _interstitials.erase(it);
                break;
            }
        }
    }
    
private:
    struct BirthDate
    {
        BirthDate():defined(false) {}
        bool defined;
        unsigned month;
        unsigned day;
        unsigned year;
    };
    struct Location
    {
        Location():defined(false) {}
        bool defined;
        unsigned latitude;
        unsigned longitude;
        unsigned accuracyInMeters;
    };
    std::map<GADAdNetworkExtras, std::map<std::string,std::string>> _extras;
    std::vector<std::string> _testDevices;
    GADGender _gender;
    BirthDate _birthDate;
    Location _location;
    std::string _locationStr;
    std::pair<bool,bool> _tagForChildDirectedTreatment;
    std::vector<std::string> _keywords;
    std::list<std::shared_ptr<GADBannerView>> _bannerViews;
    std::list<std::shared_ptr<GADInterstitial>> _interstitials;
};
    
AdMob *AdMob::getInstance()
{
    static MACAdMob *instance = new MACAdMob("fake");
    return instance;
}

}
