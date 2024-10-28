#ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H

#include <vector>
#include "TradingSignal.h"  // Ensure TradingSignal is included
#include "Trade.h"          // Include Trade for executed trades
#include "SlidingWindow.h"

class TradingStrategy {
public:
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod);
    void onNewData(double high, double low, double close);
    std::vector<TradingSignal> evaluateSignals();

    // Method to retrieve executed trades
    std::vector<Trade> getTrades() const;

private:
    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;
    int atrPeriod;
    SlidingWindow highsWindow;
    SlidingWindow lowsWindow;
    SlidingWindow closes;

    std::vector<Trade> executedTrades;  // Store executed trades
};

#endif // TRADINGSTRATEGY_H
