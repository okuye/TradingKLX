#include "ConfigManager.h"
#include "OandA_API.hpp"
#include "TradingServerAPI.h"
#include "TradingStrategy.h"
#include "DataProcessor.h"
#include "PerformanceAssessor.h"
#include "SlidingWindow.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main() {
    std::cout << "TradingKLX Application Starting..." << std::endl;

    // Load configuration
    Json::Value config = ConfigManager::readConfig("config.json");

    std::string apiKey = config["apiKey"].asString();
    std::string accountID = config["accountID"].asString();
    std::string from_symbol = config["from_symbol"].asString();
    bool isLocalServer = config["ServerLocalOption"].asBool();

    std::string startDate = config["startDate"].asString();
    std::string endDate = config["endDate"].asString();
    std::string symbol = config["symbol"].asString();
    std::string api_key = config["api_key"].asString();

    // Get dynamic ATR period from the configuration or set a default value
    int atrPeriod = config.get("atrPeriod", 14).asInt();  // Default to 14 if not provided in config

    // Initialize Trading Strategy with dynamic atrPeriod
    TradingStrategy strategy(10000.0, 0.02, 1.5, atrPeriod);  // Use atrPeriod here
    SlidingWindow closingPricesWindow(14);  // Example window size for closing prices

    if (isLocalServer) {
        std::cout << "Fetching historical data from local TradingServerAPI..." << std::endl;

        std::string apiUrl = "http://192.168.1.155:8000/trades";

        TradingServerAPI serverAPI(apiUrl);
        Json::Value historicalData;

        try {
            historicalData = serverAPI.fetchTrades(startDate, endDate, symbol, api_key);
        } catch (const std::exception &e) {
            std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
            return 1;
        }

        std::vector<TradeData> trades = DataProcessor::processTradingServerData(historicalData);

        for (const auto& trade : trades) {
            closingPricesWindow.addDataPoint(trade.closeAsk);

            if (closingPricesWindow.size() >= closingPricesWindow.getMaxSize()) {
                try {
                    std::vector<double> closingPrices = closingPricesWindow.toVector();
                    strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error processing sliding window: " << e.what() << std::endl;
                }
            }
        }
    } else {
        std::cout << "Fetching real-time data from OANDA API..." << std::endl;

        OandA_API oandaAPI(apiKey, accountID);
        Json::Value realTimeData = oandaAPI.getInstrumentPrices(from_symbol);

        std::vector<TradeData> trades = DataProcessor::processOandAData(realTimeData);

        for (const auto& trade : trades) {
            strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
        }
    }

    // Evaluate signals and execute trades
    std::vector<TradingSignal> signals = strategy.evaluateSignals();
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << std::endl;
        }
        if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << std::endl;
        }
    }

    // Gather executed trades from the strategy
    std::vector<Trade> executedTrades = strategy.getTrades();

    // Performance assessment
    PerformanceAssessor assessor;

    // Use the new performance metrics function
    std::cout << "\n--- Performance Metrics ---" << std::endl;
    assessor.calculatePerformanceMetrics(executedTrades);

    return 0;
}


//int main() {
//    std::cout << "TradingKLX Application Starting..." << std::endl;
//
//    // Load configuration
//    Json::Value config = ConfigManager::readConfig("config.json");
//
//    std::string apiKey = config["apiKey"].asString();
//    std::string accountID = config["accountID"].asString();
//    std::string from_symbol = config["from_symbol"].asString();
//    bool isLocalServer = config["ServerLocalOption"].asBool();
//
//    std::string startDate = config["startDate"].asString();
//    std::string endDate = config["endDate"].asString();
//    std::string symbol = config["symbol"].asString();
//    std::string api_key = config["api_key"].asString();
//
//    // Get dynamic ATR period from the configuration or set a default value
//    int atrPeriod = config.get("atrPeriod", 14).asInt();  // Default to 14 if not provided in config
//
//    // Initialize Trading Strategy with dynamic atrPeriod
//    TradingStrategy strategy(10000.0, 0.02, 1.5, atrPeriod);  // Use atrPeriod here
//    SlidingWindow closingPricesWindow(14);  // Example window size for closing prices
//
//    if (isLocalServer) {
//        std::cout << "Fetching historical data from local TradingServerAPI..." << std::endl;
//
//        std::string apiUrl = "http://192.168.1.155:8000/trades";
//
//        TradingServerAPI serverAPI(apiUrl);
//        Json::Value historicalData;
//
//        try {
//            historicalData = serverAPI.fetchTrades(startDate, endDate, symbol, api_key);
////            std::cout << "Raw response: " << historicalData << std::endl;
//        } catch (const std::exception &e) {
//            std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
//            return 1;
//        }
//
//        std::vector<TradeData> trades = DataProcessor::processTradingServerData(historicalData);
//
//        for (const auto& trade : trades) {
//            closingPricesWindow.addDataPoint(trade.closeAsk);
//
//            if (closingPricesWindow.size() >= closingPricesWindow.getMaxSize()) {
//                try {
//                    std::vector<double> closingPrices = closingPricesWindow.toVector();
//                    strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
//                } catch (const std::runtime_error& e) {
//                    std::cerr << "Error processing sliding window: " << e.what() << std::endl;
//                }
//            }
//        }
//    } else {
//        std::cout << "Fetching real-time data from OANDA API..." << std::endl;
//
//        OandA_API oandaAPI(apiKey, accountID);
//        Json::Value realTimeData = oandaAPI.getInstrumentPrices(from_symbol);
//
//        std::vector<TradeData> trades = DataProcessor::processOandAData(realTimeData);
//
//        for (const auto& trade : trades) {
//            strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
//        }
//    }
//
//    std::vector<TradingSignal> signals = strategy.evaluateSignals();
//    for (const auto& signal : signals) {
//        if (signal.buy) {
//            std::cout << "Buy signal at index: " << signal.index << std::endl;
//        }
//        if (signal.sell) {
//            std::cout << "Sell signal at index: " << signal.index << std::endl;
//        }
//    }
//
//    std::vector<Trade> executedTrades = strategy.getTrades();
//
//    PerformanceAssessor assessor;
//
//    double winRate = assessor.calculateWinRate(executedTrades);
//    double profitFactor = assessor.calculateProfitFactor(executedTrades);
//    double roi = assessor.calculateReturnOnInvestment(executedTrades, 1000.0);
//
//    std::cout << "Win Rate: " << winRate << "%\n";
//    std::cout << "Profit Factor: " << profitFactor << "\n";
//    std::cout << "ROI: " << roi << "%\n";
//
//    return 0;
//}
