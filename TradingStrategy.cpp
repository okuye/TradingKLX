#include "TradingStrategy.h"
#include "TradingSignal.h"
#include "Trade.h"
#include "TechnicalIndicators.h"
#include <spdlog/spdlog.h>
#include <iostream>

TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod)
        : accountBalance(initialBalance),
          riskPerTrade(riskPerTrade),
          stopLossMultiplier(stopLossMultiplier),
          atrPeriod(atrPeriod),
          highsWindow(52),
          lowsWindow(52),
          closes(52) {}

// Method to evaluate signals and store executed trades
std::vector<TradingSignal> TradingStrategy::evaluateSignals() {
    std::vector<TradingSignal> generatedSignals;

    if (closes.size() < 52 || highsWindow.size() < 52 || lowsWindow.size() < 52) {
        spdlog::warn("Insufficient data to evaluate signals.");
        return generatedSignals;
    }

    size_t latestIndex = closes.size() - 1;
    TradingSignal signal;
    signal.entryPrice = closes.at(latestIndex);
    double volatility = 0.05;  // Placeholder for actual volatility calculation

    if (volatility > 0.05) {
        signal.stopLossLevel = signal.entryPrice - (volatility * 1.2 * 2.0);
    } else {
        signal.stopLossLevel = signal.entryPrice - (volatility * 2.0);
    }

    // Placeholder values for trade execution
    signal.exitPrice = signal.entryPrice + 10.0;  // Mock exit price
    signal.profit = signal.exitPrice - signal.entryPrice;
    signal.buy = signal.profit > 0;
    signal.sell = !signal.buy;

    generatedSignals.push_back(signal);

    // Create and store Trade instance based on signal
    if (signal.buy || signal.sell) {
        Trade trade(signal.entryPrice, signal.exitPrice, signal.buy);
        executedTrades.push_back(trade);
    }

    return generatedSignals;
}

// Method to retrieve executed trades
std::vector<Trade> TradingStrategy::getTrades() const {
    return executedTrades;
}

void TradingStrategy::onNewData(double high, double low, double close) {
    highsWindow.addDataPoint(high);
    lowsWindow.addDataPoint(low);
    closes.addDataPoint(close);

    if (closes.size() >= 52) {
        evaluateSignals();
    }
}
