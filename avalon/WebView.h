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
    /**
     *	@brief	显示WebView
     *
     *	@param 	url 	地址
     *	@param 	x 	x位置
     *	@param 	y 	y位置(左上的位置，坐标系为左上0)
     *	@param 	width 	宽度
     *	@param 	height 	高度
     */
    void showWebView(const std::string &url, float x, float y, float width, float height);

    
    void updateURL(const std::string &url);
        
    void removeWebView();
    
private:
    void *_data;

};
    
} // namespace web
} // namespace avalon

#endif