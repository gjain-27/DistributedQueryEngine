#include "currency_manager.hpp"

#include <stdexcept>

std::int64_t CurrencyManager::convert(std::int64_t amount, Currency from, Currency to) const {
	double fromRate = mCurrencyMultipliers.at(from);
	double toRate = mCurrencyMultipliers.at(to);

	double amountInGBP = amount / fromRate;
	double converted = amountInGBP * toRate;

	return static_cast<std::int64_t>(converted);
}

std::string CurrencyManager::toString(Currency currency) const {
	switch (currency) {
		case Currency::GBP: return "GBP";
		case Currency::USD: return "USD";
		case Currency::EUR: return "EUR";
		case Currency::JPY: return "JPY";
	}

	throw std::runtime_error("Unknown currency");
}