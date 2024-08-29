#include "TechnicalIndicators.h"
#include <algorithm> // For std::max_element and std::min_element
#include <cmath>     // For std::pow and std::sqrt
#include <stdexcept> // For std::invalid_argument
#include <numeric>   // For std::accumulate

// Improved calculateTenkanSen with better input validation and memoization
double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    // Validate inputs: check that index is within range and period is positive
    if (highs.size() != lows.size() || index < 0 || index >= highs.size() || period <= 0) {
        throw std::invalid_argument("Invalid input for Tenkan-sen calculation");
    }

    // Check if Tenkan-sen is already calculated for this index to avoid redundant calculation
    if (memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end()) {
        // Calculate Tenkan-sen for this index
        auto highIt = std::max_element(highs.begin() + std::max(0, index - period + 1), highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + std::max(0, index - period + 1), lows.begin() + index + 1);
        memo.tenkanSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }

    return memo.tenkanSenMemo[index];
}

double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    // Validate inputs: check that index is within range and period is positive
    if (highs.size() != lows.size() || index < 0 || index >= highs.size() || period <= 0) {
        throw std::invalid_argument("Invalid input for Kijun-sen calculation");
    }

    // Check if Kijun-sen is already calculated for this index to avoid redundant calculation
    if (memo.kijunSenMemo.find(index) == memo.kijunSenMemo.end()) {
        // Calculate Kijun-sen for this index
        auto highIt = std::max_element(highs.begin() + std::max(0, index - period + 1), highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + std::max(0, index - period + 1), lows.begin() + index + 1);
        memo.kijunSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }

    return memo.kijunSenMemo[index];
}

double TechnicalIndicators::calculateSenkouSpanA(int index, IchimokuMemo& memo) {
    // Validate input: Check if the index is non-negative and Tenkan-sen and Kijun-sen values are available for this index
    if (index < 0 || memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end() || memo.kijunSenMemo.find(index) == memo.kijunSenMemo.end()) {
        throw std::invalid_argument("Invalid index or required Tenkan-sen/Kijun-sen values not available for Senkou Span A calculation");
    }

    // Check if Senkou Span A is already calculated for this index
    auto it = memo.senkouSpanAMemo.find(index);
    if (it != memo.senkouSpanAMemo.end()) {
        // If already calculated, return the memoized value
        return it->second;
    }

    // Calculate Senkou Span A using memoized Tenkan-sen and Kijun-sen values and store the result for future reference
    double senkouSpanA = (memo.tenkanSenMemo[index] + memo.kijunSenMemo[index]) / 2.0;
    memo.senkouSpanAMemo[index] = senkouSpanA;

    return senkouSpanA;
}

double TechnicalIndicators::calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo) {
    // The period for Senkou Span B is traditionally set to 52
    const int period = 52;

    // Validate inputs
    if (index < period - 1 || index >= highs.size() || index >= lows.size())
        throw std::invalid_argument("Index out of range for Senkou Span B calculation");

    // Check if Senkou Span B is already calculated for this index
    auto it = memo.senkouSpanBMemo.find(index);
    if (it != memo.senkouSpanBMemo.end()) {
        // Return memoized value if available
        return it->second;
    }

    // Calculate Senkou Span B
    auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
    auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
    double senkouSpanB = (*highIt + *lowIt) / 2.0;

    // Memoize and return the result
    memo.senkouSpanBMemo[index] = senkouSpanB;
    return senkouSpanB;
}


std::pair<double, double> TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    if (data.size() < window || window < 1) {
        throw std::invalid_argument("Invalid data size or window period for Bollinger Bands calculation");
    }

    size_t lastIndex = data.size() - 1;

    if (memo.upperBandMemo.find(lastIndex) == memo.upperBandMemo.end() ||
        memo.lowerBandMemo.find(lastIndex) == memo.lowerBandMemo.end()) {

        double sma = calculateSMA(data, lastIndex - window + 1, lastIndex + 1);
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



double TechnicalIndicators::calculateSMA(const std::vector<double>& data, int start, int end) {
    // Validate input range
    if (start < 0 || end > static_cast<int>(data.size()) || start >= end) {
        throw std::invalid_argument("Invalid range for SMA calculation");
    }

    // Optimize calculation by checking for memoized values if applicable
    // For instance, if calculating SMA for a rolling window, check if the previous value is memoized
    // This part would require additional logic and data structure for memoization, which is not shown here

    // Calculate the sum of elements in the given range
    double sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0);

    // Return the average
    return sum / (end - start);
}

double TechnicalIndicators::calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    // Validate input range for safety
    if (start < 0 || end > static_cast<int>(data.size()) || start >= end) {
        throw std::invalid_argument("Invalid range for standard deviation calculation");
    }

    // Calculate variance
    double variance = std::accumulate(data.begin() + start, data.begin() + end, 0.0,
                                      [mean](double acc, double val) {
                                          return acc + std::pow(val - mean, 2);
                                      }) / (end - start);

    // Return the square root of variance (Standard Deviation)
    return std::sqrt(variance);
}

double TechnicalIndicators::calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int index) {
    // Validate inputs
    if (highs.size() != lows.size() || lows.size() != closes.size() || index < period || index >= highs.size()) {
        throw std::invalid_argument("Invalid input for ATR calculation");
    }

    double atr = 0.0;
    for (int i = index - period + 1; i <= index; ++i) {
        double trueRange = std::max({highs[i] - lows[i], std::abs(highs[i] - closes[i - 1]), std::abs(lows[i] - closes[i - 1])});
        atr += trueRange;
    }

    return atr / period;
}