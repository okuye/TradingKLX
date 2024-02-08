#include "TechnicalIndicators.h"
#include <algorithm> // For std::max_element and std::min_element
#include <cmath>     // For std::pow and std::sqrt
#include <stdexcept> // For std::invalid_argument

double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end() && index >= period - 1) {
        auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
        memo.tenkanSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }
    return memo.tenkanSenMemo[index];
}

double TechnicalIndicators::calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.kijunSenMemo.find(index) == memo.kijunSenMemo.end() && index >= period - 1) {
        auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
        memo.kijunSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }
    return memo.kijunSenMemo[index];
}

double TechnicalIndicators::calculateSenkouSpanA(int index, IchimokuMemo& memo) {
    // Assuming Tenkan-sen and Kijun-sen calculations are done elsewhere and memoized
    if (memo.senkouSpanAMemo.find(index) == memo.senkouSpanAMemo.end()) {
        memo.senkouSpanAMemo[index] = (memo.tenkanSenMemo[index] + memo.kijunSenMemo[index]) / 2.0;
    }
    return memo.senkouSpanAMemo[index];
}

double TechnicalIndicators::calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo) {
    if (memo.senkouSpanBMemo.find(index) == memo.senkouSpanBMemo.end() && index >= 52 - 1) {
        auto highIt = std::max_element(highs.begin() + index - 51, highs.begin() + index + 1); // 52 periods
        auto lowIt = std::min_element(lows.begin() + index - 51, lows.begin() + index + 1);
        memo.senkouSpanBMemo[index] = (*highIt + *lowIt) / 2.0;
    }
    return memo.senkouSpanBMemo[index];
}

void TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    for (size_t i = window - 1; i < data.size(); ++i) {
        if (memo.smaMemo.find(i) == memo.smaMemo.end()) {
            double sma = calculateSMA(data, i - window + 1, i + 1);
            memo.smaMemo[i] = sma;
            double stdDev = calculateStdDev(data, i - window + 1, i + 1, sma);
            memo.stdDevMemo[i] = stdDev;
            memo.upperBandMemo[i] = sma + numStdDev * stdDev;
            memo.lowerBandMemo[i] = sma - numStdDev * stdDev;
        }
    }
}

double TechnicalIndicators::calculateSMA(const std::vector<double>& data, int start, int end) {
    if (start < 0 || end > data.size() || start >= end) {
        throw std::invalid_argument("Invalid range for SMA calculation");
    }
    double sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0);
    return sum / (end - start);
}

double TechnicalIndicators::calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    if (start < 0 || end > data.size() || start >= end) {
        throw std::invalid_argument("Invalid range for standard deviation calculation");
    }
    double variance = std::accumulate(data.begin() + start, data.begin() + end, 0.0, [mean](double acc, double val) {
        return acc + std::pow(val - mean, 2);
    }) / (end - start);
    return std::sqrt(variance);
}
