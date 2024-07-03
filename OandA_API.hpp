#ifndef OANDA_API_HPP
#define OANDA_API_HPP

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp> // Updated to use nlohmann/json

class OandA_API {
public:
    OandA_API(const std::string& api_key);
    std::vector<double> getPrices(const std::string& instrument);
private:
    std::string api_key;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif // OANDA_API_HPP
