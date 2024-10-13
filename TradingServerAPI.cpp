#include "TradingServerAPI.h"
#include <iostream>
#include <stdexcept>

TradingServerAPI::TradingServerAPI(const std::string& baseURL) : baseURL(baseURL) {}

size_t TradingServerAPI::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Json::Value TradingServerAPI::fetchTrades(const std::string& startDate, const std::string& endDate, const std::string& symbol, const std::string& apiKey) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string url = baseURL + "?startDate=" + startDate + "&endDate=" + endDate + "&symbol=" + symbol + "&api_key=" + apiKey;



    std::cout << "Fetching URL: " << url << std::endl;  // Log the correct URL for debugging

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Set timeout for the connection and operation
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to fetch trades: " + std::string(curl_easy_strerror(res)));
        }
    } else {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Log the raw response for debugging
    std::cout << "Raw response: " << readBuffer << std::endl;

    if (readBuffer.empty()) {
        throw std::runtime_error("Received empty response from TradingServerAPI");
    }

    // JSON Parsing
    Json::CharReaderBuilder readerBuilder;
    Json::Value jsonData;
    std::string errs;

    std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
    if (!reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &jsonData, &errs)) {
        throw std::runtime_error("Failed to parse JSON: " + errs);
    }

    return jsonData;
}
