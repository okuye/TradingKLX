#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include "OandA_API.hpp"
#include "ConfigManager.h"
#include "DataProcessor.h"
#include "TechnicalIndicators.h"
#include "TradingStrategy.h"
#include "Utilities.h"
#include "JSONParser.h"

// Function to load configuration from a file
Json::Value loadConfiguration(const std::string& configFile) {
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

// Function to extract and validate configuration values
void extractConfigValues(const Json::Value& config, std::string& apiKey, std::string& accountID, std::string& from_symbol, std::string& to_symbol, std::string& dbName, std::string& collectionName) {
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
}

// Function to process market data and extract highs, lows, and closes
void processMarketData(const std::vector<PriceData>& priceData, std::vector<double>& highs, std::vector<double>& lows, std::vector<double>& closes) {
    for (const auto& data : priceData) {
        highs.push_back(data.high);
        lows.push_back(data.low);
        closes.push_back(data.close);
    }
}

// Named function to calculate Ichimoku indicators
void calculateIchimokuIndicators(std::vector<double>& highs, std::vector<double>& lows, std::vector<double>& tenkanS, std::vector<double>& kijunS,
                                 std::vector<double>& senkouA, std::vector<double>& senkouB, TechnicalIndicators& indicators, IchimokuMemo& ichimokuMemo) {
    for (size_t i = 0; i < highs.size(); ++i) {
        tenkanS.push_back(indicators.calculateTenkanSen(highs, lows, 9, i, ichimokuMemo));
        kijunS.push_back(indicators.calculateKijunSen(highs, lows, 26, i, ichimokuMemo));
        senkouA.push_back(indicators.calculateSenkouSpanA(i, ichimokuMemo));

        if (i >= 51) {
            senkouB.push_back(indicators.calculateSenkouSpanB(highs, lows, i, ichimokuMemo));
        }
    }
}

// Named function to calculate Bollinger Bands
void calculateBollingerBands(std::vector<double>& closes, std::vector<double>& lowerBB, std::vector<double>& upperBB,
                             TechnicalIndicators& indicators, BollingerBandsMemo& bbMemo) {
    for (size_t i = 0; i < closes.size(); ++i) {
        if (i >= 20) {
            auto bands = indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);
            lowerBB.push_back(bands.first);
            upperBB.push_back(bands.second);
        } else {
            lowerBB.push_back(0.0);
            upperBB.push_back(0.0);
        }
    }
}

// Function to log or output the trading signals
void logSignals(const std::vector<TradingSignal>& signals) {
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << ", Position Size: " << signal.positionSize << ", Stop Loss: " << signal.stopLossLevel << std::endl;
        } else if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << std::endl;
        }
    }
}

int main() {
    Json::Value config;

    try {
        config = loadConfiguration("config.json");
    } catch (const std::exception& e) {
        std::cerr << "Error reading config file: " << e.what() << std::endl;
        return 1;
    }

    std::string apiKey, accountID, from_symbol, to_symbol, dbName, collectionName;
    try {
        extractConfigValues(config, apiKey, accountID, from_symbol, to_symbol, dbName, collectionName);
    } catch (const std::exception& e) {
        std::cerr << "Error extracting config values: " << e.what() << std::endl;
        return 1;
    }

    OandA_API oandA_API(apiKey, accountID);
    Json::Value jsonData = oandA_API.getHistoricalData(from_symbol, "S5", 60);

    auto priceData = DataProcessor::processData(jsonToString(jsonData), "candles");
    TradingStrategy strategy(10000.0, 0.01, 3.0);
    TechnicalIndicators indicators;

    std::vector<double> highs, lows, closes;
    processMarketData(priceData, highs, lows, closes);

    // Declare memoization objects
    IchimokuMemo ichimokuMemo;
    BollingerBandsMemo bbMemo;

    std::vector<double> tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB;

    // Create threads for parallel execution
    std::thread ichimokuThread([&]() {
        calculateIchimokuIndicators(highs, lows, tenkanS, kijunS, senkouA, senkouB, indicators, ichimokuMemo);
    });

    std::thread bollingerThread([&]() {
        calculateBollingerBands(closes, lowerBB, upperBB, indicators, bbMemo);
    });


    // Wait for both threads to finish
    ichimokuThread.join();
    bollingerThread.join();

    std::vector<TradingSignal> signals = strategy.evaluateSignals(priceData, closes, highs, lows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    logSignals(signals);

    return 0;
}
