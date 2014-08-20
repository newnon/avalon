#include "avalon/url.h"

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

namespace avalon {
namespace web {

void openUrl(const std::string &url)
{
    NSString* msg = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
    NSURL* nsUrl = [NSURL URLWithString:msg];
    [[UIApplication sharedApplication] openURL:nsUrl];
}

} // namespace web
} // namespace avalon