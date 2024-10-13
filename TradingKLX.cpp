#include "ConfigManager.h"
#include "OandA_API.hpp"
#include "TradingServerAPI.h"
#include "TradingStrategy.h"
#include "DataProcessor.h"
#include "PerformanceAssessor.h"
#include "SlidingWindow.h"
#include <iostream>
#include <vector>

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

    TradingStrategy strategy(10000.0, 0.02, 1.5);  // Example initial values
    SlidingWindow closingPricesWindow(14);  // Example window size for closing prices

    if (isLocalServer) {
        std::cout << "Fetching historical data from local TradingServerAPI..." << std::endl;

        // Construct the API URL with the necessary parameters
        std::string apiUrl = "http://192.168.1.155:8080/trades?startDate=" + startDate +
                             "&endDate=" + endDate +
                             "&symbol=" + symbol +
                             "&api_key=" + api_key;

        std::cout << "API URL: " << apiUrl << std::endl;

        // Instantiate TradingServerAPI with the constructed URL
        TradingServerAPI serverAPI("http://192.168.1.155:8080/trades");
        Json::Value historicalData = serverAPI.fetchTrades(startDate, endDate, symbol, api_key);

        // Process the historical data received
        std::vector<TradeData> trades = DataProcessor::processTradingServerData(historicalData);

        // Pass the trades data to the strategy
        for (const auto& trade : trades) {
            // Add data to the sliding window
            closingPricesWindow.addDataPoint(trade.closeAsk);

            // Ensure the sliding window has enough data points before processing
            if (closingPricesWindow.size() >= closingPricesWindow.getMaxSize()) {
                try {
                    std::vector<double> closingPrices = closingPricesWindow.toVector();
                    strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
                    // You can now safely process the closingPrices vector
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error processing sliding window: " << e.what() << std::endl;
                }
            }
        }
    }
 else {
        std::cout << "Fetching real-time data from OANDA API..." << std::endl;

        // Instantiate OANDA API and fetch real-time data
        OandA_API oandaAPI(apiKey, accountID);
        Json::Value realTimeData = oandaAPI.getInstrumentPrices(from_symbol);

        // Process the real-time data received
        std::vector<TradeData> trades = DataProcessor::processOandAData(realTimeData);

        // Pass the trades data to the strategy
        for (const auto& trade : trades) {
            strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
        }
    }

    // Evaluate the trading signals
    std::vector<TradingSignal> signals = strategy.evaluateSignals();
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << std::endl;
        }
        if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << std::endl;
        }
    }

    // Retrieve the trades executed based on signals
    std::vector<Trade> executedTrades = strategy.getTrades();

    // Initialize PerformanceAssessor to evaluate the strategy's performance
    PerformanceAssessor assessor;

    double winRate = assessor.calculateWinRate(executedTrades);
    double profitFactor = assessor.calculateProfitFactor(executedTrades);
    double roi = assessor.calculateReturnOnInvestment(executedTrades, 10000.0);

    // Output performance metrics
    std::cout << "Win Rate: " << winRate << "%\n";
    std::cout << "Profit Factor: " << profitFactor << "\n";
    std::cout << "ROI: " << roi << "%\n";

    return 0;
}
