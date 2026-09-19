#include <pqxx/pqxx>

#include <iostream>
#include <chrono>

#include "env_reader.hpp"
#include "customers_generator.hpp"
#include "products_generator.hpp"
#include "stores_generator.hpp"
#include "transactions_generator.hpp"

void deleteAllExistingDataFromDatabase(pqxx::connection& databaseConnection);

int main() {
	try {
		EnvReader envReader{ std::string(DQE_SOURCE_DIR) + "/.env" };

		std::string	connectionString = "host=" + envReader["DB_HOST"] + " port=" + envReader["DB_PORT"] + " dbname=" + envReader["DB_NAME"] + " user=" + envReader["DB_USER"] + " password=" + envReader["DB_PASSWORD"];
		pqxx::connection databaseConnection{ connectionString };

		if (databaseConnection.is_open()) {
			std::cout << "Connected to database successfully" << std::endl;
		} else {
			std::cerr << "Failed to open a connection with the database" << std::endl;
			return 1;
		}

		auto start = std::chrono::steady_clock::now();
		deleteAllExistingDataFromDatabase(databaseConnection);
		auto end = std::chrono::steady_clock::now();
		std::cout << "Database cleared, time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

		start = std::chrono::steady_clock::now();
		CustomerGenerator customerGenerator{ databaseConnection };
		customerGenerator.generateData(10000);
		end = std::chrono::steady_clock::now();

		std::cout << "Generated customers, time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

		start = std::chrono::steady_clock::now();
		ProductGenerator productGenerator{ databaseConnection };
		productGenerator.generateData(500);
		end = std::chrono::steady_clock::now();

		std::cout << "Generated products, time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

		start = std::chrono::steady_clock::now();
		StoreGenerator storeGenerator{ databaseConnection };
		storeGenerator.generateData(10);
		end = std::chrono::steady_clock::now();

		std::cout << "Generated stores, time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

		start = std::chrono::steady_clock::now();
		TransactionGenerator transactionGenerator{ databaseConnection };
		transactionGenerator.generateData(1000000);
		end = std::chrono::steady_clock::now();

		std::cout << "Generated transactions, time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void deleteAllExistingDataFromDatabase(pqxx::connection& databaseConnection) {
	pqxx::work transaction{ databaseConnection };

	transaction.exec0("TRUNCATE TABLE customers, products, stores, transactions CASCADE");

	transaction.commit();
}