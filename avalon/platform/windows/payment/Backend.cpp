#include "avalon/payment/Backend.h"

#include "avalon/payment/Manager.h"
#include "avalon/payment/ManagerDelegate.h"
#include "avalon/payment/Product.h"

namespace avalon {
namespace payment {

Backend::Backend(Manager& manager)
: manager(manager)
{
}

Backend::~Backend()
{
    shutdown();
}

bool Backend::isInitialized() const
{
    return false;
}

void Backend::initialize(const std::string &data)

}

void Backend::shutdown()
{
}

void Backend::purchase(Product* const product)
{
}

bool Backend::isPurchaseReady() const
{
    return false;
}

void Backend::restorePurchases() const
{
}

} // namespace payment
} // namespace avalon
