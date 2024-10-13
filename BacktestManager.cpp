#include "TradingServerAPI.h"
#include "OandA_API.hpp"
#include "DataProcessor.h"
#include "PriceData.h"
#include <vector>
#include <iostream>

class BacktestManager {
public:
    BacktestManager(bool isRealTime, const std::string& apiKey, const std::string& accountID)
            : isRealTime(isRealTime), oandaAPI(apiKey, accountID), tradingServerAPI("http://localhost:8080") {}

    void runBacktest(const std::string& startDate, const std::string& endDate, const std::string& symbol) {
        if (isRealTime) {
            runRealTimeTrading(symbol);  // Real-time trading with OandA_API
        } else {
            runHistoricalBacktest(startDate, endDate, symbol);  // Backtesting with TradingServerAPI
        }
    }

private:
    bool isRealTime;
    OandA_API oandaAPI;         // For real-time execution with OANDA
    TradingServerAPI tradingServerAPI;  // For fetching historical data for backtesting

    void runRealTimeTrading(const std::string& symbol) {
        Json::Value instrumentPrices = oandaAPI.getInstrumentPrices(symbol);
        std::vector<TradeData> trades = DataProcessor::processOandAData(instrumentPrices);

        for (const auto& trade : trades) {
            // Process real-time trade data
            std::cout << "Real-time trade for symbol: " << trade.symbol << " at time: " << trade.date << std::endl;
        }
    }

    void runHistoricalBacktest(const std::string& startDate, const std::string& endDate, const std::string& symbol) {
        Json::Value tradesJson = tradingServerAPI.fetchTrades(startDate, endDate, symbol, "Ee-osjmRSwyXkPA3QBFe");
        std::vector<TradeData> trades = DataProcessor::processTradingServerData(tradesJson);

        // Feed the processed data into your trading algorithm for backtesting
        for (const auto& trade : trades) {
            std::cout << "Backtesting trade for symbol: " << trade.symbol << " at hour: " << trade.hour << std::endl;
        }
    }
};
