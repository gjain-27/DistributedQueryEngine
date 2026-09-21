#pragma once

#include <pqxx/pqxx>

#include <string>
#include <vector>
#include <cstdint>
#include <thread>
#include <utility>

#include "currency_manager.hpp"
#include "linear_query_engine.hpp"

class DistributedQueryEngine {
public:
	DistributedQueryEngine(const std::string& connectionString, size_t workerIndex, size_t workerCount, size_t threadCount = 0);

	std::vector<CountryMonthResult> runCountryMonthReport();

private:
	std::string mConnectionString;
	size_t mWorkerIndex;
	size_t mWorkerCount;
	size_t mThreadCount;
	CurrencyManager mCurrencyManager;

	std::vector<std::pair<std::string, std::int64_t>> getCountryWorkloads();
	std::vector<std::string> selectWorkerCountryCodes(const std::vector<std::pair<std::string, std::int64_t>>& workloads) const;

	std::vector<CountryMonthResult> queryCountry(const std::string& countryCode);
};