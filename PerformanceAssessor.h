#ifndef PERFORMANCE_ASSESSOR_H
#define PERFORMANCE_ASSESSOR_H

#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "TradingSignal.h"  // Assuming TradingSignal contains 'profit' and trade details

class PerformanceAssessor {
public:
    // Function to calculate total return
    static double calculateTotalReturn(const std::vector<double>& portfolioValues, double initialBalance) {
        double finalBalance = portfolioValues.back();
        return (finalBalance - initialBalance) / initialBalance * 100;  // Return as a percentage
    }

    // Function to calculate maximum drawdown
    static double calculateMaxDrawdown(const std::vector<double>& portfolioValues) {
        double maxDrawdown = 0.0;
        double peak = portfolioValues[0];

        for (double value : portfolioValues) {
            if (value > peak) {
                peak = value;
            }
            double drawdown = (peak - value) / peak;
            if (drawdown > maxDrawdown) {
                maxDrawdown = drawdown;
            }
        }
        return maxDrawdown * 100;  // Return as a percentage
    }

    // Function to calculate win/loss ratio
    static double calculateWinLossRatio(const std::vector<TradingSignal>& signals) {
        int wins = 0, losses = 0;

        for (const auto& signal : signals) {
            if (signal.sell && signal.profit > 0) {
                wins++;
            } else if (signal.sell && signal.profit < 0) {
                losses++;
            }
        }

        return (losses == 0) ? static_cast<double>(wins) : static_cast<double>(wins) / losses;
    }

    // Function to calculate average profit/loss per trade
    static double calculateAverageProfit(const std::vector<TradingSignal>& signals) {
        double totalProfit = 0.0;
        int tradeCount = 0;

        for (const auto& signal : signals) {
            if (signal.sell) {  // Only consider closed trades
                totalProfit += signal.profit;
                tradeCount++;
            }
        }

        return (tradeCount == 0) ? 0.0 : totalProfit / tradeCount;
    }

    // Optional: Function to calculate Sharpe Ratio
    static double calculateSharpeRatio(const std::vector<double>& portfolioReturns, double riskFreeRate = 0.01) {
        double meanReturn = std::accumulate(portfolioReturns.begin(), portfolioReturns.end(), 0.0) / portfolioReturns.size();
        double variance = 0.0;

        for (double ret : portfolioReturns) {
            variance += std::pow(ret - meanReturn, 2);
        }

        double stddev = std::sqrt(variance / portfolioReturns.size());
        return (stddev == 0) ? 0 : (meanReturn - riskFreeRate) / stddev;
    }
};

#endif  // PERFORMANCE_ASSESSOR_H