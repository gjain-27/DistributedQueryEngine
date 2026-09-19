#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

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
private:
    std::unordered_map<Currency, double> mCurrencyMultipliers = {
        { Currency::GBP, 1.0 },
        { Currency::USD, 1.35 },
        { Currency::EUR, 1.16 },
        { Currency::JPY, 2.11 }
    };
};