#include "TechnicalIndicators.h"
#ifdef __ARM_NEON
#include <arm_neon.h>
#elif defined(__AVX2__)
#include <immintrin.h>
#endif
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <iostream>
#include <spdlog/spdlog.h>

// Senkou Span B calculation with memoization
double TechnicalIndicators::calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.senkouSpanBMemo.find(index) != memo.senkouSpanBMemo.end()) {
        return memo.senkouSpanBMemo[index];
    }

    if (index - period + 1 < 0) {
        spdlog::error("Invalid range for calculating Senkou Span B: index = {}, period = {}", index, period);
        throw std::out_of_range("Invalid range for Senkou Span B.");
    }

    double senkouSpanB = (*std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1)
                          + *std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1)) / 2.0;
    memo.senkouSpanBMemo[index] = senkouSpanB;
    spdlog::info("Senkou Span B calculated at index {}: {}", index, senkouSpanB);

    return senkouSpanB;
}

// Senkou Span A calculation with memoization
double TechnicalIndicators::calculateSenkouSpanA(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo) {
    if (memo.senkouSpanAMemo.find(index) != memo.senkouSpanAMemo.end()) {
        return memo.senkouSpanAMemo[index];
    }

    spdlog::info("Calculating Senkou Span A at index {}", index);
    double senkouSpanA = (calculateTenkanSen(highs, lows, 9, index, memo) + calculateKijunSen(highs, lows, 26, index, memo)) / 2.0;
    memo.senkouSpanAMemo[index] = senkouSpanA;
    spdlog::info("Senkou Span A calculated at index {}: {}", index, senkouSpanA);

    return senkouSpanA;
}

// Standard Deviation calculation
double TechnicalIndicators::calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    double variance = 0.0;
    for (int i = start; i < end; ++i) {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    return std::sqrt(variance / (end - start));
}

// SMA calculation
double TechnicalIndicators::calculateSMA(const std::vector<double>& prices, int period) {
    if (prices.size() < period) throw std::invalid_argument("Not enough data for SMA");

    double sum = std::accumulate(prices.end() - period, prices.end(), 0.0);
    return sum / period;
}

// Tenkan-sen calculation with memoization
double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (index - period + 1 < 0 || index >= highs.size() || index >= lows.size()) {
        spdlog::error("Invalid range for calculating Tenkan-Sen: index = {}, period = {}", index, period);
        throw std::out_of_range("Invalid range for Tenkan-Sen.");
    }

    if (memo.tenkanSenMemo.find(index) != memo.tenkanSenMemo.end()) {
        return memo.tenkanSenMemo[index];
    }

    spdlog::info("Calculating Tenkan-Sen at index {}", index);

    double tenkanSen = (*std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1)
                        + *std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1)) / 2.0;

    memo.tenkanSenMemo[index] = tenkanSen;
    spdlog::info("Tenkan-Sen calculated at index {}: {}", index, tenkanSen);

    return tenkanSen;
}

// Kijun-sen calculation with memoization
double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (index - period + 1 < 0 || index >= highs.size() || index >= lows.size()) {
        spdlog::error("Invalid range for calculating Kijun-Sen: index = {}, period = {}", index, period);
        throw std::out_of_range("Invalid range for Kijun-Sen.");
    }

    if (memo.kijunSenMemo.find(index) != memo.kijunSenMemo.end()) {
        return memo.kijunSenMemo[index];
    }

    spdlog::info("Calculating Kijun-Sen at index {}", index);

    double kijunSen = (*std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1)
                       + *std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1)) / 2.0;

    memo.kijunSenMemo[index] = kijunSen;
    spdlog::info("Kijun-Sen calculated at index {}: {}", index, kijunSen);

    return kijunSen;
}

std::pair<double, double> TechnicalIndicators::calculateBollingerBandsWithMemoization(
        const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {

    // Validate window size
    if (data.size() < window) {
        spdlog::error("Insufficient data for Bollinger Bands. Data size: {}, Required window: {}", data.size(), window);
        return {0.0, 0.0};  // Return dummy values if not enough data
    }

    // Use memoization to avoid recalculating if already done
    auto it = memo.find(window);
    if (it != memo.end()) {
        return it->second;  // Return the cached result if available
    }

    // Calculate the mean of the last `window` entries
    double mean = std::accumulate(data.end() - window, data.end(), 0.0) / window;

    // Calculate the standard deviation
    double variance = 0.0;
    for (size_t i = data.size() - window; i < data.size(); ++i) {
        variance += std::pow(data[i] - mean, 2);
    }
    double stdDev = std::sqrt(variance / window);

    // Calculate Bollinger Bands
    double lowerBand = mean - numStdDev * stdDev;
    double upperBand = mean + numStdDev * stdDev;

    // Store the result in the memoization map
    memo[window] = std::make_pair(lowerBand, upperBand);

    spdlog::info("Bollinger Bands calculated. LowerBB: {}, UpperBB: {}", lowerBand, upperBand);

    return {lowerBand, upperBand};
}


// ATR calculation
double TechnicalIndicators::calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex) {
    if (currentIndex < period) {
        spdlog::error("Not enough data points to calculate ATR. Current index: {}, period: {}", currentIndex, period);
        throw std::invalid_argument("Not enough data points to calculate ATR.");
    }

    double sumTrueRange = 0.0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double highLowRange = highs[i] - lows[i];
        double highClosePrevRange = std::abs(highs[i] - closes[i - 1]);
        double lowClosePrevRange = std::abs(lows[i] - closes[i - 1]);
        double trueRange = std::max({highLowRange, highClosePrevRange, lowClosePrevRange});
        sumTrueRange += trueRange;
    }

    double atr = sumTrueRange / period;
    spdlog::info("ATR calculated at index {}: {}", currentIndex, atr);
    return atr;
}

// SIMD-optimized standard deviation for NEON (ARM)
double TechnicalIndicators::neon_stdDev(const std::vector<double>& data, int window, double mean) {
    int vectorizable_length = window - (window % 2);
    float64x2_t variance_vec = vdupq_n_f64(0.0);

    for (int i = 0; i < vectorizable_length; i += 2) {
        float64x2_t data_vec = vld1q_f64(&data[i]);
        float64x2_t diff_vec = vsubq_f64(data_vec, vdupq_n_f64(mean));
        variance_vec = vmlaq_f64(variance_vec, diff_vec, diff_vec);  // Accumulate square of differences
    }

    double variance = vaddvq_f64(variance_vec) / window;
    for (int i = vectorizable_length; i < window; i++) {
        variance += std::pow(data[i] - mean, 2);
    }

    return std::sqrt(variance);
}

// SIMD-optimized standard deviation for AVX2 (x86-64)
#ifdef __AVX2__
double TechnicalIndicators::avx2_stdDev(const std::vector<double>& data, int window, double mean) {
    int vectorizable_length = window - (window % 4);
    __m256d variance_vec = _mm256_setzero_pd();

    for (int i = 0; i < vectorizable_length; i += 4) {
        __m256d data_vec = _mm256_loadu_pd(&data[i]);
        __m256d diff_vec = _mm256_sub_pd(data_vec, _mm256_set1_pd(mean));
        variance_vec = _mm256_fmadd_pd(diff_vec, diff_vec, variance_vec);  // Accumulate square of differences
    }

    double variance_array[4];
    _mm256_storeu_pd(variance_array, variance_vec);

    double variance = 0.0;
    for (int i = 0; i < 4; i++) {
        variance += variance_array[i];
    }
    variance /= window;

    for (int i = vectorizable_length; i < window; i++) {
        variance += std::pow(data[i] - mean, 2);
    }

    return std::sqrt(variance);
}
#endif
