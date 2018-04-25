//
//  Amplitude.h
//  Amplitude
//
//  Created by Vladimir Shlobin on 24.04.2018.
//

#ifndef Amplitude_h
#define Amplitude_h

#include <map>
#include <string>

namespace avalon { namespace amplitude {

void init();

void logEvent(const std::string &name, const std::map<std::string, std::string> &params);

void logRevenue(const std::string &productId, int quantity, double price, const std::string &revenueType, const std::string &receipt, const std::string &receiptSig, const std::map<std::string, std::string> &params);

} } // namespace avalon::amplitude

#endif /* Amplitude_h */
