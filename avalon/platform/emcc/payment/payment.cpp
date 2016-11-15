#include "avalon/payment.h"

#include "emscripten.h"
#include "../cocos2d/external/json/document.h"

namespace avalon {
namespace payment {

void extFailProductsData(void* arg);
void extLoadProductsData(void* arg, void* ptr, int len);

class EmscriptenManager:public Manager
{
public:
    EmscriptenManager():_delegate(nullptr),_currency(emscripten_run_script_string("SocialObject.getCurrency()"))
    {
    }
    ~EmscriptenManager()
    {
    }
    virtual void addProduct(const std::string &id, const std::string &productIdentifier, bool consumable) override
    {
        Product product;
        product.id = id;
        product.productIdentifier = productIdentifier;
        product.consumable = consumable;
        addProduct(product);
    }
    virtual void addProduct(const Product &product) override
    {
        //printf("addProduct\n");
        _products.push_back(product);
    }

    virtual void clearProducts() override
    {
        //printf("getProducts\n");
        _products.clear();
    }

    virtual void requestProductsData() override
    {

        emscripten_async_wget_data("shop.json", 0, extLoadProductsData, extFailProductsData);

        //printf("requestProductsData\n");
    }

    void failProductsData(void* arg)
    {
        _delegate->onRequestProductsFail();
    }

    void loadProductsData(void* arg, void* ptr, int len)
    {
        char * json_data = new char[len + 1];
        memcpy(json_data, ptr, len);
        json_data[len] = 0;
        //printf("parse json .........  %d \n",  len);

        rapidjson::Document json;
        std::string content = std::string((char *)ptr);
        json.Parse(json_data);
        if (json.HasParseError())
        {
            //printf("parse error json \n*\n  %d, %d \n*\n", json.GetParseError(), json.GetErrorOffset());
            return;
        }

        const auto& currency = json.FindMember("currency");
        const auto& items = json["items"];
        // printf("parse error json %d items  ok \n", items.Size());
        // printf("parse json currency en %s, ru %s \n", currency["en"].GetString(), currency["ru"].GetString());
        
        std::vector<std::string> keys;
        if(currency != json.MemberEnd())
        {
			const auto &currentCurrency = currency->value[_currentLocale.c_str()];
			if(currentCurrency.IsArray())
			{
				for(rapidjson::SizeType i=0; i<currentCurrency.Size(); ++i)
				{
					keys.emplace_back(currentCurrency[i].GetString());
				}
			}
			else if(currentCurrency.IsString())
			{
				keys.emplace_back(currentCurrency.GetString());
			}
		}
        
        for (auto &product : _products)
        {
            product.localizedPrice = getLocalizedPrice(product.price, keys);
            product.currencyCode = _currency;
        }

        if (items.IsArray())
        {
            for (rapidjson::SizeType i = 0; i < items.Size(); i++)
            {
                auto& item = items[i];

                // printf("parse  json item %s \n", item["ID"].GetString());
                
                const std::string &productIdentifier = item["ID"].GetString();

                for (auto &product : _products)
                {
                    // printf("parse  json item %s == %s \n", product.productIdentifier.c_str(), item["ID"].GetString());

                    if (product.id == productIdentifier)
                    {
                        auto &localKey = item["LocaleKey"];
                        product.localizedName = std::string(localKey[_currentLocale.c_str()].GetString());

                        // printf("!!!!!!!!!!! parse json item  == en %s, ru %s \n", localKey["en"].GetString(), localKey["ru"].GetString());
                    }
                }

            }
        }

        _delegate->onRequestProductsSucceed();
        delete [] json_data;
    }

    virtual void setDelegate(ManagerDelegate *delegate) override
    {
        _delegate = delegate;
    }
    virtual ManagerDelegate *getDelegate() const override
    {
        return _delegate;
    }
    
    virtual const std::vector<Product>& getProducts() const override
    {
        printf("getProducts\n");
        return _products;
    }
    
    virtual const Product* getProduct(const std::string &id) const override
    {
        for(const auto &product:_products)
        {
            if(product.id == id)
                return &product;
        }
        return nullptr;
    }
    
    Product* getProductByProductIdentifier(const std::string &productIdentifier)
    {
        for(auto &product:_products)
        {
            if(product.productIdentifier == productIdentifier)
                return &product;
        }
        return nullptr;
    }
    
    const Product* getProductByProductIdentifier(const std::string &productIdentifier) const
    {
        for(const auto &product:_products)
        {
            if(product.productIdentifier == productIdentifier)
                return &product;
        }
        return nullptr;
    }
    
    std::string getLocalizedPrice(float price, const std::vector<std::string> &strings)
    {
    	if(strings.empty())
		{
			char temp[256] = {0};
			sprintf(temp, "%.2g", price);
			std::string priceStr = temp;
			return priceStr + " " + _currency;
		}
    	int count = static_cast<int>(price);
    	int countMod100 = count % 100;
    	int countMod10 = count % 10;
    	int result = 0;
    	if(countMod100 >= 5 && countMod100 <= 20)
    		result = 0;
    	else if(countMod10 == 1)
    		result = 1;
    	else if(countMod10 >= 2 && countMod10 <= 4)
    		result = 2;
    	if(result >= strings.size())
    		result = 0;
    	return std::to_string(count) + " " + strings[result];
    }
    
    virtual void purchase(const std::string &id) override
    {
        const avalon::payment::Product* productToPurchase = getProduct(id);
        if (!productToPurchase)
        {
            Transaction managerTransaction;
            managerTransaction.transactionState = TransactionState::Failed;
            managerTransaction.productId = id;
            _delegate->onPurchaseFail(managerTransaction, ManagerDelegateErrors::PRODUCT_UNKNOWN);
        }
        EM_ASM_({ SocialObject.buyItem(Pointer_stringify($0), Pointer_stringify($1), Pointer_stringify($2), Pointer_stringify($3), $4); }
        		, productToPurchase->localizedName.c_str()
        		, productToPurchase->localizedDescription.c_str()
        		, productToPurchase->id.c_str()
        		, productToPurchase->productIdentifier.c_str()
        		, productToPurchase->price);

    }
    
    void onPurchaseSucceed(const char * order_id, const char * item_id)
    {
        if (_delegate)
        {
            Transaction managerTransaction;
            managerTransaction.transactionState = TransactionState::Purchased;
            managerTransaction.productId = std::string(item_id);
            managerTransaction.transactionIdentifier = order_id;
            std::string order_s = std::string(order_id);
            std::copy(order_s.begin(), order_s.end(), std::back_inserter(managerTransaction.receipt));
            _delegate->onPurchaseSucceed(managerTransaction);
        }
    }
    void onPurchaseFailed(const char * error_id, const char * id)
    {
        if (_delegate)
        {
            Transaction managerTransaction;
            managerTransaction.transactionState = TransactionState::Purchased;
            managerTransaction.productId = id;
            ManagerDelegateErrors error;
            error = ManagerDelegateErrors::PRODUCT_UNKNOWN;
            _delegate->onPurchaseFail(managerTransaction, error);
        }
    }
    void onPurchaseCanceled(const char * id)
    {
        if (_delegate)
        {
            Transaction managerTransaction;
            managerTransaction.transactionState = TransactionState::Purchased;
            managerTransaction.productId = id;
            ManagerDelegateErrors error;
            error = ManagerDelegateErrors::PAYMENT_CANCELLED;
            _delegate->onPurchaseFail(managerTransaction, error);
        }
    }
    
    virtual bool isPurchaseReady() const override
    {
        return true;
    }
    
    virtual void restorePurchases() const override
    {
        std::vector<Transaction> managerTransactions;
        if (_delegate) {
            _delegate->onRestoreSucceed(managerTransactions);
        }
    }
    
    virtual void startService(const std::string &data)
    {
        if(!_started)
        {
            _started = true;
            if (_delegate) {
                _delegate->onServiceStarted();
            }
        }
    }
    
    virtual void stopService() override
    {
        if(_started)
            _started = false;
    }
    
    virtual bool isStarted() const override
    {
        return _started;
    }

    void setCurrentLocale(const std::string& locale) override
    {
        _currentLocale = locale;
    }
    
private:
    std::vector<Product> _products;
    ManagerDelegate *_delegate;
    bool _started;
    std::string _currentLocale;
    std::string _currency;
};

Manager *Manager::getInstance()
{
    static EmscriptenManager *manager = nullptr;
    if(!manager)
    {
        manager = new EmscriptenManager();
    }
    return manager;
}

void extFailProductsData(void* arg)
{
    ((EmscriptenManager*)Manager::getInstance())->failProductsData(arg);
}

void extLoadProductsData(void* arg, void* ptr, int len)
{
    ((EmscriptenManager*)Manager::getInstance())->loadProductsData(arg, ptr, len);
}

extern "C" {
    int EMSCRIPTEN_KEEPALIVE extOnOrderSuccess(const char * order_id, const char * item_id)
    {
       ((EmscriptenManager*) Manager::getInstance())->onPurchaseSucceed(order_id, item_id);
       return 0;
    }    
    int EMSCRIPTEN_KEEPALIVE extOnOrderFail(char * error_id, char * item_id)
    {
        ((EmscriptenManager*)Manager::getInstance())->onPurchaseFailed(error_id, item_id);
        return 0;
    }
    int EMSCRIPTEN_KEEPALIVE extOnOrderCancel( char * item_id)
    {
        ((EmscriptenManager*)Manager::getInstance())->onPurchaseCanceled(item_id);
        return 0;
    }
}

} // namespace payment
} // namespace avalon
