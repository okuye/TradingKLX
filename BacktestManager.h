#ifndef BACKTEST_MANAGER_H
#define BACKTEST_MANAGER_H

#include <string>
#include "OandA_API.hpp"
#include "TradingServerAPI.h"

class BacktestManager {
public:
    BacktestManager(bool isRealTime, const std::string& apiKey, const std::string& accountID);

    void runBacktest(const std::string& startDate, const std::string& endDate, const std::string& symbol);

private:
    bool isRealTime;
    OandA_API oandaAPI;
    TradingServerAPI tradingServerAPI;

    void runRealTimeTrading(const std::string& symbol);
    void runHistoricalBacktest(const std::string& startDate, const std::string& endDate, const std::string& symbol);
};

#endif // BACKTEST_MANAGER_H
