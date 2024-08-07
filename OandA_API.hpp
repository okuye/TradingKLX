#include <string>
#include <vector>
#include "third_party/nlohmann_json/json.hpp"
#include <curl/curl.h>

class OandA_API {
public:
    OandA_API(const std::string& apiKey, const std::string& accountID);

    nlohmann::json getAccountDetails();
    nlohmann::json getInstrumentPrices(const std::string& instrument);
    nlohmann::json getHistoricalData(const std::string& instrument, const std::string& granularity, const std::string& from, const std::string& to);

private:
    std::string apiKey;
    std::string accountID;
    std::string baseURL;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    nlohmann::json makeRequest(const std::string& endpoint);
};
