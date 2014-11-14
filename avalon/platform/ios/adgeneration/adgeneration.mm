#include "avalon/AdGeneration.h"
#import "ADGManagerViewController.h"

namespace avalon {
    
AdGenerationSize makeCustomAdGenerationAdSize(unsigned short width, unsigned short height)
{
    return static_cast<AdGenerationSize>(width<<16 | height);
}
    
class IOSAdGenerationBannerView: public AdGenerationBannerView
{
public:
    virtual bool isVisible() override
    {
        return [_adsView isDescendantOfView:[UIApplication sharedApplication].keyWindow];
    }
    virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
        [_controller resumeRefresh];
        CGRect rect = CGRectMake(x, y, width, height);
        
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES) {
            float scale = [[UIScreen mainScreen] scale];
            rect.origin.x /= scale;
            rect.origin.y /= scale;
            rect.size.width /= scale;
            rect.size.height /= scale;
        }
        
        CGRect frame = [UIApplication sharedApplication].keyWindow.frame;
        
        rect.origin.y = frame.size.height - rect.size.height - rect.origin.y;
        [_adsView setFrame:rect];
        [_controller setFrame:CGRectMake(0, 0, rect.size.width, rect.size.height)];
        [[UIApplication sharedApplication].keyWindow addSubview:_adsView];
    }
    virtual void hide() override
    {
        [_adsView removeFromSuperview];
        [_controller pauseRefresh];
    }
    
    IOSAdGenerationBannerView(const std::string &adUnitID, AdGenerationSize size):AdGenerationBannerView(adUnitID, size)
    {
        unsigned int scale = 1;
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES)
            scale = [[UIScreen mainScreen] scale];
        
        unsigned short width = 0;
        unsigned short height = 0;
        ::ADGAdType bannerSize;
        switch (size) {
            case AdGenerationSize::SP:
                bannerSize = kADG_AdType_Sp;
                break;
            case AdGenerationSize::TABLET:
                bannerSize = kADG_AdType_Large;
                break;
            case AdGenerationSize::LARGE:
                bannerSize = kADG_AdType_Rect;
                break;
            case AdGenerationSize::RECT:
                bannerSize = kADG_AdType_Tablet;
                break;
            default:
            {
                width = static_cast<unsigned short>((static_cast<unsigned int>(size)/scale)>>16);
                height = static_cast<unsigned short>(static_cast<unsigned int>(size)/scale);
                bannerSize = kADG_AdType_Free;
            }
                break;
        }
        
        NSDictionary *adgparam = @{
                                   @"locationid" : @"10723",
                                   @"adtype" : @(bannerSize),
                                   @"w" : @(width),
                                   @"h" : @(height)
                                   };
        _adsView = [[UIView alloc] initWithFrame:[UIApplication sharedApplication].keyWindow.frame];
        
        _controller = [[ADGManagerViewController alloc] initWithAdParams :adgparam :_adsView];
        [_controller loadRequest];
        [_controller pauseRefresh];
    }
    ~IOSAdGenerationBannerView()
    {
        [_controller release];
        [_adsView release];
    }
private:
    ADGManagerViewController *_controller;
    UIView *_adsView;
};
    
AdGenerationBannerView* AdGenerationBannerView::create(const std::string &adUnitID, AdGenerationSize size)
{
    return new IOSAdGenerationBannerView(adUnitID, size);
}

}
