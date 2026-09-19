#include "currency_manager.hpp"

#include <stdexcept>

std::int64_t CurrencyManager::convert(std::int64_t amount, Currency from, Currency to) const {
	double fromRate = getMultiplier(from);
	double toRate = getMultiplier(to);

	double amountInGBP = amount / fromRate;
	double converted = amountInGBP * toRate;

	return static_cast<std::int64_t>(converted);
}

double CurrencyManager::getMultiplier(Currency currency) const {
	switch (currency) {
		case Currency::GBP: return 1.0;
		case Currency::USD: return 1.35;
		case Currency::EUR: return 1.16;
		case Currency::JPY: return 2.11;
	}

	throw std::runtime_error("Unknown currency");
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

Currency CurrencyManager::fromString(const std::string& code) const {
	return mStringToCurrency.at(code);
}

Currency CurrencyManager::getCurrencyByIndex(size_t index) const {
	return mCurrencies.at(index);
}

size_t CurrencyManager::getCurrencyCodesCount() const {
	return mCurrencies.size();
}