#include "avalon/utils/url.h"

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

namespace avalon {
namespace utils {

void openUrl(const std::string &url)
{
    NSString* msg = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
    NSURL* nsUrl = [NSURL URLWithString:msg];
    [[UIApplication sharedApplication] openURL:nsUrl];
}

} // namespace utils
} // namespace avalon