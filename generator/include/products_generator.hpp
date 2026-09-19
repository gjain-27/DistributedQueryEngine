#pragma once

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <unordered_map>

#include "currency_manager.hpp"

class ProductGenerator {
public:
	ProductGenerator(pqxx::connection& databaseConnection);

	void generateData(int productCount);
private:
	pqxx::connection& mDatabaseConnection;

	std::vector<std::string> mCategories = {"Electronics", "Clothing", "Food", "Furniture", "Sports", "Books", "Beauty", "Toys"};

	std::unordered_map<std::string, std::pair<std::int64_t, std::int64_t>> mCategoryPriceRanges = {
		{"Electronics", {1500, 250000}},
		{"Clothing", {500, 15000}},
		{"Food", {50, 3000}},
		{"Furniture", {2000, 300000}},
		{"Sports", {500,50000}},
		{"Books", {300, 3000}},
		{"Beauty", {300, 8000}},
		{"Toys", {300, 15000}}
	};
};