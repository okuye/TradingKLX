#include "PerformanceAssessor.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include "PriceData.h"  // Ensure you have this header included for the Trade struct

// Overall performance metrics calculation, including win rate and profit factor
void PerformanceAssessor::calculatePerformanceMetrics(const std::vector<Trade>& trades) {
    double totalProfit = 0.0;
    int winCount = 0;
    for (const auto& trade : trades) {
        totalProfit += trade.profit();  // Add parentheses here
        if (trade.profit() > 0) {  // Add parentheses here
            winCount++;
        }
    }
    double winRate = (static_cast<double>(winCount) / trades.size()) * 100;
    std::cout << "Total Profit: " << totalProfit << ", Win Rate: " << winRate << "%" << std::endl;
}

// Calculate total return as a percentage
double PerformanceAssessor::calculateTotalReturn(const std::vector<double>& portfolioValues, double initialBalance) {
    if (portfolioValues.empty()) throw std::invalid_argument("Portfolio values cannot be empty.");
    double finalBalance = portfolioValues.back();
    return ((finalBalance - initialBalance) / initialBalance) * 100;
}

// Calculate maximum drawdown as a percentage
double PerformanceAssessor::calculateMaxDrawdown(const std::vector<double>& portfolioValues) {
    double peak = portfolioValues[0];
    double maxDrawdown = 0.0;

    for (double value : portfolioValues) {
        peak = std::max(peak, value);
        double drawdown = (peak - value) / peak;
        maxDrawdown = std::max(maxDrawdown, drawdown);
    }
    return maxDrawdown * 100;
}

// Calculate win/loss ratio based on TradingSignals
double PerformanceAssessor::calculateWinLossRatio(const std::vector<TradingSignal>& signals) {
    if (signals.empty()) return 0.0;
    int wins = 0, losses = 0;
    for (const auto& signal : signals) {
        if (signal.sell) {
            signal.profit > 0 ? wins++ : losses++;
        }
    }
    return losses == 0 ? static_cast<double>(wins) : static_cast<double>(wins) / losses;
}

// Calculate average profit per trade based on TradingSignals
double PerformanceAssessor::calculateAverageProfit(const std::vector<TradingSignal>& signals) {
    if (signals.empty()) return 0.0;
    double totalProfit = 0.0;
    int count = 0;
    for (const auto& signal : signals) {
        if (signal.sell) {
            totalProfit += signal.profit;
            count++;
        }
    }
    return count == 0 ? 0.0 : totalProfit / count;
}

// Calculate Sharpe Ratio based on returns and risk-free rate
double PerformanceAssessor::calculateSharpeRatio(const std::vector<double>& returns, double riskFreeRate) {
    if (returns.empty()) throw std::invalid_argument("Returns cannot be empty.");

    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double excessReturn = meanReturn - riskFreeRate;

    double stdDev = std::sqrt(std::accumulate(returns.begin(), returns.end(), 0.0,
                                              [meanReturn](double acc, double ret) {
                                                  return acc + std::pow(ret - meanReturn, 2);
                                              }) / returns.size());

    return excessReturn / stdDev;
}

// Calculate Sortino Ratio based on returns and risk-free rate
double PerformanceAssessor::calculateSortinoRatio(const std::vector<double>& returns, double riskFreeRate) {
    if (returns.empty()) throw std::invalid_argument("Returns cannot be empty.");

    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double excessReturn = meanReturn - riskFreeRate;

    double downsideDeviation = std::sqrt(std::accumulate(returns.begin(), returns.end(), 0.0,
                                                         [riskFreeRate](double acc, double ret) {
                                                             return acc + (ret < riskFreeRate ? std::pow(ret - riskFreeRate, 2) : 0);
                                                         }) / returns.size());

    return excessReturn / downsideDeviation;
}

// Calculate win rate based on trades
double PerformanceAssessor::calculateWinRate(const std::vector<Trade>& trades) {
    if (trades.empty()) return 0.0;

    int wins = std::count_if(trades.begin(), trades.end(), [](const Trade& trade) {
        return trade.profit() > 0;  // Add parentheses here
    });

    return (static_cast<double>(wins) / trades.size()) * 100.0;
}

// Calculate return on investment based on trades and initial capital
double PerformanceAssessor::calculateReturnOnInvestment(const std::vector<Trade>& trades, double initialCapital) {
    double netProfit = std::accumulate(trades.begin(), trades.end(), 0.0, [](double sum, const Trade& trade) {
        return sum + trade.profit();  // Add parentheses here
    });
    return (netProfit / initialCapital) * 100.0;
}

// Profit factor calculation based on trades
double PerformanceAssessor::calculateProfitFactor(const std::vector<Trade>& trades) {
    double totalProfit = 0.0;
    double totalLoss = 0.0;
    for (const Trade& trade : trades) {
        if (trade.profit() > 0) {  // Add parentheses here
            totalProfit += trade.profit();  // Add parentheses here
        } else {
            totalLoss += trade.profit();  // Add parentheses here
        }
    }
    return totalProfit / -totalLoss;
}
