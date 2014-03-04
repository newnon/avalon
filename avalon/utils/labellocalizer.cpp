#include <avalon/utils/labellocalizer.h>

#include <avalon/i18n/Localization.h>
#include <avalon/i18n/LanguageKey.h>

namespace avalon {
namespace utils {
namespace labellocalizer {
    
static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void localize(const cocos2d::Node& node)
{
    using namespace cocos2d;
    using namespace std;
    using namespace avalon::utils::labellocalizer;
    using namespace avalon::i18n;

    for (auto& object : node.getChildren()) {
        auto ttfLabel = dynamic_cast<LabelTTF*>(object);
        if (ttfLabel) {
            auto label = ttfLabel->getString();
            vector<string> elements;
            split(label, '/', elements);
            if (elements.size() == 2) {
                ttfLabel->setString(_(elements[0].c_str(), elements[1].c_str()).get());
            }
        }

        auto subNode = dynamic_cast<Node*>(object);
        if (subNode) {
            localize(*subNode);
        }
    }
}

} // namespace labellocalizer
} // namespace utils
} // namespace avalon