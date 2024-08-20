#include "OandA_API.hpp"
#include "JsonStructureException.hpp"
#include <stdexcept>
#include <iostream>
#include <json/json.h>
#include <curl/curl.h>
#include <fstream>
#include <vector>
#include <string>
#include "matplotlibcpp.h"
#include <sstream>
#include <algorithm>

namespace plt = matplotlibcpp;

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
    std::string endpoint = "instruments/" + instrument + "/candles?granularity=" + granularity + "&count=" + std::to_string(count) + "&price=MBA";

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

Json::Value OandA_API::getOrderBook(const std::string& instrument, const std::string& snapshotTime) {
    std::string endpoint = "instruments/" + instrument + "/orderBook";
    if (!snapshotTime.empty()) {
        endpoint += "?time=" + snapshotTime;
    }

    Json::Value jsonData = makeRequest(endpoint);

    if (jsonData.isMember("errorMessage")) {
        std::cerr << "API Error: " << jsonData["errorMessage"].asString() << std::endl;
        throw std::runtime_error("API Error: " + jsonData["errorMessage"].asString());
    }

    return jsonData;
}

Json::Value OandA_API::getPositionBook(const std::string& instrument, const std::string& time) {
    std::string endpoint = "instruments/" + instrument + "/positionBook";
    if (!time.empty()) {
        endpoint += "?time=" + time;
    }

    Json::Value jsonData = makeRequest(endpoint);

    if (jsonData.isMember("errorMessage")) {
        std::cerr << "API Error: " << jsonData["errorMessage"].asString() << std::endl;
        throw std::runtime_error("API Error: " + jsonData["errorMessage"].asString());
    }

    return jsonData;
}

Json::Value OandA_API::getHistoricalDataByDateRange(const std::string& instrument, const std::string& granularity, const std::string& from, const std::string& to) {
    std::string endpoint = "instruments/" + instrument + "/candles?granularity=" + granularity + "&from=" + from + "&to=" + to;

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

std::pair<int, Json::Value> OandA_API::fetchInstruments() {
    std::string url = baseURL + "/accounts/" + accountID + "/instruments";

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, secureHeader);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_easy_cleanup(curl);

        Json::Value jsonResponse;
        Json::Reader reader;
        reader.parse(response, jsonResponse);

        return std::make_pair(static_cast<int>(http_code), jsonResponse);
    }

    return std::make_pair(0, Json::Value());
}

std::pair<int, Json::Value> OandA_API::fetchCandles(const std::string& pair_name, int count, const std::string& granularity) {
    std::string endpoint = "instruments/" + pair_name + "/candles";

    std::string params = "count=" + std::to_string(count) +
                         "&granularity=" + granularity +
                         "&price=MBA";

    std::string url = baseURL + endpoint + "?" + params;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        Json::Value jsonResponse;
        Json::Reader reader;
        reader.parse(response, jsonResponse);

        return std::make_pair(static_cast<int>(http_code), jsonResponse);
    }

    return std::make_pair(0, Json::Value());
}

std::vector<InstrumentData> OandA_API::getInstrumentsData() {
    auto [code, data] = this->fetchInstruments();
    std::vector<InstrumentData> result;

    if (code == 200) {
        const Json::Value& instruments = data["instruments"];
        for (const auto& instrument : instruments) {
            InstrumentData instrumentData;
            instrumentData.name = instrument["name"].asString();
            instrumentData.type = instrument["type"].asString();
            instrumentData.displayName = instrument["displayName"].asString();
            instrumentData.pipLocation = instrument["pipLocation"].asInt();
            instrumentData.marginRate = instrument["marginRate"].asDouble();
            result.push_back(instrumentData);
        }
    }

    return result;
}

void OandA_API::saveInstruments() {
    std::vector<InstrumentData> instruments = this->getInstrumentsData();
    if (!instruments.empty()) {
        std::ofstream file("instruments_data.txt");
        if (file.is_open()) {
            for (const auto& instrument : instruments) {
                file << instrument.name << ","
                     << instrument.type << ","
                     << instrument.displayName << ","
                     << instrument.pipLocation << ","
                     << instrument.marginRate << "\n";
            }
            file.close();
            std::cout << "Instruments data saved successfully." << std::endl;
        } else {
            std::cerr << "Unable to open file for saving instruments data." << std::endl;
        }
    } else {
        std::cout << "No instruments data to save." << std::endl;
    }
}


std::string get_his_data_filename(const std::string& pair, const std::string& granularity) {
    return pair + "_" + granularity + "_data.csv";
}

void plotCandlestick(const std::string& pair, const std::string& granularity) {
    std::string filename = get_his_data_filename(pair, granularity);

    std::vector<std::string> time;
    std::vector<double> open, high, low, close;

    // Read data from file
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string t;
        double o, h, l, c;
        if (!(iss >> t >> o >> h >> l >> c)) { break; }
        time.push_back(t);
        open.push_back(o);
        high.push_back(h);
        low.push_back(l);
        close.push_back(c);
    }

    // Get last 100 data points
    int dataSize = std::min(100, static_cast<int>(time.size()));
    std::vector<std::string> plotTime(time.end() - dataSize, time.end());
    std::vector<double> plotOpen(open.end() - dataSize, open.end());
    std::vector<double> plotHigh(high.end() - dataSize, high.end());
    std::vector<double> plotLow(low.end() - dataSize, low.end());
    std::vector<double> plotClose(close.end() - dataSize, close.end());

    // Create candlestick plot manually
    plt::figure_size(1200, 800);
    plt::title(pair + " - " + granularity);

    for (size_t i = 0; i < plotTime.size(); ++i) {
        // Draw the wick
        plt::plot({static_cast<double>(i), static_cast<double>(i)}, {plotLow[i], plotHigh[i]}, "b-");

        // Draw the body
        std::string color = plotClose[i] > plotOpen[i] ? "g" : "r";
        plt::plot({i-0.4, i-0.4, i+0.4, i+0.4, i-0.4},
                  {plotOpen[i], plotClose[i], plotClose[i], plotOpen[i], plotOpen[i]},
                  color);
    }

    plt::xlabel("Time");
    plt::ylabel("Price");
    plt::show();
}

