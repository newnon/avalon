//
//  ShareManager.mm
//
//

#include "avalon/ShareManager.h"

@interface ShareText :UIActivityItemProvider
@property(nonatomic,readonly) NSString *text;
@property(nonatomic,readonly) NSString *longText;
@property(nonatomic,readonly) NSString *hashTag;
@property(nonatomic,readonly) NSString *appId;
@end

@implementation ShareText :UIActivityItemProvider
+ (instancetype)textWithText:(NSString *)text andLongText:(NSString *)longText andHashTag:(NSString *)hashTag andAppId:(NSString *)appId
{
    return [[ShareText alloc] initWithText:text andLongText:longText andHashTag:hashTag andAppId:appId];
}

- (instancetype)initWithText:(NSString *)text andLongText:(NSString *)longText andHashTag:(NSString *)hashTag andAppId:(NSString *)appId
{
    self =  [super initWithPlaceholderItem:text];
    if(self)
    {
        self->_text = text;
        self->_hashTag = hashTag;
        self->_appId = appId;
        self->_longText = longText;
    }
    return self;
}

- (id)item {
    if ([self.activityType isEqualToString:UIActivityTypeMail] || [self.activityType isEqualToString:UIActivityTypeMessage])
    {
        return [NSString stringWithFormat:@"%@ https://itunes.apple.com/app/id%@", _text, _appId];
    }
    else if ([self.activityType isEqualToString:UIActivityTypePostToTwitter])
    {
        if([_hashTag length] != 0)
        {
            if([_appId length] != 0)
                return [NSString stringWithFormat:@"%@ https://itunes.apple.com/app/id%@ #%@", _text, _appId, _hashTag];
            else
                return [NSString stringWithFormat:@"%@ #%@", _text, _hashTag];
        }
        else
        {
            if([_appId length] != 0)
                return [NSString stringWithFormat:@"%@ https://itunes.apple.com/app/id%@", _text, _appId];
            else
                return [NSString stringWithFormat:@"%@", _text];
        }
    }
    else
    {
        if([_hashTag length] != 0)
        {
            if([_appId length] != 0)
                return [NSString stringWithFormat:@"%@ https://itunes.apple.com/app/id%@ #%@", _longText, _appId, _hashTag];
            else
                return [NSString stringWithFormat:@"%@ #%@", _longText, _hashTag];
        }
        else
        {
            if([_appId length] != 0)
                return [NSString stringWithFormat:@"%@ https://itunes.apple.com/app/id%@", _longText, _appId];
            else
                return [NSString stringWithFormat:@"%@", _longText];
        }
    }
    return nil;
}
@end

namespace avalon {

class iOSShareManager : public ShareManager
{
public:
    virtual void setAppId(const std::string& appId)
    {
        _appId = appId;
    }
    
    virtual void setHashTag(const std::string& hashTag)
    {
        _hashTag = hashTag;
    }

    virtual void share(const std::string& text, const std::string& image)
    {
        share(text, text, image.empty()?nil:[UIImage imageNamed:[NSString stringWithUTF8String:image.c_str()]]);
    }
    
    virtual void share(const std::string& text, const std::string& longText, const std::string& image)
    {
        share(text, longText, image.empty()?nil:[UIImage imageNamed:[NSString stringWithUTF8String:image.c_str()]]);
    }
    
    virtual void shareScreenshot(const std::string& text)
    {
        shareScreenshot(text, text);
    }

    virtual void shareScreenshot(const std::string& text, const std::string& longText)
    {
        UIWindow *window = UIApplication.sharedApplication.keyWindow;
        UIGraphicsBeginImageContextWithOptions(window.bounds.size, NO, UIScreen.mainScreen.scale);
        [window drawViewHierarchyInRect:window.bounds afterScreenUpdates:YES];
        UIImage* image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        share(text, longText, image);
    }
    
    void share(const std::string& text, const std::string& longText, UIImage *image)
    {
        ShareText *shareText = nil;
        if(!text.empty())
        {
            shareText = [ShareText textWithText:[NSString stringWithUTF8String:text.c_str()] andLongText:[NSString stringWithUTF8String:longText.c_str()] andHashTag:[NSString stringWithUTF8String:_hashTag.c_str()] andAppId:[NSString stringWithUTF8String:_appId.c_str()]];
        }
        NSMutableArray* dataToShare = [NSMutableArray array];
        if(shareText)
            [dataToShare addObject:shareText];
        if(image)
            [dataToShare addObject:image];
        UIActivityViewController* activityViewController = [[UIActivityViewController alloc] initWithActivityItems:dataToShare applicationActivities:nil];
        activityViewController.excludedActivityTypes = @[
                                            UIActivityTypePrint,
                                            UIActivityTypeAssignToContact,
                                            UIActivityTypeAddToReadingList,
                                            UIActivityTypeAirDrop];
        if( [activityViewController respondsToSelector:@selector(popoverPresentationController)] )
        {
            // iOS8
            activityViewController.popoverPresentationController.sourceView = UIApplication.sharedApplication.keyWindow;
        }
        [UIApplication.sharedApplication.keyWindow.rootViewController presentViewController:activityViewController animated:YES completion:^{}];
    }

    iOSShareManager()
    {

    }
    
    virtual ~iOSShareManager()
    {
    }
    
private:
    std::string _appId;
    std::string _hashTag;
};
    
ShareManager* ShareManager::getInstance()
{
    static iOSShareManager* instance = new iOSShareManager();
    return instance;
}

} // namespace avalon