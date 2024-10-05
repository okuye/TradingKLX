#include "TechnicalIndicators.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <iostream>

// Optimized Tenkan-sen Calculation
double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (index < period - 1) {
        std::cerr << "Insufficient data for Tenkan-sen calculation at index " << index << std::endl;
        return 0;
    }

    if (memo.tenkanSenMemo.find(index) != memo.tenkanSenMemo.end()) {
        return memo.tenkanSenMemo[index];
    }

    auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
    auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
    double tenkanSen = (*highIt + *lowIt) / 2.0;
    memo.tenkanSenMemo[index] = tenkanSen;
    return tenkanSen;
}

// Optimized Kijun-sen Calculation
double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.kijunSenMemo.find(index) != memo.kijunSenMemo.end()) {
        return memo.kijunSenMemo[index];
    }

    auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
    auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
    double kijunSen = (*highIt + *lowIt) / 2.0;
    memo.kijunSenMemo[index] = kijunSen;
    return kijunSen;
}

// Senkou Span A Calculation
double TechnicalIndicators::calculateSenkouSpanA(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    double tenkanS = calculateTenkanSen(highs, lows, 9, index, memo);
    double kijunS = calculateKijunSen(highs, lows, 26, index, memo);
    double senkouSpanA = (tenkanS + kijunS) / 2.0;
    memo.senkouSpanAMemo[index] = senkouSpanA;
    return senkouSpanA;
}

// Senkou Span B Calculation
double TechnicalIndicators::calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
    auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
    double senkouSpanB = (*highIt + *lowIt) / 2.0;
    memo.senkouSpanBMemo[index] = senkouSpanB;
    return senkouSpanB;
}

// Bollinger Bands Calculation with Memoization
std::pair<double, double> TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    if (data.size() < static_cast<size_t>(window)) {
        throw std::invalid_argument("Not enough data for Bollinger Bands calculation.");
    }

    size_t lastIndex = data.size() - 1;

    double sma = calculateSMA(data, window);
    double stdDev = calculateStdDev(data, lastIndex - window + 1, lastIndex + 1, sma);

    double upperBand = sma + numStdDev * stdDev;
    double lowerBand = sma - numStdDev * stdDev;

    memo.upperBandMemo[lastIndex] = upperBand;
    memo.lowerBandMemo[lastIndex] = lowerBand;

    return {lowerBand, upperBand};
}

// Simple Moving Average (SMA) Calculation
double TechnicalIndicators::calculateSMA(const std::vector<double>& prices, int period) {
    if (prices.size() < period) {
        return 0.0;
    }
    double sum = std::accumulate(prices.end() - period, prices.end(), 0.0);
    return sum / period;
}

// Standard Deviation Calculation
double TechnicalIndicators::calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    double variance = std::accumulate(data.begin() + start, data.begin() + end, 0.0,
                                      [mean](double acc, double val) {
                                          return acc + std::pow(val - mean, 2);
                                      }) / (end - start);
    return std::sqrt(variance);
}

// ATR (Average True Range) Calculation
double TechnicalIndicators::calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex, double volatilityThreshold) {
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
    }

    return std::accumulate(trueRanges.begin(), trueRanges.end(), 0.0) / period;
}