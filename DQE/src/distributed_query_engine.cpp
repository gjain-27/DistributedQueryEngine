#include "distributed_query_engine.hpp"
#include "thread_pool.hpp"

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <future>
#include <stdexcept>

DistributedQueryEngine::DistributedQueryEngine(const std::string& connectionString, size_t workerIndex, size_t workerCount, size_t threadCount)
	: mConnectionString(connectionString), mWorkerIndex(workerIndex), mWorkerCount(workerCount) {
	if (mWorkerCount == 0) throw std::invalid_argument("workerCount must be at least 1");
	if (mWorkerIndex >= mWorkerCount) throw std::invalid_argument("workerIndex must be less than workerCount");

	if (threadCount == 0) {
		threadCount = std::thread::hardware_concurrency();
	}

	if (threadCount == 0) {
		threadCount = 4;
	}

	mThreadCount = threadCount;
}

std::vector<std::pair<std::string, std::int64_t>> DistributedQueryEngine::getCountryWorkloads() {
	pqxx::connection connection{ mConnectionString };
	pqxx::work transaction{ connection };

	std::vector<std::pair<std::string, std::int64_t>> workloads;

	auto stream = pqxx::stream_from::query(transaction,
		"SELECT country_code, COUNT(*) AS store_count FROM stores "
		"GROUP BY country_code ORDER BY store_count DESC, country_code"
	);

	for (auto [countryCode, storeCount] : stream.iter<std::string, std::int64_t>()) {
		workloads.emplace_back(std::move(countryCode), storeCount);
	}

	stream.complete();
	transaction.commit();

	return workloads;
}

std::vector<std::string> DistributedQueryEngine::selectWorkerCountryCodes(const std::vector<std::pair<std::string, std::int64_t>>& workloads) const {
	std::vector<std::int64_t> workerLoads(mWorkerCount, 0);
	std::vector<std::string> selected;

	for (const auto& [countryCode, storeCount] : workloads) {
		size_t leastLoadedWorker = 0;

		for (size_t i = 1; i < mWorkerCount; i++) {
			if (workerLoads[i] < workerLoads[leastLoadedWorker]) {
				leastLoadedWorker = i;
			}
		}

		workerLoads[leastLoadedWorker] += storeCount;

		if (leastLoadedWorker == mWorkerIndex) {
			selected.push_back(countryCode);
		}
	}

	return selected;
}

std::vector<CountryMonthResult> DistributedQueryEngine::queryCountry(const std::string& countryCode) {
	pqxx::connection connection{ mConnectionString };
	pqxx::work transaction{ connection };

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
	transaction.commit();

	std::vector<CountryMonthResult> countryResults;

	for (const auto& [month, revenue] : revenueByMonth) {
		countryResults.emplace_back(countryCode, month, revenue, countByMonth[month]);
	}

	return countryResults;
}

std::vector<CountryMonthResult> DistributedQueryEngine::runCountryMonthReport() {
	std::vector<CountryMonthResult> results;

	std::vector<std::string> countryCodes = selectWorkerCountryCodes(getCountryWorkloads());

	auto startTime = std::chrono::high_resolution_clock::now();

	ThreadPool pool(mThreadCount);
	std::vector<std::future<std::vector<CountryMonthResult>>> futures;

	for (const std::string& countryCode : countryCodes) {
		futures.push_back(pool.enqueue([this, countryCode]() {
			return queryCountry(countryCode);
			}));
	}

	for (auto& future : futures) {
		std::vector<CountryMonthResult> countryResults = future.get();
		results.insert(results.end(), countryResults.begin(), countryResults.end());
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	std::cout << "Distributed query engine (worker " << mWorkerIndex << "/" << mWorkerCount << ", " << mThreadCount << " threads): " << countryCodes.size() << " queries run in " << durationMs << " ms" << std::endl;

	return results;
}