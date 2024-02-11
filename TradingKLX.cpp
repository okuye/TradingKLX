// TradingKLX.cpp : Defines the entry point for the application.
// #include <TradingKLX.h>
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
#include "AlphaVantageAPI.h"  
#include "database_utils.h"  // Include the header for database utilities
#include "ConfigManager.h"
#include "DataProcessor.h"
#include "TechnicalIndicators.h"



using namespace std;
using json = nlohmann::json;

// Utility function declarations
double calculateTenkanSen(const vector<double>& highs, const vector<double>& lows, int period, int index, IchimokuMemo& memo);
// double calculateKijunSen(const vector<double>& highs, const vector<double>& lows, int period, int index, IchimokuMemo& memo);
double calculateSenkouSpanA(int index, IchimokuMemo& memo);
double calculateSenkouSpanB(const vector<double>& highs, const vector<double>& lows, int index, IchimokuMemo& memo);
void calculateBollingerBandsWithMemoization(const vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo);
double calculateSMA(const vector<double>& data, int start, int end);
double calculateStdDev(const vector<double>& data, int start, int end, double mean);

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
    string function = config["function"].get<string>();
    string from_symbol = config["from_symbol"].get<string>();
    string to_symbol = config["to_symbol"].get<string>();
    string dbName = config["dbName"].get<string>();
    string collectionName = config["collectionName"].get<string>();

        // Create an instance of AlphaVantageAPI with your API key
    AlphaVantageAPI alphaVantageAPI(apiKey);  // Make sure apiKey is properly defined


    mongocxx::client client{mongocxx::uri{mongoDBUri}};

    string jsonData;
    if (useMongoDB) {
        jsonData = fetchDataFromMongo(client, dbName, collectionName);
    } else {
        jsonData = alphaVantageAPI.fetchData(function, from_symbol, to_symbol, "5min");  // Adjust interval as needed
        storeDataInMongo(client, dbName, collectionName, jsonData);
    }

    auto priceData = DataProcessor::processData(jsonData, "TimeSeriesKey");
    double accountBalance = 10000.0; // Example starting balance
    double riskPerTrade = 0.01; // Example risk per trade
    double stopLossMultiplier = 3.0; // Example stop loss multiplier

    // Initialize memoization structures and indicators
    TechnicalIndicators indicators;

    // Initialize memoization structures
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

// Definitions for utility functions, JSON configuration reading, data processing, and memoized calculations follow...
