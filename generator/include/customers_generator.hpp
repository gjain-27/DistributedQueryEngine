#pragma once

#include <pqxx/pqxx>

#include <vector>

class CustomerGenerator {
public:
	CustomerGenerator(int numberOfCustomers, pqxx::connection& databaseconnection);

	void generateData() const;
	void deleteExistingData() const;
private:
	int mCustomerCount;
	pqxx::connection& mDatabaseConnection;

	std::vector<std::string> mCountryCodes = {"GBR", "USA", "CAN", "AUS", "DEU", "FRA", "ESP", "ITA", "NLD", "BEL", "SWE", "NOR", "DNK", "FIN", "IRL", "PRT", "POL", "AUT", "CHE", "JPN", "KOR", "CHN", "IND", "SGP", "BRA", "MEX", "ARG", "ZAF", "ARE", "NZL"};
};