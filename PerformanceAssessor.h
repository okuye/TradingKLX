#ifndef PERFORMANCE_ASSESSOR_H
#define PERFORMANCE_ASSESSOR_H

#include <vector>
#include "PriceData.h"  // Ensure this is included for the Trade struct
#include "TradingSignal.h"

class PerformanceAssessor {
public:
    // Calculate performance metrics for the trades
    void calculatePerformanceMetrics(const std::vector<Trade>& trades);

    // Calculate total return as a percentage
    static double calculateTotalReturn(const std::vector<double>& portfolioValues, double initialBalance);

    // Calculate maximum drawdown as a percentage
    static double calculateMaxDrawdown(const std::vector<double>& portfolioValues);

    // Calculate win/loss ratio based on TradingSignals
    static double calculateWinLossRatio(const std::vector<TradingSignal>& signals);

    // Calculate average profit per trade based on TradingSignals
    static double calculateAverageProfit(const std::vector<TradingSignal>& signals);

    // Calculate Sharpe Ratio based on returns and risk-free rate
    static double calculateSharpeRatio(const std::vector<double>& returns, double riskFreeRate = 0.0);

    // Calculate Sortino Ratio based on returns and risk-free rate
    static double calculateSortinoRatio(const std::vector<double>& returns, double riskFreeRate = 0.0);

    // Calculate profit factor based on trades
    static double calculateProfitFactor(const std::vector<Trade>& trades);

    // Calculate return on investment (ROI) based on trades and initial capital
    static double calculateReturnOnInvestment(const std::vector<Trade>& trades, double initialCapital);

    // Calculate win rate based on trades
    static double calculateWinRate(const std::vector<Trade>& trades);
};

#endif // PERFORMANCE_ASSESSOR_H
