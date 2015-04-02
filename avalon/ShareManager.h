#ifndef AVALON_SHAREMANAGER_H
#define AVALON_SHAREMANAGER_H

#include <string>

namespace avalon {

class ShareManager;
    
class ShareManagerDelegate
{
    virtual ~ShareManagerDelegate() {}
};

class ShareManager
{
public:
    static ShareManager* getInstance();

    /*
     Set your Apple generated software id here.
     */
    virtual void setAppId(const std::string& appId) = 0;
    
    /*
     Set your hashTag here.
     */
    virtual void setHashTag(const std::string& hashTag) = 0;
    
    /*
     Share your text & image.
     */
    virtual void share(const std::string& text, const std::string& image) = 0;
    
    /*
     Share your text & screenshot.
     */
    virtual void shareScreenshot(const std::string& text) = 0;
    
    /*
     Share your text & image.
     */
    virtual void share(const std::string& text, const std::string& longText, const std::string& image) = 0;
    
    /*
     Share your text & screenshot.
     */
    virtual void shareScreenshot(const std::string& text, const std::string& longText) = 0;
    
    /*
     Share your string without adding anything.
     */
    virtual void shareString(const std::string& text) = 0;
    
    virtual ~ShareManager() {}
};

} // namespace avalon

#endif /* AVALON_SHAREMANAGER_H */
