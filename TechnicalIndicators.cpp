#include "TechnicalIndicators.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <iostream>

// Optimized Tenkan-sen
double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (highs.size() != lows.size() || index < 0 || index >= highs.size() || period <= 0) {
        throw std::invalid_argument("Invalid input for Tenkan-sen calculation.");
    }
    
    if (memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end()) {
        auto highIt = std::max_element(highs.begin() + std::max(0, index - period + 1), highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + std::max(0, index - period + 1), lows.begin() + index + 1);
        memo.tenkanSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }

    return memo.tenkanSenMemo[index];
}

// Optimized Kijun-sen
double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (highs.size() != lows.size() || index < 0 || index >= highs.size() || period <= 0) {
        throw std::invalid_argument("Invalid input for Kijun-sen calculation.");
    }

    if (memo.kijunSenMemo.find(index) == memo.kijunSenMemo.end()) {
        auto highIt = std::max_element(highs.begin() + std::max(0, index - period + 1), highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + std::max(0, index - period + 1), lows.begin() + index + 1);
        memo.kijunSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }

    return memo.kijunSenMemo[index];
}

// Optimized Senkou Span A
double TechnicalIndicators::calculateSenkouSpanA(int index, IchimokuMemo& memo) {
    if (index < 0 || memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end() || memo.kijunSenMemo.find(index) == memo.kijunSenMemo.end()) {
        throw std::invalid_argument("Invalid input or missing data for Senkou Span A.");
    }

    if (memo.senkouSpanAMemo.find(index) != memo.senkouSpanAMemo.end()) {
        return memo.senkouSpanAMemo[index];
    }

    double senkouSpanA = (memo.tenkanSenMemo[index] + memo.kijunSenMemo[index]) / 2.0;
    memo.senkouSpanAMemo[index] = senkouSpanA;

    return senkouSpanA;
}

// Optimized Senkou Span B
double TechnicalIndicators::calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo) {
    const int period = 52;
    if (index < period - 1 || index >= highs.size() || index >= lows.size()) {
        throw std::invalid_argument("Index out of range for Senkou Span B.");
    }

    if (memo.senkouSpanBMemo.find(index) != memo.senkouSpanBMemo.end()) {
        return memo.senkouSpanBMemo[index];
    }

    auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
    auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
    double senkouSpanB = (*highIt + *lowIt) / 2.0;

    memo.senkouSpanBMemo[index] = senkouSpanB;
    return senkouSpanB;
}

// Optimized Bollinger Bands
std::pair<double, double> TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    if (data.size() < window || window < 1) {
        throw std::invalid_argument("Invalid data size or window for Bollinger Bands.");
    }

    size_t lastIndex = data.size() - 1;

    if (memo.upperBandMemo.find(lastIndex) == memo.upperBandMemo.end() || memo.lowerBandMemo.find(lastIndex) == memo.lowerBandMemo.end()) {
        double sma = calculateSMA(data, lastIndex, window);
        memo.smaMemo[lastIndex] = sma;

        double stdDev = calculateStdDev(data, lastIndex - window + 1, lastIndex + 1, sma);
        memo.stdDevMemo[lastIndex] = stdDev;

        double upperBand = sma + numStdDev * stdDev;
        double lowerBand = sma - numStdDev * stdDev;

        memo.upperBandMemo[lastIndex] = upperBand;
        memo.lowerBandMemo[lastIndex] = lowerBand;
    }

    return {memo.lowerBandMemo[lastIndex], memo.upperBandMemo[lastIndex]};
}

// Optimized SMA calculation
double TechnicalIndicators::calculateSMA(const std::vector<double>& data, int currentIndex, int period) {
    if (currentIndex < period - 1) {
        throw std::invalid_argument("Not enough data points for SMA.");
    }

    static double sum = 0.0;
    if (currentIndex == period - 1) {
        sum = std::accumulate(data.begin(), data.begin() + period, 0.0);
    } else {
        sum += data[currentIndex] - data[currentIndex - period];
    }

    return sum / period;
}

// Calculate Standard Deviation
double TechnicalIndicators::calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    if (start < 0 || end > static_cast<int>(data.size()) || start >= end) {
        throw std::invalid_argument("Invalid range for standard deviation calculation.");
    }

    double variance = std::accumulate(data.begin() + start, data.begin() + end, 0.0,
                                      [mean](double acc, double val) {
                                          return acc + std::pow(val - mean, 2);
                                      }) / (end - start);

    return std::sqrt(variance);
}

// Calculate ATR
double TechnicalIndicators::calculateATR(const std::vector<double>& highs, 
                                         const std::vector<double>& lows, 
                                         const std::vector<double>& closes, 
                                         int period, int currentIndex) {
    if (currentIndex < period) {
        throw std::invalid_argument("Not enough data points to calculate ATR.");
    }

    std::vector<double> trueRanges;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double highLowRange = highs[i] - lows[i];
        double highClosePrevRange = std::abs(highs[i] - closes[i - 1]);
        double lowClosePrevRange = std::abs(lows[i] - closes[i - 1]);

        double trueRange = std::max({highLowRange, highClosePrevRange, lowClosePrevRange});
        trueRanges.push_back(trueRange);

        // Log each true range for debugging
        std::cout << "Index " << i << " - High: " << highs[i] << ", Low: " << lows[i] 
                  << ", ClosePrev: " << closes[i - 1] << ", TrueRange: " << trueRange << std::endl;
    }

    double atr = std::accumulate(trueRanges.begin(), trueRanges.end(), 0.0) / period;

    // Log the final ATR value
    std::cout << "Calculated ATR for index " << currentIndex << ": " << atr << std::endl;

    return atr;
}

// Grid search for optimal Tenkan-sen and Kijun-sen periods
std::pair<int, int> TechnicalIndicators::gridSearchIchimokuOptimization(const std::vector<double>& highs, const std::vector<double>& lows) {
    int bestTenkan = 9, bestKijun = 26;
    double bestProfit = -INFINITY;

    for (int tenkanPeriod = 7; tenkanPeriod <= 12; ++tenkanPeriod) {
        for (int kijunPeriod = 22; kijunPeriod <= 30; ++kijunPeriod) {
            IchimokuMemo memo;
            std::vector<double> tenkanS, kijunS;

            for (size_t i = 0; i < highs.size(); ++i) {
                tenkanS.push_back(calculateTenkanSen(highs, lows, tenkanPeriod, i, memo));
                kijunS.push_back(calculateKijunSen(highs, lows, kijunPeriod, i, memo));
            }

            double profit = runStrategyWithParams(tenkanS, kijunS);  // Implement `runStrategyWithParams` for your strategy

            if (profit > bestProfit) {
                bestProfit = profit;
                bestTenkan = tenkanPeriod;
                bestKijun = kijunPeriod;
            }
        }
    }

    return {bestTenkan, bestKijun};
}

double TechnicalIndicators::runStrategyWithParams(const std::vector<double>& tenkanS, const std::vector<double>& kijunS) {
    // Example logic: Calculate a profit based on Tenkan-sen and Kijun-sen values
    // In practice, this should run your strategy logic and return a profit value

    double profit = 0.0;
    // TODO: Implement your trading logic here to calculate profit
    // This is just a placeholder logic
    for (size_t i = 0; i < tenkanS.size(); ++i) {
        if (tenkanS[i] > kijunS[i]) {
            profit += 10;  // Example: Gain $10 if Tenkan-sen > Kijun-sen
        } else {
            profit -= 5;   // Example: Lose $5 if Tenkan-sen <= Kijun-sen
        }
    }
    return profit;
}

