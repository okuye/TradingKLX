// TradingStrategy.cpp

#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include "PerformanceAssessor.h" // Uncomment if used
#include <vector>
#include <algorithm> // For std::max and std::min
#include <iostream>


int main() {
    std::cout << "TradingKLX Application Starting..." << std::endl;

    // Initialize the strategy with some example values
    double initialBalance = 10000.0;
    double riskPerTrade = 0.02;
    double stopLossMultiplier = 1.5;

    TradingStrategy strategy(initialBalance, riskPerTrade, stopLossMultiplier);

    // Simulate adding at least 52 data points (this is required for Ichimoku to calculate all indicators)
    for (int i = 0; i < 60; ++i) { // Adding 60 data points to make sure all indicators can be calculated
        double high = 1.15 + i * 0.01;
        double low = 1.10 + i * 0.01;
        double close = 1.12 + i * 0.01;
        strategy.onNewData(high, low, close);
    }

    // Evaluate signals
    std::vector<TradingSignal> signals = strategy.evaluateSignals();

    // Print the signals for demonstration
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << std::endl;
        }
        if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << std::endl;
        }
    }

    // Process the trades and calculate performance metrics
    std::vector<Trade> trades = strategy.getTrades();
    PerformanceAssessor assessor;

    double winRate = assessor.calculateWinRate(trades);
    double profitFactor = assessor.calculateProfitFactor(trades);
    double roi = assessor.calculateReturnOnInvestment(trades, initialBalance);

    std::cout << "Win Rate: " << winRate << "%\n";
    std::cout << "Profit Factor: " << profitFactor << "\n";
    std::cout << "ROI: " << roi << "%\n";

    return 0;
}