#ifndef PERFORMANCE_ASSESSOR_H
#define PERFORMANCE_ASSESSOR_H

#include "Trade.h"
#include <vector>
#include "PriceData.h"
#include "TradingSignal.h"

class PerformanceAssessor {
public:
    void calculatePerformanceMetrics(const std::vector<Trade>& trades);

    static double calculateTotalReturn(const std::vector<double>& portfolioValues, double initialBalance);
    static double calculateMaxDrawdown(const std::vector<double>& portfolioValues);
    static double calculateWinLossRatio(const std::vector<TradingSignal>& signals);
    static double calculateAverageProfit(const std::vector<TradingSignal>& signals);
    static double calculateSharpeRatio(const std::vector<double>& returns, double riskFreeRate = 0.0);
    static double calculateSortinoRatio(const std::vector<double>& returns, double riskFreeRate = 0.0);
    static double calculateProfitFactor(const std::vector<Trade>& trades);
    static double calculateReturnOnInvestment(const std::vector<Trade>& trades, double initialCapital);
    static double calculateWinRate(const std::vector<Trade>& trades);
};

#endif // PERFORMANCE_ASSESSOR_H
