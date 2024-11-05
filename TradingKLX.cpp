#include "TradingKLX.h"
#include "ConfigManager.h"
#include "TradingServerAPI.h"
#include "DataProcessor.h"
#include "PerformanceAssessor.h"
#include "TechnicalIndicators.h"
#include "SlidingWindow.h"
#include "TradeData.h"
#include "TradingStrategy.h"
#include "OandA_API.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <json/json.h>
#include "ForexConnect.h"

// Constructor
TradingKLX::TradingKLX()
        : dataProcessor(std::make_shared<DataProcessor>()),     // Initialize data processor
          configManager(std::make_shared<ConfigManager>()),     // Initialize config manager
          strategy(10000.0, 0.02, 1.5, 14),                     // Set up strategy with default values
          assessor()                                            // Initialize performance assessor
{
    std::cout << "TradingKLX Initialized" << std::endl;
}

// Destructor
TradingKLX::~TradingKLX() {
    std::cout << "TradingKLX Shutting Down..." << std::endl;
    // Resources managed by shared_ptrs are automatically cleaned up.
}

// Initialize and process data
void TradingKLX::InitializeAndProcessData(const std::string& configFilePath) {
    std::cout << "TradingKLX Application Starting..." << std::endl;

    // Load configuration
    Json::Value config = ConfigManager::readConfig(configFilePath);
    std::string apiKey = config["apiKey"].asString();
    std::string accountID = config["accountID"].asString();
    std::string fromSymbol = config["from_symbol"].asString();
    bool isLocalServer = config["ServerLocalOption"].asBool();
    std::string startDate = config["startDate"].asString();
    std::string endDate = config["endDate"].asString();
    std::string symbol = config["symbol"].asString();
    std::string api_key = config["api_key"].asString();
    int atrPeriod = config.get("atrPeriod", 14).asInt();

    // Initialize the trading strategy
    TradingStrategy strategy(10000.0, 0.02, 1.5, atrPeriod);
    SlidingWindow closingPricesWindow(14);

    if (isLocalServer) {
        std::cout << "Fetching historical data from local TradingServerAPI..." << std::endl;
        std::string apiUrl = "http://192.168.1.155:8000/trades";
        TradingServerAPI serverAPI(apiUrl);
        Json::Value historicalData;

        try {
            historicalData = serverAPI.fetchTrades(startDate, endDate, symbol, api_key);
        } catch (const std::exception &e) {
            std::cerr << "Failed to fetch or parse JSON from TradingServerAPI: " << e.what() << std::endl;
            return;
        }

        std::vector<TradeData> trades = DataProcessor::processTradingServerData(historicalData);
        for (const auto& trade : trades) {
            closingPricesWindow.addDataPoint(trade.closeAsk);
            if (closingPricesWindow.size() >= closingPricesWindow.getMaxSize()) {
                try {
                    strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error processing sliding window data: " << e.what() << std::endl;
                }
            }
        }
    } else {
        std::cout << "Fetching real-time data from OANDA API..." << std::endl;
        OandA_API oandaAPI(apiKey, accountID);
        Json::Value realTimeData;

        try {
            realTimeData = oandaAPI.getInstrumentPrices(fromSymbol);
        } catch (const std::exception &e) {
            std::cerr << "Failed to fetch or parse real-time data from OANDA API: " << e.what() << std::endl;
            return;
        }

        std::vector<TradeData> trades = DataProcessor::processOandAData(realTimeData);
        for (const auto& trade : trades) {
            strategy.onNewData(trade.highAsk, trade.lowAsk, trade.closeAsk);
        }
    }

    std::vector<TradingSignal> signals = strategy.evaluateSignals();
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << std::endl;
        }
        if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << std::endl;
        }
    }

    std::vector<Trade> executedTrades = strategy.getTrades();
    PerformanceAssessor assessor;
    std::cout << "\n--- Performance Metrics ---" << std::endl;
    assessor.calculatePerformanceMetrics(executedTrades);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
        return 1;
    }

    std::string configFilePath = argv[1];
    TradingKLX app;
    app.InitializeAndProcessData(configFilePath);

    return 0;
}
