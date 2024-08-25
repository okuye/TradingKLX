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
#include "JSONParser.h"

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



    std::string jsonDataString = jsonToString(jsonData);
    auto priceData = DataProcessor::processData(jsonDataString, "candles");

    double accountBalance = 10000.0;
    double riskPerTrade = 0.01;
    double stopLossMultiplier = 3.0;

    TechnicalIndicators indicators;
    TradingStrategy strategy(accountBalance, riskPerTrade, stopLossMultiplier);
    IchimokuMemo ichimokuMemo;
    BollingerBandsMemo bbMemo;

    std::vector<double> highs, lows, closes, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB;
    for (const auto& data : priceData) {
        highs.push_back(data.high);
        lows.push_back(data.low);
        closes.push_back(data.close);
    }

    for (size_t i = 0; i < priceData.size(); ++i) {
        double tenkanSen = indicators.calculateTenkanSen(highs, lows, 9, i, ichimokuMemo);
        tenkanS.push_back(tenkanSen);

        double kijunSen = indicators.calculateKijunSen(highs, lows, 26, i, ichimokuMemo);
        kijunS.push_back(kijunSen);

        double senkouSpanA = indicators.calculateSenkouSpanA(i, ichimokuMemo);
        senkouA.push_back(senkouSpanA);

        if (i >= 51) {
            double senkouSpanB = indicators.calculateSenkouSpanB(highs, lows, i, ichimokuMemo);
            senkouB.push_back(senkouSpanB);
        }

        if (closes.size() >= 20) {
            auto [lowerBand, upperBand] = indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);
            lowerBB.push_back(lowerBand);
            upperBB.push_back(upperBand);
        } else {
            lowerBB.push_back(0.0);  // Placeholder for indices where we don't have enough data
            upperBB.push_back(0.0);  // Placeholder for indices where we don't have enough data
        }
    }

    std::vector<TradingSignal> signals = strategy.evaluateSignals(priceData, closes, highs, lows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    // Output signals for debugging purposes
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << ", Position Size: " << signal.positionSize << ", Stop Loss: " << signal.stopLossLevel << std::endl;
        } else if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << std::endl;
        }
    }

    return 0;
}
