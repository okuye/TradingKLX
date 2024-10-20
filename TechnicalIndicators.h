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

// Memoization for Bollinger Bands: window size mapped to lower/upper bands
using BollingerBandsMemo = std::unordered_map<int, std::pair<double, double>>;

class TechnicalIndicators {
public:
    double calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateSenkouSpanA(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo);
    double calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    std::pair<double, double> calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo);
    double calculateSMA(const std::vector<double>& prices, int period);
    double calculateStdDev(const std::vector<double>& data, int start, int end, double mean);
    double calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex);

    // SIMD functions for NEON and AVX2
    double neon_stdDev(const std::vector<double>& data, int window, double mean);
    double avx2_stdDev(const std::vector<double>& data, int window, double mean);

    double calculateStandardDeviation(const SlidingWindow& data, size_t index, size_t period);
};

#endif // TRADINGKLX_TECHNICALINDICATORS_H
