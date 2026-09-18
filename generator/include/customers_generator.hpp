#pragma once

#include <pqxx/pqxx>

#include <string>
#include <vector>

class CustomerGenerator {
public:
	CustomerGenerator(pqxx::connection& databaseconnection);

	void generateData(int customerCount);
	void deleteAllExistingData();
private:
	pqxx::connection& mDatabaseConnection;

	std::vector<std::string> mCountryCodes = {"GBR", "USA", "CAN", "AUS", "DEU", "FRA", "ESP", "ITA", "NLD", "BEL", "SWE", "NOR", "DNK", "FIN", "IRL", "PRT", "POL", "AUT", "CHE", "JPN", "KOR", "CHN", "IND", "SGP", "BRA", "MEX", "ARG", "ZAF", "ARE", "NZL"};
};