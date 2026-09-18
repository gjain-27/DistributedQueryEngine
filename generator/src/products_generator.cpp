#include "products_generator.hpp"

#include <pqxx/pqxx>

#include <random>
#include <stdexcept>
#include <string>
#include <tuple>

ProductGenerator::ProductGenerator(pqxx::connection& databaseConnection)
	: mDatabaseConnection(databaseConnection) {}

void ProductGenerator::generateData(int productCount) {
	if (mCurrencyCodes.empty()) throw std::runtime_error("Currency codes are empty");
	if (mCategories.empty()) throw std::runtime_error("Categories are empty");

	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());

	std::uniform_int_distribution<size_t> currencyCodeUniformDistribution(0, mCurrencyCodes.size() - 1);
	std::uniform_int_distribution<size_t> categoryUniformDistribution(0, mCategories.size() - 1);

	pqxx::work transaction{ mDatabaseConnection };
	auto stream = pqxx::stream_to::table(transaction, { "products" }, { "category", "base_price", "currency_code" });

	for (int i = 0; i < productCount; i++) {
		const std::string& category = mCategories[categoryUniformDistribution(generator)];
		const std::string& currencyCode = mCurrencyCodes[currencyCodeUniformDistribution(generator)];

		auto [minPrice, maxPrice] = mCategoryPriceRanges.at(category);
		std::uniform_int_distribution<std::int64_t> priceDistribution(minPrice, maxPrice);
		std::int64_t basePriceGBP = priceDistribution(generator);

		double multiplier = mCurrencyMultipliers.at(currencyCode);
		std::int64_t basePrice = static_cast<std::int64_t>(basePriceGBP * multiplier);

		stream << std::make_tuple(category, basePrice, currencyCode);
	}

	stream.complete();
	transaction.commit();
}