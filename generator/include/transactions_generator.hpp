#pragma once

#include <pqxx/pqxx>

#include <string>
#include <vector>

struct Product;

class TransactionGenerator {
public:
	TransactionGenerator(pqxx::connection& databaseConnection);

	void generateData(int transactionCount);
private:
	pqxx::connection& mDatabaseConnection;

	void getCustomerData(pqxx::work& transaction, std::vector<std::string>& customerData);
	void getProductData(pqxx::work& transaction, std::vector<Product>& productData);
	void getStoreData(pqxx::work& transaction, std::vector<std::string>& storeData);
};