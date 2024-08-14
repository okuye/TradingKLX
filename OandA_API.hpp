#pragma once
#include <string>
#include <json/json.h>

class OandA_API {
public:
    OandA_API(const std::string& apiKey, const std::string& accountID);

    Json::Value getAccountDetails();
    Json::Value getInstrumentPrices(const std::string& instrument);
//    Json::Value getHistoricalData(const std::string& instrument, const std::string& granularity, const std::string& from, const std::string& to, int count);
    Json::Value getHistoricalData(const std::string& instrument, const std::string& granularity, int count);

private:
    std::string apiKey;
    std::string accountID;
    std::string baseURL;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    Json::Value makeRequest(const std::string& endpoint);
};