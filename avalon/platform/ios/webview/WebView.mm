//
//  WebView.mm
//
//

#include "avalon/WebView.h"
#import "WebView_iOS.h"

namespace avalon {
namespace web {

WebView::WebView()
{
    _data = [[WebView_iOS alloc] init];
}

WebView::~WebView()
{
    [(WebView_iOS*)_data release];
    _data = NULL;
}

void WebView::showWebView(const std::string &url, float x, float y, float width, float height)
{
    [(WebView_iOS*)_data showWebView_x:x y:y width:width height:height];
    
    this->updateURL(url);
}

void WebView::updateURL(const std::string &url)
{
    [(WebView_iOS*)_data updateURL:url.c_str()];
}

void WebView::removeWebView()
{
    [(WebView_iOS*)_data removeWebView];
}
    
} // namespace web
} // namespace avalon
