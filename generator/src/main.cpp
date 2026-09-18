#include <pqxx/pqxx>

#include <iostream>

#include "env_reader.hpp"
#include "customers_generator.hpp"
#include "products_generator.hpp"
#include "stores_generator.hpp"

void deleteAllExistingDataFromDatabase(pqxx::connection& databaseConnection);

int main() {
	try {
		EnvReader envReader{std::string(DQE_SOURCE_DIR) + "/.env" };

		std::string	connectionString = "host=" + envReader["DB_HOST"] + " port=" + envReader["DB_PORT"] + " dbname=" + envReader["DB_NAME"] + " user=" + envReader["DB_USER"] + " password=" + envReader["DB_PASSWORD"];
		pqxx::connection databaseConnection{ connectionString };

		if (databaseConnection.is_open()) {
			std::cout << "Connected to database successfully" << std::endl;
		} else {
			std::cerr << "Failed to open a connection with the database" << std::endl;
			return 1;
		}

		deleteAllExistingDataFromDatabase(databaseConnection);
		std::cout << "Database cleared" << std::endl;

		CustomerGenerator customerGenerator{ databaseConnection };
		customerGenerator.generateData(10000);

		std::cout << "Generated customers" << std::endl;

		ProductGenerator productGenerator{ databaseConnection };
		productGenerator.generateData(500);

		std::cout << "Generated products" << std::endl;

		StoreGenerator storeGenerator{ databaseConnection };
		storeGenerator.generateData(10);

		std::cout << "Generated stores" << std::endl;

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