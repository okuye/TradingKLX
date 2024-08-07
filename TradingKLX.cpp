#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <string>
#include <sstream> // For stringstream

// Include jsoncpp headers
#include <json/json.h>

// Include your custom headers
#include "OandA_API.hpp"
#include "database_utils.h"
#include "ConfigManager.h"
#include "DataProcessor.h"
#include "TechnicalIndicators.h"

using namespace std;

// Function to read a JSON file using jsoncpp
Json::Value readConfig(const string& configFile) {
    ifstream configFileStream(configFile, ifstream::binary);
    if (!configFileStream.is_open()) {
        throw runtime_error("Could not open config file: " + configFile);
    }

    Json::CharReaderBuilder readerBuilder;
    Json::Value config;
    string errs;

    if (!Json::parseFromStream(readerBuilder, configFileStream, &config, &errs)) {
        throw runtime_error("Error parsing config file: " + errs);
    }

    return config;
}

int main() {
    // Initialize jsoncpp Value for configuration
    Json::Value config;

    try {
        // Attempt to read and parse the JSON configuration
        config = readConfig("config.json");
    } catch (const std::exception& e) {
        cerr << "Error reading config file: " << e.what() << endl;
        return 1;
    }

    // Extract configuration values with error handling
    string apiKey, accountID, from_symbol, to_symbol, dbName, collectionName;
    try {
        if (config.isMember("apiKey") && config["apiKey"].isString()) {
            apiKey = config["apiKey"].asString();
        } else {
            throw runtime_error("Missing or invalid apiKey");
        }
        if (config.isMember("accountID") && config["accountID"].isString()) {
            accountID = config["accountID"].asString();
        } else {
            throw runtime_error("Missing or invalid accountID");
        }
        if (config.isMember("from_symbol") && config["from_symbol"].isString()) {
            from_symbol = config["from_symbol"].asString();
        } else {
            throw runtime_error("Missing or invalid from_symbol");
        }
        if (config.isMember("to_symbol") && config["to_symbol"].isString()) {
            to_symbol = config["to_symbol"].asString();
        } else {
            throw runtime_error("Missing or invalid to_symbol");
        }
        if (config.isMember("dbName") && config["dbName"].isString()) {
            dbName = config["dbName"].asString();
        } else {
            throw runtime_error("Missing or invalid dbName");
        }
        if (config.isMember("collectionName") && config["collectionName"].isString()) {
            collectionName = config["collectionName"].asString();
        } else {
            throw runtime_error("Missing or invalid collectionName");
        }
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    // Create an instance of OandA_API with your API key and account ID
    OandA_API oandA_API(apiKey, accountID);

    // Fetch historical data from OANDA API
    string granularity = "M5"; // 5-minute granularity
    string from = "2023-01-01T00:00:00Z"; // Example start date
    string to = "2023-01-02T00:00:00Z"; // Example end date
    string jsonData = oandA_API.getHistoricalData(from_symbol, granularity, from, to).dump();

    // Process the fetched price data
    auto priceData = DataProcessor::processData(jsonData, "TimeSeriesKey");

    // Initialize trading parameters
    double accountBalance = 10000.0; // Example starting balance
    double riskPerTrade = 0.01; // Example risk per trade
    double stopLossMultiplier = 3.0; // Example stop loss multiplier

    // Initialize memoization structures and indicators
    TechnicalIndicators indicators;
    IchimokuMemo ichimokuMemo;
    BollingerBandsMemo bbMemo;

    // Example usage of memoized functions
    // For each price data point, calculate indicators using memoization
    for (size_t i = 0; i < priceData.size(); ++i) {
        vector<double> highs, lows, closes;

        // Populate highs, lows, and closes based on priceData
        // Assume these vectors are filled with corresponding values from priceData

        double tenkanSen = indicators.calculateTenkanSen(highs, lows, 9, i, ichimokuMemo);
        double kijunSen = indicators.calculateKijunSen(highs, lows, 26, i, ichimokuMemo);
        double senkouSpanA = indicators.calculateSenkouSpanA(i, ichimokuMemo);
        double senkouSpanB = indicators.calculateSenkouSpanB(highs, lows, i, ichimokuMemo);

        indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);

        // Additional logic for trading decisions based on calculated indicators
    }

    // Further processing based on indicators, risk management, etc.

    return 0;
}