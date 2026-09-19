#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class Currency {
    GBP,
    USD,
    EUR,
    JPY
};

class CurrencyManager {
public:
    CurrencyManager() = default;

    std::int64_t convert(std::int64_t amount, Currency from, Currency to) const;
    std::string toString(Currency currency) const;
    Currency fromString(const std::string& code) const;
    Currency getCurrencyByIndex(size_t index) const;

    size_t getCurrencyCodesCount() const;
private:
    double getMultiplier(Currency currency) const;

    std::vector<Currency> mCurrencies = { Currency::GBP, Currency::USD, Currency::EUR, Currency::JPY };

    std::unordered_map<std::string, Currency> mStringToCurrency = {
        { "GBP", Currency::GBP },
        { "USD", Currency::USD },
        { "EUR", Currency::EUR },
        { "JPY", Currency::JPY }
    };
};