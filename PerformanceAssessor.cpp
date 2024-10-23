#include "PerformanceAssessor.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include "PriceData.h"

void PerformanceAssessor::calculatePerformanceMetrics(const std::vector<Trade>& trades) {
    if (trades.empty()) {
        std::cout << "No trades available." << std::endl;
        return;
    }

    double totalTrades = trades.size();
    double winningTrades = 0;
    double grossProfit = 0;
    double grossLoss = 0;
    double totalInvestedCapital = 0;
    double netProfit = 0;

    for (const auto& trade : trades) {
        double tradeProfit = trade.profit();
        if (tradeProfit > 0) {
            winningTrades++;
            grossProfit += tradeProfit;
        } else if (tradeProfit < 0) {
            grossLoss += std::abs(tradeProfit);
        }
        totalInvestedCapital += trade.entryPrice; // Ensure this correctly represents invested capital
        netProfit += tradeProfit;
    }

    std::cout << "Total Trades: " << totalTrades << std::endl;
    std::cout << "Winning Trades: " << winningTrades << std::endl;
    std::cout << "Gross Profit: " << grossProfit << std::endl;
    std::cout << "Gross Loss: " << grossLoss << std::endl;
    std::cout << "Total Invested Capital: " << totalInvestedCapital << std::endl;
    std::cout << "Net Profit: " << netProfit << std::endl;

    if (totalTrades > 0) {
        double winRate = (winningTrades / totalTrades) * 100;
        std::cout << "Win Rate: " << winRate << "%" << std::endl;
    } else {
        std::cout << "Win Rate: N/A (No Trades)" << std::endl;
    }

    if (grossLoss > 0) {
        double profitFactor = grossProfit / grossLoss;
        std::cout << "Profit Factor: " << profitFactor << std::endl;
    } else if (grossProfit > 0) {
        std::cout << "Profit Factor: Infinite (No Losses)" << std::endl;
    } else {
        std::cout << "Profit Factor: N/A (No Profits or Losses)" << std::endl;
    }

    if (totalInvestedCapital > 0) {
        double ROI = (netProfit / totalInvestedCapital) * 100;
        std::cout << "ROI: " << ROI << "%" << std::endl;
    } else {
        std::cout << "ROI: N/A (No Capital Invested)" << std::endl;
    }
}

double PerformanceAssessor::calculateTotalReturn(const std::vector<double>& portfolioValues, double initialBalance) {
    if (portfolioValues.empty()) throw std::invalid_argument("Portfolio values cannot be empty.");
    double finalBalance = portfolioValues.back();
    return ((finalBalance - initialBalance) / initialBalance) * 100;
}

double PerformanceAssessor::calculateMaxDrawdown(const std::vector<double>& portfolioValues) {
    if (portfolioValues.empty()) throw std::invalid_argument("Portfolio values cannot be empty.");

    double peak = portfolioValues[0];
    double maxDrawdown = 0.0;

    for (double value : portfolioValues) {
        peak = std::max(peak, value);
        double drawdown = (peak - value) / peak;
        maxDrawdown = std::max(maxDrawdown, drawdown);
    }
    return maxDrawdown * 100;
}

double PerformanceAssessor::calculateWinLossRatio(const std::vector<TradingSignal>& signals) {
    if (signals.empty()) return 0.0;
    int wins = 0, losses = 0;
    for (const auto& signal : signals) {
        if (signal.sell || signal.buy) { // Consider both sell and buy signals
            signal.profit > 0 ? wins++ : losses++;
        }
    }
    return losses == 0 ? static_cast<double>(wins) : static_cast<double>(wins) / losses;
}

double PerformanceAssessor::calculateAverageProfit(const std::vector<TradingSignal>& signals) {
    if (signals.empty()) return 0.0;
    double totalProfit = 0.0;
    int count = 0;
    for (const auto& signal : signals) {
        if (signal.sell || signal.buy) { // Consider both sell and buy signals
            totalProfit += signal.profit;
            count++;
        }
    }
    return count == 0 ? 0.0 : totalProfit / count;
}

double PerformanceAssessor::calculateSharpeRatio(const std::vector<double>& returns, double riskFreeRate) {
    if (returns.empty()) throw std::invalid_argument("Returns cannot be empty.");

    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double excessReturn = meanReturn - riskFreeRate;

    double stdDev = std::sqrt(std::accumulate(returns.begin(), returns.end(), 0.0,
                                              [meanReturn](double acc, double ret) {
                                                  return acc + std::pow(ret - meanReturn, 2);
                                              }) / returns.size());

    if (stdDev == 0.0) return 0.0; // Prevent division by zero

    return excessReturn / stdDev;
}

double PerformanceAssessor::calculateSortinoRatio(const std::vector<double>& returns, double riskFreeRate) {
    if (returns.empty()) throw std::invalid_argument("Returns cannot be empty.");

    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double excessReturn = meanReturn - riskFreeRate;

    double downsideDeviation = std::sqrt(std::accumulate(returns.begin(), returns.end(), 0.0,
                                                         [riskFreeRate](double acc, double ret) {
                                                             return acc + (ret < riskFreeRate ? std::pow(ret - riskFreeRate, 2) : 0);
                                                         }) / returns.size());

    if (downsideDeviation == 0.0) return 0.0; // Prevent division by zero

    return excessReturn / downsideDeviation;
}

double PerformanceAssessor::calculateWinRate(const std::vector<Trade>& trades) {
    if (trades.empty()) return 0.0;

    int wins = std::count_if(trades.begin(), trades.end(), [](const Trade& trade) {
        return trade.profit() > 0;
    });

    return (static_cast<double>(wins) / trades.size()) * 100.0;
}

double PerformanceAssessor::calculateReturnOnInvestment(const std::vector<Trade>& trades, double initialCapital) {
    if (initialCapital == 0.0) throw std::invalid_argument("Initial capital cannot be zero.");

    double netProfit = std::accumulate(trades.begin(), trades.end(), 0.0, [](double sum, const Trade& trade) {
        return sum + trade.profit();
    });
    return (netProfit / initialCapital) * 100.0;
}

double PerformanceAssessor::calculateProfitFactor(const std::vector<Trade>& trades) {
    double totalProfit = 0.0;
    double totalLoss = 0.0;
    for (const Trade& trade : trades) {
        if (trade.profit() > 0) {
            totalProfit += trade.profit();
        } else {
            totalLoss += trade.profit();
        }
    }
    if (totalLoss == 0.0) {
        return totalProfit > 0.0 ? INFINITY : 0.0; // Avoid division by zero
    }
    return totalProfit / -totalLoss;
}