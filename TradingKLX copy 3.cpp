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
#include "PerformanceAssessor.h"
#include "PriceData.h"

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

// Function to load data from the Ask and Bid CSV files
std::vector<PriceData> loadHistoricalData(const std::string& askFile, const std::string& bidFile) {
    std::vector<PriceData> priceData;
    std::ifstream askFileStream(askFile);
    std::ifstream bidFileStream(bidFile);

    if (!askFileStream.is_open()) {
        throw std::runtime_error("Could not open Ask file: " + askFile);
    }
    if (!bidFileStream.is_open()) {
        throw std::runtime_error("Could not open Bid file: " + bidFile);
    }

    std::string lineAsk, lineBid;
    std::getline(askFileStream, lineAsk);  // Skip header
    std::getline(bidFileStream, lineBid);  // Skip header

    while (std::getline(askFileStream, lineAsk) && std::getline(bidFileStream, lineBid)) {
        std::stringstream ssAsk(lineAsk);
        std::stringstream ssBid(lineBid);
        PriceData dataAsk, dataBid;
        std::string tempAsk, tempBid;

        try {
            // Process ASK data
            std::getline(ssAsk, dataAsk.timestamp, ',');
            std::getline(ssAsk, tempAsk, ',');
            dataAsk.open = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            dataAsk.high = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            dataAsk.low = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            dataAsk.close = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            dataAsk.volume = std::stod(tempAsk);

            // Process BID data
            std::getline(ssBid, dataBid.timestamp, ',');
            std::getline(ssBid, tempBid, ',');
            dataBid.open = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            dataBid.high = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            dataBid.low = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            dataBid.close = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            dataBid.volume = std::stod(tempBid);

            // Ensure timestamps match between Ask and Bid data
            if (dataAsk.timestamp == dataBid.timestamp) {
                priceData.push_back({dataAsk.timestamp, dataAsk.open, dataAsk.high, dataAsk.low, dataAsk.close, dataAsk.volume, 
                                     dataBid.open, dataBid.high, dataBid.low, dataBid.close, dataBid.volume});
            } else {
                std::cerr << "Mismatched timestamps: " << dataAsk.timestamp << " != " << dataBid.timestamp << std::endl;
            }

        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data at row (Ask or Bid): " << lineAsk << " or " << lineBid << "\nError: " << e.what() << std::endl;
        }
    }

    askFileStream.close();
    bidFileStream.close();
    return priceData;
}

// Function to process market data and extract highs, lows, and closes
void processMarketData(const std::vector<PriceData>& priceData, std::vector<double>& askHighs, 
                       std::vector<double>& askLows, std::vector<double>& askCloses, 
                       std::vector<double>& bidHighs, std::vector<double>& bidLows, std::vector<double>& bidCloses) {
    for (const auto& data : priceData) {
        askHighs.push_back(data.high);
        askLows.push_back(data.low);
        askCloses.push_back(data.close);

        bidHighs.push_back(data.bidHigh);
        bidLows.push_back(data.bidLow);
        bidCloses.push_back(data.bidClose);
    }
}

int main() {
    // Load historical data from Ask and Bid CSV files
    std::vector<PriceData> priceData = loadHistoricalData("EURUSD_Candlestick_1_Hour_ASK_01.09.2023-07.09.2024.csv",
                                                          "EURUSD_Candlestick_1_Hour_BID_01.01.2023-07.09.2024.csv");

    // Initialize strategy and indicators
    TradingStrategy strategy(10000.0, 0.0015, 1.25);  // $10,000 starting balance, 0.15% risk, stop-loss multiplier 1.25
    TechnicalIndicators indicators;

    std::vector<double> askHighs, askLows, askCloses, bidHighs, bidLows, bidCloses;
    processMarketData(priceData, askHighs, askLows, askCloses, bidHighs, bidLows, bidCloses);

    // Declare memoization objects for Ichimoku and Bollinger Bands
    IchimokuMemo ichimokuMemo;
    BollingerBandsMemo bbMemo;

    std::vector<double> tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB;

    // Calculate Ichimoku indicators (using Ask prices for calculations)
    calculateIchimokuIndicators(askHighs, askLows, tenkanS, kijunS, senkouA, senkouB, indicators, ichimokuMemo);

    // Calculate Bollinger Bands (using Bid closes for calculations)
    calculateBollingerBands(bidCloses, lowerBB, upperBB, indicators, bbMemo);

    // Evaluate buy/sell signals (you can use both Ask and Bid data here)
    std::vector<TradingSignal> signals = strategy.evaluateSignals(priceData, askCloses, askHighs, askLows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    // Log trading signals
    logSignals(signals);

    // Track portfolio values for performance assessment
    std::vector<double> portfolioValues;
    double currentBalance = strategy.getAccountBalance();
    portfolioValues.push_back(currentBalance);

    for (const auto& signal : signals) {
        currentBalance = strategy.getAccountBalance();
        portfolioValues.push_back(currentBalance);
    }

    // Use PerformanceAssessor to evaluate backtest results
    double totalReturn = PerformanceAssessor::calculateTotalReturn(portfolioValues, 10000.0);  // Initial balance of $10,000
    double maxDrawdown = PerformanceAssessor::calculateMaxDrawdown(portfolioValues);
    double winLossRatio = PerformanceAssessor::calculateWinLossRatio(signals);
    double avgProfit = PerformanceAssessor::calculateAverageProfit(signals);

    // Output the performance metrics
    std::cout << "Total Return: " << totalReturn << "%\n";
    std::cout << "Max Drawdown: " << maxDrawdown << "%\n";
    std::cout << "Win/Loss Ratio: " << winLossRatio << "\n";
    std::cout << "Average Profit per Trade: " << avgProfit << "\n";

    return 0;
}