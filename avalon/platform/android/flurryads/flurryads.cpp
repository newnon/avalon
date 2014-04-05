//
//  AnalyticX.cpp
//  AnalyticX
//
//  Created by diwwu on 5/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "avalon/FlurryAds.h"

#include <jni.h>

namespace avalon {

void FlurryAds::fetchAdForSpace(const std::string &space, int x,int y,int width,int heignt, AdSize size)
{
}

bool FlurryAds::adReadyForSpace(const std::string &space)
{
	return false;
}
void FlurryAds::displayAdForSpace(const std::string &space)
{
}
void FlurryAds::displayAdForSpaceModally(const std::string &space)
{
}
void FlurryAds::fetchAndDisplayAdForSpace(const std::string &space, void *viewContainer, AdSize size)
{
}

void FlurryAds::removeAdFromSpace(const std::string &space)
{
}
void FlurryAds::initialize()
{
}

void FlurryAds::setAdDelegate(FlurryAdsDelegate *delegate)
{

}
void FlurryAds::enableTestAds(bool enable)
{
}
void FlurryAds::setUserCookies(const std::map<std::string,std::string> &userCookies)
{
}
void FlurryAds::clearUserCookies()
{
}
void FlurryAds::setKeywordsForTargeting(const std::map<std::string,std::string> &keywords)
{
}
void FlurryAds::clearKeywords()
{
}

} // namespace avalon






