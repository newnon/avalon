//
//  WebView_iOS.h
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIWebView.h>

@interface WebView_iOS : NSObject <UIWebViewDelegate>
{
    UIWebView* m_webview;
}

- (void)showWebView_x:(float)x y:(float)y width:(float) widht height:(float)height;

- (void)updateURL:(const char*)url;

- (void)removeWebView;

@end
