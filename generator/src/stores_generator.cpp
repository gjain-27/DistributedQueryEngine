#include "stores_generator.hpp"

#include <pqxx/pqxx>

#include <random>
#include <stdexcept>
#include <string>
#include <tuple>

StoreGenerator::StoreGenerator(pqxx::connection& databaseConnection)
	: mDatabaseConnection(databaseConnection) {};

void StoreGenerator::generateData(int storeCount) {
	if (mCountryCodes.empty()) throw std::runtime_error("Country codes are empty");

	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());

	std::uniform_int_distribution<std::size_t> uniformDistribution(0, mCountryCodes.size() - 1);

	pqxx::work transaction(mDatabaseConnection);
	auto stream = pqxx::stream_to::table(transaction, {"stores"}, {"country_code"});

	for (int i = 0; i < storeCount; i++) {
		std::size_t countryCodeIndex = uniformDistribution(generator);
		const std::string& countryCode = mCountryCodes[countryCodeIndex];

		stream << std::make_tuple(countryCode);
	}

	stream.complete();
	transaction.commit();
}