#include <string>
#include <vector>
#include <json/json.h>  // Include JsonCpp header
#include <curl/curl.h>

class OandA_API {
public:
    OandA_API(const std::string& apiKey, const std::string& accountID);

    Json::Value getAccountDetails();  // Use Json::Value instead of nlohmann::json
    Json::Value getInstrumentPrices(const std::string& instrument);  // Use Json::Value
    Json::Value getHistoricalData(const std::string& instrument, const std::string& granularity, const std::string& from, const std::string& to);  // Use Json::Value

private:
    std::string apiKey;
    std::string accountID;
    std::string baseURL;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    Json::Value makeRequest(const std::string& endpoint);  // Use Json::Value
};