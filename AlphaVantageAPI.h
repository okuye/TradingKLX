#ifndef ALPHA_VANTAGE_API_H
#define ALPHA_VANTAGE_API_H

#include <string>

class AlphaVantageAPI {
public:
    AlphaVantageAPI(const std::string& apiKey);
    std::string fetchData(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval = "5min");

private:
    std::string apiKey;
    std::string buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval);
};

#endif // ALPHA_VANTAGE_API_H
