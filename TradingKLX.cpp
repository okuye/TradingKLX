#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <string>
#include <sstream>
#include <json/json.h>
#include "OandA_API.hpp"
#include "ConfigManager.h"
#include "DataProcessor.h"
#include "TechnicalIndicators.h"
#include "TradingStrategy.h"
#include "Utilities.h"

Json::Value readConfig(const std::string& configFile) {
    std::ifstream configFileStream(configFile, std::ifstream::binary);
    if (!configFileStream.is_open()) {
        throw std::runtime_error("Could not open config file: " + configFile);
    }

    Json::CharReaderBuilder readerBuilder;
    Json::Value config;
    std::string errs;

    if (!Json::parseFromStream(readerBuilder, configFileStream, &config, &errs)) {
        throw std::runtime_error("Error parsing config file: " + errs);
    }

    return config;
}

int main() {
    Json::Value config;

    try {
        config = readConfig("config.json");
    } catch (const std::exception& e) {
        std::cerr << "Error reading config file: " << e.what() << std::endl;
        return 1;
    }

    std::string apiKey, accountID, from_symbol, to_symbol, dbName, collectionName;
    try {
        if (config.isMember("apiKey") && config["apiKey"].isString()) {
            apiKey = config["apiKey"].asString();
        } else {
            throw std::runtime_error("Missing or invalid apiKey");
        }
        if (config.isMember("accountID") && config["accountID"].isString()) {
            accountID = config["accountID"].asString();
        } else {
            throw std::runtime_error("Missing or invalid accountID");
        }
        if (config.isMember("from_symbol") && config["from_symbol"].isString()) {
            from_symbol = config["from_symbol"].asString();
        } else {
            throw std::runtime_error("Missing or invalid from_symbol");
        }
        if (config.isMember("to_symbol") && config["to_symbol"].isString()) {
            to_symbol = config["to_symbol"].asString();
        } else {
            throw std::runtime_error("Missing or invalid to_symbol");
        }
        if (config.isMember("dbName") && config["dbName"].isString()) {
            dbName = config["dbName"].asString();
        } else {
            throw std::runtime_error("Missing or invalid dbName");
        }
        if (config.isMember("collectionName") && config["collectionName"].isString()) {
            collectionName = config["collectionName"].asString();
        } else {
            throw std::runtime_error("Missing or invalid collectionName");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    OandA_API oandA_API(apiKey, accountID);
    std::string granularity = "S5";
    Json::Value jsonData = oandA_API.getHistoricalData("EUR_USD", granularity, 60);

    try {
        std::cout << "Full JSON Response: " << jsonToString(jsonData) << std::endl;

        if (jsonData.isMember("errorMessage")) {
            std::cerr << "API Error: " << jsonData["errorMessage"].asString() << std::endl;
            return 1;
        }

        if (!jsonData.isMember("candles") || !jsonData["candles"].isArray()) {
            std::cerr << "Invalid JSON structure: missing or incorrect 'candles' key" << std::endl;
            std::cout << "JSON Response contains 'candles': " << jsonData.isMember("candles") << std::endl;
            std::cout << "JSON 'candles' is array: " << jsonData["candles"].isArray() << std::endl;
            throw std::runtime_error("JSON structure is not as expected");
        }

        std::cout << "Number of elements in 'candles': " << jsonData["candles"].size() << std::endl;
        std::cout << "First element in 'candles': " << jsonToString(jsonData["candles"][0]) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    }

    std::string jsonDataString = jsonToString(jsonData);
    auto priceData = DataProcessor::processData(jsonDataString, "candles");

    double accountBalance = 10000.0;
    double riskPerTrade = 0.01;
    double stopLossMultiplier = 3.0;

    TechnicalIndicators indicators;
    TradingStrategy strategy(accountBalance, riskPerTrade, stopLossMultiplier);
    IchimokuMemo ichimokuMemo;
    BollingerBandsMemo bbMemo;

    std::vector<double> highs, lows, closes;
    for (const auto& data : priceData) {
        highs.push_back(data.high);
        lows.push_back(data.low);
        closes.push_back(data.close);
    }

    for (size_t i = 0; i < priceData.size(); ++i) {
        double tenkanSen = indicators.calculateTenkanSen(highs, lows, 9, i, ichimokuMemo);
        double kijunSen = indicators.calculateKijunSen(highs, lows, 26, i, ichimokuMemo);
        double senkouSpanA = indicators.calculateSenkouSpanA(i, ichimokuMemo);

        if (i >= 51) {
            double senkouSpanB = indicators.calculateSenkouSpanB(highs, lows, i, ichimokuMemo);
        }

        if (closes.size() >= 20) {
            indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);
        }
    }

    std::vector<TradingSignal> signals = strategy.evaluateSignals(priceData);

    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index " << signal.index
                      << ", Position size: " << signal.positionSize
                      << ", Stop loss: " << signal.stopLossLevel << std::endl;
        } else if (signal.sell) {
            std::cout << "Sell signal at index " << signal.index << std::endl;
        }
    }

    oandA_API.plotCandlestick("EUR_USD", granularity);

    return 0;
}
