#include "TechnicalIndicators.h"
#include <algorithm> // For std::max_element and std::min_element
#include <cmath>     // For std::pow and std::sqrt
#include <stdexcept> // For std::invalid_argument
#include <numeric>   // For std::accumulate


// Optimized calculateTenkanSen with memoization and input validation
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

// Optimized calculateKijunSen with memoization and input validation
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

// Optimized Senkou Span A calculation with memoization
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

// Optimized Senkou Span B calculation with memoization
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

// Optimized Bollinger Bands calculation with memoization
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

// Optimized calculateSMA with rolling window technique
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

// Helper function to calculate standard deviation
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

// Implementation of calculateIchimokuIndicators
void calculateIchimokuIndicators(const std::vector<double>& highs, const std::vector<double>& lows, 
                                 std::vector<double>& tenkanS, std::vector<double>& kijunS, 
                                 std::vector<double>& senkouA, std::vector<double>& senkouB, 
                                 TechnicalIndicators& indicators, IchimokuMemo& ichimokuMemo) {
    for (size_t i = 0; i < highs.size(); ++i) {
        tenkanS.push_back(indicators.calculateTenkanSen(highs, lows, 9, i, ichimokuMemo));
        kijunS.push_back(indicators.calculateKijunSen(highs, lows, 26, i, ichimokuMemo));
        senkouA.push_back(indicators.calculateSenkouSpanA(i, ichimokuMemo));

        if (i >= 51) {
            senkouB.push_back(indicators.calculateSenkouSpanB(highs, lows, i, ichimokuMemo));
        }
    }
}

// Implementation of calculateBollingerBands
void calculateBollingerBands(const std::vector<double>& closes, std::vector<double>& lowerBB, 
                             std::vector<double>& upperBB, TechnicalIndicators& indicators, 
                             BollingerBandsMemo& bbMemo) {
    for (size_t i = 0; i < closes.size(); ++i) {
        if (i >= 20) {
            auto bands = indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);
            lowerBB.push_back(bands.first);
            upperBB.push_back(bands.second);
        } else {
            lowerBB.push_back(0.0);
            upperBB.push_back(0.0);
        }
    }
}

// Add this function to TechnicalIndicators.cpp

double TechnicalIndicators::calculateATR(const std::vector<double>& highs, 
                                         const std::vector<double>& lows, 
                                         const std::vector<double>& closes, 
                                         int period, int currentIndex) {
    if (currentIndex < period - 1) {
        throw std::invalid_argument("Not enough data points to calculate ATR.");
    }

    double atr = 0.0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double highLowRange = highs[i] - lows[i];
        double highClosePrevRange = std::abs(highs[i] - closes[i - 1]);
        double lowClosePrevRange = std::abs(lows[i] - closes[i - 1]);

        double trueRange = std::max({highLowRange, highClosePrevRange, lowClosePrevRange});
        atr += trueRange;
    }

    return atr / period;  // Average True Range over the period
}