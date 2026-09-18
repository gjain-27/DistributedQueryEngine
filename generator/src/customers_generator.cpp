#include "customers_generator.hpp"

#include <pqxx/pqxx>

#include <random>
#include <stdexcept>
#include <string>

CustomerGenerator::CustomerGenerator(pqxx::connection& databaseConnection) 
	: mDatabaseConnection(databaseConnection) {}

void CustomerGenerator::generateData(int customerCount) {
	if (mCountryCodes.size() <= 0) throw std::runtime_error("Country codes are empty");

	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());

	std::uniform_int_distribution<std::size_t> uniformDistribution(0, mCountryCodes.size() - 1);
	
	pqxx::work transaction{mDatabaseConnection};
	auto stream = pqxx::stream_to::table(transaction, {"customers"}, {"country_code"});

	for (int i = 0; i < customerCount; i++) {
		size_t randomCountryCodeIndex = uniformDistribution(generator);
		const std::string& randomCountryCode = mCountryCodes[randomCountryCodeIndex];

		stream << std::make_tuple(randomCountryCode);
	}

	stream.complete();
	transaction.commit();
}

void CustomerGenerator::deleteAllExistingData() {
	pqxx::work transaction(mDatabaseConnection);

	transaction.exec0("DELETE FROM customers");

	transaction.commit();
}