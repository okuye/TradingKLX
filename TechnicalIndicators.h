#ifndef TRADINGKLX_TECHNICALINDICATORS_H
#define TRADINGKLX_TECHNICALINDICATORS_H

#include <vector>
#include <unordered_map>
#include "SlidingWindow.h"

// Memoization classes for Ichimoku indicators
struct IchimokuMemo {
    std::unordered_map<int, double> tenkanSenMemo;
    std::unordered_map<int, double> kijunSenMemo;
    std::unordered_map<int, double> senkouSpanAMemo;
    std::unordered_map<int, double> senkouSpanBMemo;
};

// Composite key for Bollinger Bands memoization
struct BollingerBandsKey {
    int window;
    int index;

    bool operator==(const BollingerBandsKey& other) const {
        return window == other.window && index == other.index;
    }
};

// Hash function for BollingerBandsKey
struct BollingerBandsKeyHash {
    std::size_t operator()(const BollingerBandsKey& key) const {
        return std::hash<int>()(key.window) ^ (std::hash<int>()(key.index) << 1);
    }
};

// Memoization for Bollinger Bands: composite key mapped to lower/upper bands
using BollingerBandsMemo = std::unordered_map<BollingerBandsKey, std::pair<double, double>, BollingerBandsKeyHash>;

class TechnicalIndicators {
public:
    double calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateSenkouSpanA(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo);
    double calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    std::pair<double, double> calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo, int index);
    double calculateSMA(const std::vector<double>& prices, int period) const;
    double calculateStdDev(const std::vector<double>& data, int start, int end, double mean) const;
    double calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex) const;

    // SIMD functions for NEON and AVX2
    double neon_stdDev(const std::vector<double>& data, int window, double mean) const;
    double avx2_stdDev(const std::vector<double>& data, int window, double mean) const;

    double calculateStandardDeviation(const SlidingWindow& data, size_t index, size_t period) const;
};

#endif // TRADINGKLX_TECHNICALINDICATORS_H