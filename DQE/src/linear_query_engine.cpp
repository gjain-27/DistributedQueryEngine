#include "linear_query_engine.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>

LinearQueryEngine::LinearQueryEngine(pqxx::connection& databaseConnection)
	: mDatabaseConnection(databaseConnection) {}

std::vector<CountryMonthResult> LinearQueryEngine::runCountryMonthReport() {
	std::vector<CountryMonthResult> result;

	pqxx::work transaction{ mDatabaseConnection };

	std::vector<std::string> countryCodes = getDistinctCountryCodes(transaction);

	auto startTime = std::chrono::high_resolution_clock::now();

	for (const std::string& countryCode : countryCodes) {
		auto stream = pqxx::stream_from::query(transaction,
			"SELECT EXTRACT(MONTH FROM t.occurred_at)::int AS month, "
			"t.currency_code, "
			"SUM(t.total_amount) AS total_amount, COUNT(*) AS transaction_count "
			"FROM transactions t "
			"JOIN stores s ON t.store_id = s.store_id "
			"WHERE s.country_code = '" + countryCode + "' "
			"GROUP BY month, t.currency_code"
		);

		std::unordered_map<int, std::int64_t> revenueByMonth;
		std::unordered_map<int, std::int64_t> countByMonth;

		for (auto [month, currencyCodeStr, totalAmount, transactionCount] : stream.iter<int, std::string, std::int64_t, std::int64_t>()) {
			Currency currency = mCurrencyManager.fromString(currencyCodeStr);
			std::int64_t amountInGBP = mCurrencyManager.convert(totalAmount, currency, Currency::GBP);

			revenueByMonth[month] += amountInGBP;
			countByMonth[month] += transactionCount;
		}

		stream.complete();

		for (const auto& [month, revenue] : revenueByMonth) {
			result.emplace_back(countryCode, month, revenue, countByMonth[month]);
		}
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	std::cout << "Linear query engine: " << countryCodes.size() << " queries run in " << durationMs << " ms" << std::endl;

	transaction.commit();

	return result;
}

std::vector<std::string> LinearQueryEngine::getDistinctCountryCodes(pqxx::work& transaction) {
	std::vector<std::string> countryCodes;

	auto stream = pqxx::stream_from::query(transaction, "SELECT DISTINCT country_code FROM stores");

	for (auto [countryCode] : stream.iter<std::string>()) {
		countryCodes.push_back(std::move(countryCode));
	}

	stream.complete();

	return countryCodes;
}