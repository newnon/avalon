#include "avalon/ui/Alert.h"

#import <Foundation/Foundation.h>

namespace avalon {
namespace ui {

void showAlert(const std::string& title, const std::string& message, const Alert::ButtonList& buttons, Alert::Callback& delegate)
{
    NSAlert *alert = [[NSAlert alloc] init];
    if(buttons.empty())
    {
        [alert addButtonWithTitle:@"OK"];
    }
    else
    {
        for(auto it : buttons)
            [alert addButtonWithTitle:[NSString stringWithUTF8String:it.second.c_str()]];
    }
    [alert setMessageText:[NSString stringWithUTF8String:title.c_str()]];
    [alert setInformativeText:[NSString stringWithUTF8String:message.c_str()]];
    [alert setAlertStyle:NSWarningAlertStyle];
    
    if ([alert runModal] == NSAlertFirstButtonReturn) {
        // OK clicked, delete the record
    }
    [alert release];
}

} // namespace ui
} // namespace avalon