#pragma once
#include <string>
#include <json/json.h>

struct InstrumentData {
    std::string name;
    std::string type;
    std::string displayName;
    int pipLocation;
    double marginRate;
};

class OandA_API {
public:
    OandA_API(const std::string& apiKey, const std::string& accountID);

    Json::Value getAccountDetails();
    Json::Value getInstrumentPrices(const std::string& instrument);
    Json::Value getHistoricalData(const std::string& instrument, const std::string& granularity, int count);
    Json::Value getOrderBook(const std::string& instrument, const std::string& time = "");
    Json::Value getPositionBook(const std::string& instrument, const std::string& time = "");
    Json::Value getHistoricalDataByDateRange(const std::string& instrument, const std::string& granularity, const std::string& from, const std::string& to);
    std::pair<int, Json::Value> fetchInstruments();
    std::pair<int, Json::Value> fetchCandles(const std::string& pair_name, int count, const std::string& granularity);
    std::vector<InstrumentData> getInstrumentsData();
    void saveInstruments();
    void plotCandlestick(const std::string& pair, const std::string& granularity);
    std::string get_his_data_filename(const std::string& pair, const std::string& granularity); // Add this line

private:
    std::string apiKey;
    std::string accountID;
    std::string baseURL;

    struct curl_slist* secureHeader;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    Json::Value makeRequest(const std::string& endpoint);
};