#include "OandA_API.hpp"
#include "JsonStructureException.hpp"
#include <stdexcept>
#include <iostream>
#include <json/json.h>
#include <curl/curl.h>

OandA_API::OandA_API(const std::string& apiKey, const std::string& accountID)
        : apiKey(apiKey), accountID(accountID), baseURL("https://api-fxpractice.oanda.com/v3/") {}

size_t OandA_API::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Json::Value OandA_API::makeRequest(const std::string& endpoint) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = baseURL + endpoint;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        std::cout << "Request URL: " << url << std::endl;
        std::cout << "Authorization Header: " << "Bearer " + apiKey << std::endl;

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to make request: " + std::string(curl_easy_strerror(res)));
        }
    }

    Json::CharReaderBuilder readerBuilder;
    Json::Value jsonData;
    std::string errs;

    std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
    if (!reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &jsonData, &errs)) {
        throw std::runtime_error("Failed to parse JSON: " + errs);
    }

    return jsonData;
}

Json::Value OandA_API::getAccountDetails() {
    return makeRequest("accounts/" + accountID);
}

Json::Value OandA_API::getInstrumentPrices(const std::string& instrument) {
    return makeRequest("instruments/" + instrument + "/pricing");
}

Json::Value OandA_API::getHistoricalData(const std::string& instrument, const std::string& granularity, int count) {
    std::string endpoint = "instruments/" + instrument + "/candles?granularity=" + granularity + "&count=" + std::to_string(count) + "&price=M";

    Json::Value jsonData = makeRequest(endpoint);

    if (jsonData.isMember("errorMessage")) {
        std::cerr << "API Error: " << jsonData["errorMessage"].asString() << std::endl;
        throw std::runtime_error("API Error: " + jsonData["errorMessage"].asString());
    }

    if (!jsonData.isMember("candles") || !jsonData["candles"].isArray()) {
        std::cerr << "Invalid JSON structure: missing or incorrect 'candles' key" << std::endl;
        throw JsonStructureException("Invalid JSON structure");
    }

    return jsonData;
}