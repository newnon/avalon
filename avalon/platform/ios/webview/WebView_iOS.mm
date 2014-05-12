//
//  ZYWebView_iOS.m
//  CCXWebview
//
//  Created by Vincent on 12-11-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "WebView_iOS.h"
#import <UIKit/UIImageView.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
//#import "EAGLView.h"

@implementation WebView_iOS


- (void)showWebView_x:(float)x y:(float)y width:(float) widht height:(float)height
{
    if (!m_webview)
    {
        float scaleFactor = [UIScreen mainScreen].scale;
        m_webview = [[UIWebView alloc] initWithFrame:CGRectMake(x/scaleFactor, y/scaleFactor, widht/scaleFactor , height/scaleFactor)];
        [m_webview setDelegate:self];
        
        [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:m_webview];
        [m_webview release];
        
        m_webview.backgroundColor = [UIColor clearColor];
        m_webview.opaque = NO;
        
        for (UIView *aView in [m_webview subviews])  
        { 
            if ([aView isKindOfClass:[UIScrollView class]])  
            { 
                UIScrollView* scView = (UIScrollView *)aView;
                
//              [(UIScrollView *)aView setShowsVerticalScrollIndicator:NO];
                [scView setShowsHorizontalScrollIndicator:NO];
//              scView.bounces = NO;
                
                for (UIView *shadowView in aView.subviews)  
                {
                    if ([shadowView isKindOfClass:[UIImageView class]]) 
                    { 
                        shadowView.hidden = YES;
                    } 
                } 
            } 
        }  
    }
}

- (void)updateURL:(const char*)url
{
    NSString *request = [NSString stringWithUTF8String:url];
    [m_webview loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:request] 
                                            cachePolicy:NSURLRequestReloadIgnoringLocalCacheData 
                                        timeoutInterval:60]];
}

-(void)dealloc
{
    [self removeWebView];
    [super dealloc];
}

- (void)removeWebView
{
    [m_webview removeFromSuperview];
    m_webview = NULL;
}

#pragma mark - WebView
- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    return true;
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    
}

@end
