#ifndef AVALON_PAYMENT_PRODUCT_H
#define AVALON_PAYMENT_PRODUCT_H

#include <string>

namespace avalon {
namespace payment {

class Manager;

class Product
{
    friend class Manager;

public:
    float price;
    std::string localizedPrice;
    std::string localizedName;
    std::string localizedDescription;

    explicit Product(const std::string &productId);
    virtual ~Product();

    const std::string &getProductId() const;

    bool canBePurchased() const;
    void purchase();

    void onHasBeenPurchased();
    bool hasBeenPurchased() const;
    virtual void consume();

protected:
    int purchasedCounter;
    Manager* manager;

private:
    const std::string productId;
    
    Product( const Product& ) = delete;
    Product& operator=( const Product& ) = delete;
};

} // namespace payment
} // namespace avalon

#endif /* AVALON_PAYMENT_PRODUCT_H */
