#include <pqxx/pqxx>

#include <iostream>

int main() {
	try {
		pqxx::connection databaseConnection{};



	} catch (std::exception& e) {
		std::cerr << "Database Error: " << e.what() << std::endl;
	}


	return 0;
}