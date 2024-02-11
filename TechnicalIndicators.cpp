#include "TechnicalIndicators.h"
#include <algorithm> // For std::max_element and std::min_element
#include <cmath>     // For std::pow and std::sqrt
#include <stdexcept> // For std::invalid_argument
#include <numeric>   // For std::accumulate

double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    // Validate input
    if (index < period - 1) throw std::invalid_argument("Index out of range for Tenkan-sen calculation");
    if (memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end() && index >= period - 1) {
        auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
        memo.tenkanSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }
    return memo.tenkanSenMemo[index];
}

double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    // Validate inputs
    if (period <= 0) throw std::invalid_argument("Period must be positive for Kijun-sen calculation");
    if (index < period - 1 || index >= highs.size() || index >= lows.size())
        throw std::invalid_argument("Index out of range for Kijun-sen calculation");
    
    // Check if Kijun-sen is already calculated for this index
    auto it = memo.kijunSenMemo.find(index);
    if (it != memo.kijunSenMemo.end()) {
        // Return memoized value if available
        return it->second;
    }
    
    // Calculate Kijun-sen
    auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
    auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
    double kijunSen = (*highIt + *lowIt) / 2.0;
    
    // Memoize and return the result
    memo.kijunSenMemo[index] = kijunSen;
    return kijunSen;
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

void TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    if (data.size() < window || window < 1) {
        throw std::invalid_argument("Invalid data size or window period for Bollinger Bands calculation");
    }

    for (size_t i = window - 1; i < data.size(); ++i) {
        // Check if Bollinger Bands are already calculated for this index
        if (memo.upperBandMemo.find(i) != memo.upperBandMemo.end() &&
            memo.lowerBandMemo.find(i) != memo.lowerBandMemo.end()) {
            // Bollinger Bands already calculated for this index, skip to next
            continue;
        }

        double sma = 0.0;
        // Check if SMA is already memoized for this index
        auto smaIt = memo.smaMemo.find(i);
        if (smaIt != memo.smaMemo.end()) {
            sma = smaIt->second;
        } else {
            sma = calculateSMA(data, i - window + 1, i + 1);
            memo.smaMemo[i] = sma;  // Memoize the calculated SMA
        }

        double stdDev = 0.0;
        // Check if standard deviation is already memoized for this index
        auto stdDevIt = memo.stdDevMemo.find(i);
        if (stdDevIt != memo.stdDevMemo.end()) {
            stdDev = stdDevIt->second;
        } else {
            stdDev = calculateStdDev(data, i - window + 1, i + 1, sma);
            memo.stdDevMemo[i] = stdDev;  // Memoize the calculated standard deviation
        }

        // Calculate and memoize the Bollinger Bands
        memo.upperBandMemo[i] = sma + numStdDev * stdDev;
        memo.lowerBandMemo[i] = sma - numStdDev * stdDev;
    }
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
