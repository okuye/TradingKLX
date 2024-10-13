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

double TechnicalIndicators::calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.senkouSpanBMemo.find(index) != memo.senkouSpanBMemo.end()) {
        return memo.senkouSpanBMemo[index];
    }
    double senkouSpanB = (*std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1)
                          + *std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1)) / 2.0;
    memo.senkouSpanBMemo[index] = senkouSpanB;
    return senkouSpanB;
}

double TechnicalIndicators::calculateSenkouSpanA(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo) {
    if (memo.senkouSpanAMemo.find(index) != memo.senkouSpanAMemo.end()) {
        return memo.senkouSpanAMemo[index];
    }
    double senkouSpanA = (calculateTenkanSen(highs, lows, 9, index, memo) + calculateKijunSen(highs, lows, 26, index, memo)) / 2.0;
    memo.senkouSpanAMemo[index] = senkouSpanA;
    return senkouSpanA;
}

double TechnicalIndicators::calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    double variance = 0.0;
    for (int i = start; i < end; ++i) {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    return std::sqrt(variance / (end - start));
}

double TechnicalIndicators::calculateSMA(const std::vector<double>& prices, int period) {
    if (prices.size() < period) throw std::invalid_argument("Not enough data for SMA");
    
    // Additional bounds check
    if (period <= 0 || prices.size() < period) {
        throw std::out_of_range("Invalid period or insufficient data for calculating SMA");
    }

    double sum = std::accumulate(prices.end() - period, prices.end(), 0.0);
    return sum / period;
}


// Tenkan-sen and Kijun-sen calculation with memoization
double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    // Ensure the range is valid and enough data is present
    if (index - period + 1 < 0 || index >= highs.size() || index >= lows.size()) {
        throw std::out_of_range("Invalid range for calculating Tenkan-Sen: period extends beyond available data.");
    }

    double tenkanSen = (*std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1)
                        + *std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1)) / 2.0;
    
    memo.tenkanSenMemo[index] = tenkanSen;
    return tenkanSen;
}

// Kijun-sen calculation with memoization
double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    // Adjust the period to the available data points if the period exceeds the data size
    int actualPeriod = std::min(period, index + 1);  // Ensure period does not exceed index + 1 (available data points)
    
    // Ensure the index is within bounds for both highs and lows vectors
    if (index >= highs.size() || index >= lows.size()) {
        throw std::out_of_range("Index out of bounds in calculateKijunSen: index exceeds vector size");
    }
    
    // Calculate the adjusted Kijun-Sen
    double maxHigh = *std::max_element(highs.begin() + index - actualPeriod + 1, highs.begin() + index + 1);
    double minLow = *std::min_element(lows.begin() + index - actualPeriod + 1, lows.begin() + index + 1);

    double kijunSen = (maxHigh + minLow) / 2.0;

    // Store in memoization map
    memo.kijunSenMemo[index] = kijunSen;

    return kijunSen;
}





// Bollinger Bands calculation with memoization and SIMD optimization
std::pair<double, double> TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    if (data.size() < window) throw std::invalid_argument("Not enough data for Bollinger Bands");

    double sma = calculateSMA(data, window);

    if (data.size() < window) {
        throw std::out_of_range("Insufficient data for calculating Bollinger Bands");
    }

    double stdDev = calculateStdDev(data, data.size() - window, data.size(), sma);
    memo.upperBandMemo[data.size() - 1] = sma + numStdDev * stdDev;
    memo.lowerBandMemo[data.size() - 1] = sma - numStdDev * stdDev;
    return {memo.lowerBandMemo[data.size() - 1], memo.upperBandMemo[data.size() - 1]};
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

// ATR calculation
double TechnicalIndicators::calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex) {
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
