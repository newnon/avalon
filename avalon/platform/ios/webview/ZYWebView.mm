//
//  ZYWebView.cpp
//  CCXWebview
//
//  Created by Vincent on 12-11-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include "avalon/WebView.h"
#import "ZYWebView_iOS.h"

namespace avalon {
namespace web {

WebView::WebView()
{
    _data = [[ZYWebView_iOS alloc] init];
}

WebView::~WebView()
{
    [(ZYWebView_iOS*)_data release];
    _data = NULL;
}

void WebView::showWebView(const std::string &url, float x, float y, float width, float height)
{
    [(ZYWebView_iOS*)_data showWebView_x:x y:y width:width height:height];
    
    this->updateURL(url);
}

void WebView::updateURL(const std::string &url)
{
    [(ZYWebView_iOS*)_data updateURL:url.c_str()];
}

void WebView::removeWebView()
{
    [(ZYWebView_iOS*)_data removeWebView];
}
    
} // namespace web
} // namespace avalon
