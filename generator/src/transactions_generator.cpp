#include "transactions_generator.hpp"

#include <string>
#include <vector>
#include <tuple>
#include <random>

#include "currency_manager.hpp"

struct Product {
	std::string productId;
	std::int64_t basePrice;
	Currency currencyCode;

	Product(std::string productId, std::int64_t basePrice, Currency currencyCode) :
		productId(std::move(productId)), basePrice(basePrice), currencyCode(currencyCode) {
	}
};

TransactionGenerator::TransactionGenerator(pqxx::connection& databaseConnection)
	: mDatabaseConnection(databaseConnection) {
}

void TransactionGenerator::generateData(int transactionCount) {
	pqxx::work transaction{ mDatabaseConnection };

	CurrencyManager currencyManager;

	std::vector<std::string> customerData;
	getCustomerData(transaction, customerData);

	std::vector<Product> productData;
	getProductData(transaction, productData, currencyManager);

	std::vector<std::string> storeData;
	getStoreData(transaction, storeData);

	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());

	std::uniform_int_distribution<size_t> customerUniformDistribution(0, customerData.size() - 1);
	std::uniform_int_distribution<size_t> productUniformDistribution(0, productData.size() - 1);
	std::uniform_int_distribution<size_t> storeUniformDistribution(0, storeData.size() - 1);
	std::uniform_int_distribution<size_t> currencyUniformDistribution(0, currencyManager.getCurrencyCodesCount() - 1);
	std::uniform_int_distribution<int> quantityUniformDistribution(1, 20);

	auto stream = pqxx::stream_to::table(transaction, { "transactions" }, { "customer_id", "product_id", "store_id", "currency_code", "quantity", "total_amount" });

	for (int i = 0; i < transactionCount; i++) {
		const std::string& customerId = customerData[customerUniformDistribution(generator)];
		const Product& product = productData[productUniformDistribution(generator)];
		const std::string& storeId = storeData[storeUniformDistribution(generator)];
		const Currency currencyCode = currencyManager.getCurrencyByIndex(currencyUniformDistribution(generator));
		const int quantity = quantityUniformDistribution(generator);
		const std::int64_t totalAmount = currencyManager.convert(product.basePrice * quantity, product.currencyCode, currencyCode);

		stream << std::make_tuple(customerId, product.productId, storeId, currencyManager.toString(currencyCode), quantity, totalAmount);
	}

	stream.complete();
	transaction.commit();
}

void TransactionGenerator::getCustomerData(pqxx::work& transaction, std::vector<std::string>& customerData) {
	auto stream = pqxx::stream_from::table(transaction, { "customers" }, { "customer_id" });

	for (auto [customerId] : stream.iter<std::string>()) {
		customerData.emplace_back(customerId);
	}

	stream.complete();
}

void TransactionGenerator::getProductData(pqxx::work& transaction, std::vector<Product>& productData, CurrencyManager& currencyManager) {
	auto stream = pqxx::stream_from::table(transaction, { "products" }, { "product_id", "base_price", "currency_code" });

	for (auto [productId, basePrice, currencyCodeStr] : stream.iter<std::string, std::int64_t, std::string>()) {
		productData.emplace_back(std::move(productId), basePrice, currencyManager.fromString(currencyCodeStr));
	}

	stream.complete();
}

void TransactionGenerator::getStoreData(pqxx::work& transaction, std::vector<std::string>& storeData) {
	auto stream = pqxx::stream_from::table(transaction, { "stores" }, { "store_id" });

	for (auto [storeId] : stream.iter<std::string>()) {
		storeData.emplace_back(storeId);
	}

	stream.complete();
}