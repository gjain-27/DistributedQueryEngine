#pragma once

#include <pqxx/pqxx>

#include <string>
#include <vector>
#include <cstdint>
#include <thread>

#include "currency_manager.hpp"
#include "linear_query_engine.hpp"

class ThreadedQueryEngine {
public:
	ThreadedQueryEngine(const std::string& connectionString, size_t threadCount = 0);

	std::vector<CountryMonthResult> runCountryMonthReport();

private:
	std::string mConnectionString;
	size_t mThreadCount;
	CurrencyManager mCurrencyManager;

	std::vector<std::string> getDistinctCountryCodes();

	std::vector<CountryMonthResult> queryCountry(const std::string& countryCode);
};