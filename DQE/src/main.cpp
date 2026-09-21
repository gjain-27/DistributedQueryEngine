#include <pqxx/pqxx>

#include <iostream>
#include <string>

#include "env_reader.hpp"
#include "linear_query_engine.hpp"
#include "threaded_query_engine.hpp"
#include "distributed_query_engine.hpp"

void printResults(const std::vector<CountryMonthResult>& results) {
	for (const CountryMonthResult& result : results) {
		std::cout << result.countryCode << " month " << result.month << " | revenue: " << result.totalRevenue << " GBP | transactions: " << result.transactionCount << std::endl;
	}
}

int main(int argc, char* argv[]) {
	try {
		EnvReader envReader{ std::string(DQE_SOURCE_DIR) + "/.env" };

		std::string	connectionString = "host=" + envReader["DB_HOST"] + " port=" + envReader["DB_PORT"] + " dbname=" + envReader["DB_NAME"] + " user=" + envReader["DB_USER"] + " password=" + envReader["DB_PASSWORD"];

		if (argc == 3) {
			size_t workerIndex = std::stoul(argv[1]);
			size_t workerCount = std::stoul(argv[2]);

			std::cout << "Executing queries as distributed worker " << workerIndex << " of " << workerCount << "..." << std::endl;
			std::cout << std::endl;

			DistributedQueryEngine distributedQueryEngine{ connectionString, workerIndex, workerCount };
			std::vector<CountryMonthResult> distributedResults = distributedQueryEngine.runCountryMonthReport();

			printResults(distributedResults);

			return 0;
		}

		if (argc != 1) {
			std::cerr << "Usage: " << argv[0] << " [workerIndex workerCount]" << std::endl;
			return 1;
		}

		pqxx::connection databaseConnection{ connectionString };

		if (databaseConnection.is_open()) {
			std::cout << "Connected to database successfully" << std::endl;
		} else {
			std::cerr << "Failed to open a connection with the database" << std::endl;
			return 1;
		}

		std::cout << std::endl;
		std::cout << "Executing queries linearly..." << std::endl;
		std::cout << std::endl;

		LinearQueryEngine linearQueryEngine{ databaseConnection };
		std::vector<CountryMonthResult> linearResults = linearQueryEngine.runCountryMonthReport();

		printResults(linearResults);

		std::cout << std::endl;
		std::cout << "Executing queries with a multi-threaded approach..." << std::endl;
		std::cout << std::endl;

		ThreadedQueryEngine threadedQueryEngine{ connectionString };
		std::vector<CountryMonthResult> threadedResults = threadedQueryEngine.runCountryMonthReport();

		printResults(threadedResults);
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}