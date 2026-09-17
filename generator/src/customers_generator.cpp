#include "customers_generator.hpp"

#include <pqxx/pqxx>

#include <random>
#include <stdexcept>
#include <string>

CustomerGenerator::CustomerGenerator(int numberOfCustomers, pqxx::connection& databaseConnection) 
	: mCustomerCount(numberOfCustomers), mDatabaseConnection(databaseConnection) {}

void CustomerGenerator::generateData() const {
	if (mCountryCodes.size() <= 0) throw std::runtime_error("Country codes are empty");

	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());

	std::uniform_int_distribution<std::size_t> uniformDistribution(0, mCountryCodes.size() - 1);
	
	pqxx::work transaction(mDatabaseConnection);

	for (int i = 0; i < mCustomerCount; i++) {
		size_t randomCountryCodeIndex = uniformDistribution(generator);
		std::string randomCountryCode = mCountryCodes[randomCountryCodeIndex];

		transaction.exec_params0("INSERT INTO customers (country_code) VALUES ($1)", randomCountryCode);
	}

	transaction.commit();
}

void CustomerGenerator::deleteExistingData() const {
	pqxx::work transaction(mDatabaseConnection);

	transaction.exec0("DELETE FROM customers");

	transaction.commit();
}