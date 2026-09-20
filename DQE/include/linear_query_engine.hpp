#pragma once

#include <pqxx/pqxx>

#include <string>
#include <vector>
#include <cstdint>

#include "currency_manager.hpp"

struct CountryMonthResult {
	std::string countryCode;
	int month;
	std::int64_t totalRevenue;
	std::int64_t transactionCount;

	CountryMonthResult(std::string countryCode, int month, std::int64_t totalRevenue, std::int64_t transactionCount) :
		countryCode(std::move(countryCode)), month(month), totalRevenue(totalRevenue), transactionCount(transactionCount) {}
};

class LinearQueryEngine {
public:
	LinearQueryEngine(pqxx::connection& databaseConnection);

	std::vector<CountryMonthResult> runCountryMonthReport();
private:
	pqxx::connection& mDatabaseConnection;
	CurrencyManager mCurrencyManager;

	std::vector<std::string> getDistinctCountryCodes(pqxx::work& transaction);
};