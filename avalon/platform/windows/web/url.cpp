#include "avalon/url.h"
#include <shellapi.h>

namespace avalon {
namespace web {

void openUrl(const std::string &url)
{
    WCHAR *temp = new WCHAR[url.size() + 1];
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), url.size() + 1, temp, url.size() + 1);
    ShellExecuteW(NULL, L"open", temp, NULL, NULL, SW_SHOWNORMAL);
    delete[] temp;
}

} // namespace web
} // namespace avalon