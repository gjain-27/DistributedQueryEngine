#include "transactions_generator.hpp"

#include <string>
#include <vector>
#include <tuple>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "currency_manager.hpp"

struct Product {
	std::string productId;
	std::int64_t basePrice;
	Currency currencyCode;

	Product(std::string productId, std::int64_t basePrice, Currency currencyCode) :
		productId(std::move(productId)), basePrice(basePrice), currencyCode(currencyCode) {}
};

TransactionGenerator::TransactionGenerator(pqxx::connection& databaseConnection)
	: mDatabaseConnection(databaseConnection) {}

std::string formatTimestamp(std::chrono::system_clock::time_point timePoint) {
	std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
	std::tm utcTime{};
	gmtime_s(&utcTime, &time);

	std::ostringstream oss;
	oss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
	return oss.str();
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

	auto now = std::chrono::system_clock::now();
	auto twoYearsAgo = now - std::chrono::hours(24 * 365 * 2);

	std::int64_t startEpochSeconds = std::chrono::duration_cast<std::chrono::seconds>(twoYearsAgo.time_since_epoch()).count();
	std::int64_t endEpochSeconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

	std::uniform_int_distribution<std::int64_t> timestampUniformDistribution(startEpochSeconds, endEpochSeconds);

	auto stream = pqxx::stream_to::table(transaction, { "transactions" }, { "customer_id", "product_id", "store_id", "currency_code", "quantity", "total_amount", "occurred_at" });

	for (int i = 0; i < transactionCount; i++) {
		const std::string& customerId = customerData[customerUniformDistribution(generator)];
		const Product& product = productData[productUniformDistribution(generator)];
		const std::string& storeId = storeData[storeUniformDistribution(generator)];
		const Currency currencyCode = currencyManager.getCurrencyByIndex(currencyUniformDistribution(generator));
		const int quantity = quantityUniformDistribution(generator);
		const std::int64_t totalAmount = currencyManager.convert(product.basePrice * quantity, product.currencyCode, currencyCode);

		std::int64_t randomEpochSeconds = timestampUniformDistribution(generator);
		auto randomTimePoint = std::chrono::system_clock::time_point(std::chrono::seconds(randomEpochSeconds));
		std::string occurredAt = formatTimestamp(randomTimePoint);

		stream << std::make_tuple(customerId, product.productId, storeId, currencyManager.toString(currencyCode), quantity, totalAmount, occurredAt);
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