#ifndef ALPHA_VANTAGE_API_H
#define ALPHA_VANTAGE_API_H

#include <string>

std::string fetchDataFromAlphaVantage(const std::string& apiKey, const std::string& function, const std::string& from_symbol, const std::string& to_symbol);

#endif // ALPHA_VANTAGE_API_H
