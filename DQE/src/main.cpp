#include <pqxx/pqxx>

#include <iostream>

#include "env_reader.hpp"
#include "linear_query_engine.hpp"
#include "threaded_query_engine.hpp"

int main() {
	try {
		EnvReader envReader{ std::string(DQE_SOURCE_DIR) + "/.env" };

		std::string	connectionString = "host=" + envReader["DB_HOST"] + " port=" + envReader["DB_PORT"] + " dbname=" + envReader["DB_NAME"] + " user=" + envReader["DB_USER"] + " password=" + envReader["DB_PASSWORD"];
		pqxx::connection databaseConnection{ connectionString };

		if (databaseConnection.is_open()) {
			std::cout << "Connected to database successfully" << std::endl;
		}
		else {
			std::cerr << "Failed to open a connection with the database" << std::endl;
			return 1;
		}

		std::cout << std::endl;
		std::cout << "Executing queries linearly..." << std::endl;
		std::cout << std::endl;

		LinearQueryEngine linearQueryEngine{ databaseConnection };
		std::vector<CountryMonthResult> linearResults = linearQueryEngine.runCountryMonthReport();

		for (const CountryMonthResult& result : linearResults) {
			std::cout << result.countryCode << " month " << result.month << " | revenue: " << result.totalRevenue << " GBP | transactions: " << result.transactionCount << std::endl;
		}

		std::cout << std::endl;
		std::cout << "Executing queries with a multi-threaded approach..." << std::endl;
		std::cout << std::endl;

		ThreadedQueryEngine threadedQueryEngine{ connectionString };
		std::vector<CountryMonthResult> threadedResults = threadedQueryEngine.runCountryMonthReport();

		for (const CountryMonthResult& result : threadedResults) {
			std::cout << result.countryCode << " month " << result.month << " | revenue: " << result.totalRevenue << " GBP | transactions: " << result.transactionCount << std::endl;
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}