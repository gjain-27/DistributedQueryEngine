#include <pqxx/pqxx>

#include <iostream>
#include <filesystem>

#include "env_reader.hpp"
#include "customers_generator.hpp"
#include "products_generator.hpp"

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

		CustomerGenerator customerGenerator{ databaseConnection };
		customerGenerator.deleteAllExistingData();
		customerGenerator.generateData(10000);

		std::cout << "Generated customers" << std::endl;

		ProductGenerator productGenerator{ databaseConnection };
		productGenerator.deleteAllExistingData();
		productGenerator.generateData(500);

		std::cout << "Generated products" << std::endl;

	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}


	return 0;
}