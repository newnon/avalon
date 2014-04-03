//
//  BFWebView.h
//  CCXWebview
//
//  Created by Vincent on 12-11-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef NewPuzzle_BFWebView_h
#define NewPuzzle_BFWebView_h

#include <string>

namespace avalon {
namespace web {

class WebView
{
public:
    WebView();
    ~WebView();

    void showWebView(const std::string &url, float x, float y, float width, float height);

    
    void updateURL(const std::string &url);
        
    void removeWebView();
    
private:
    void *_data;

};
    
} // namespace web
} // namespace avalon

#endif