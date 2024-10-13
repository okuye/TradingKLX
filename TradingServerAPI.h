#ifndef TRADINGSERVER_API_H
#define TRADINGSERVER_API_H

#include <curl/curl.h>
#include <json/json.h>
#include <string>

class TradingServerAPI {
public:
    TradingServerAPI(const std::string& baseURL);
    Json::Value fetchTrades(const std::string& startDate, const std::string& endDate, const std::string& symbol, const std::string& apiKey);

private:
    std::string baseURL;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif // TRADINGSERVER_API_H
