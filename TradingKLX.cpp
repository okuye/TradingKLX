#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <nlohmann/json.hpp>
#include "OandA_API.hpp"
#include "database_utils.h"  // Include the header for database utilities
#include "ConfigManager.h"
#include "DataProcessor.h"
#include "TechnicalIndicators.h"

using namespace std;
using json = nlohmann::json;

// Configuration and data processing functions
json readConfig(const string& configFile);
vector<PriceData> processData(const string& jsonData);

int main() {
    // MongoDB instance initialization
    mongocxx::instance instance{};

    // Read configuration
    json config = ConfigManager::readConfig("config.json");

    // Extract configuration values
    bool useMongoDB = config["useMongoDB"].get<bool>();
    string mongoDBUri = config["mongoDBUri"].get<string>();
    string apiKey = config["apiKey"].get<string>();
    string accountID = config["accountID"].get<string>();
    string from_symbol = config["from_symbol"].get<string>();
    string to_symbol = config["to_symbol"].get<string>();
    string dbName = config["dbName"].get<string>();
    string collectionName = config["collectionName"].get<string>();

    // Create an instance of OandA_API with your API key and account ID
    OandA_API oandA_API(apiKey, accountID);

    mongocxx::client client{mongocxx::uri{mongoDBUri}};

    string jsonData;
    if (useMongoDB) {
        jsonData = fetchDataFromMongo(client, dbName, collectionName);
    } else {
        // Fetch historical data from OANDA API
        string granularity = "M5"; // 5-minute granularity
        string from = "2023-01-01T00:00:00Z"; // Example start date
        string to = "2023-01-02T00:00:00Z"; // Example end date
        jsonData = oandA_API.getHistoricalData(from_symbol, granularity, from, to).dump();
        storeDataInMongo(client, dbName, collectionName, jsonData);
    }

    auto priceData = DataProcessor::processData(jsonData, "TimeSeriesKey");
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
        // Assume highs, lows, and closes are vectors containing the respective values
        vector<double> highs, lows, closes;
        // Populate highs, lows, and closes based on priceData

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
