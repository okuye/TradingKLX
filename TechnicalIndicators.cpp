#include "TechnicalIndicators.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <iostream>
#include "SlidingWindow.h"

// Optimized Tenkan-sen
double TechnicalIndicators::calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows,
                                               int period, int index, IchimokuMemo& memo) {
    if (highs.size() < period || lows.size() < period || index < 0 || index >= highs.size()) {
        throw std::invalid_argument("Invalid data size or index for Tenkan-sen calculation.");
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

// Optimized Senkou Span A with error handling for missing data
double TechnicalIndicators::calculateSenkouSpanA(const SlidingWindow& tenkanWindow, const SlidingWindow& kijunWindow, IchimokuMemo& memo) {
    // Ensure we have enough data in both Tenkan-sen and Kijun-sen windows
    if (tenkanWindow.size() < 9 || kijunWindow.size() < 26) {
        std::cerr << "Insufficient data for Senkou Span A calculation." << std::endl;
        return 0.0;
    }

    // Calculate Senkou Span A
    double tenkan = tenkanWindow.getData().back();
    double kijun = kijunWindow.getData().back();

    double senkouSpanA = (tenkan + kijun) / 2.0;
    memo.senkouSpanAMemo[tenkanWindow.size()] = senkouSpanA;
    return senkouSpanA;
}

double TechnicalIndicators::calculateSenkouSpanB(const SlidingWindow& highsWindow, const SlidingWindow& lowsWindow, IchimokuMemo& memo) {
    const int period = 52;

    // Ensure we have enough data in the window for Senkou Span B
    if (highsWindow.size() < period || lowsWindow.size() < period) {
        std::cerr << "Insufficient data for Senkou Span B calculation." << std::endl;
        return 0.0;
    }

    double high52 = *std::max_element(highsWindow.getData().begin(), highsWindow.getData().end());
    double low52 = *std::min_element(lowsWindow.getData().begin(), lowsWindow.getData().end());

    double senkouSpanB = (high52 + low52) / 2.0;
    memo.senkouSpanBMemo[highsWindow.size()] = senkouSpanB;
    return senkouSpanB;
}

// Updated Bollinger Bands Calculation with Memoization
std::pair<double, double> TechnicalIndicators::calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo) {
    if (data.size() < window || window < 1) {
        throw std::invalid_argument("Invalid data size or window for Bollinger Bands.");
    }

    size_t lastIndex = data.size() - 1;

    if (memo.upperBandMemo.find(lastIndex) == memo.upperBandMemo.end() || memo.lowerBandMemo.find(lastIndex) == memo.lowerBandMemo.end()) {
        double sma = calculateSMA(data, lastIndex, window);  // Corrected
        memo.smaMemo[lastIndex] = sma;

        double stdDev = calculateStdDev(data, lastIndex - window + 1, lastIndex + 1, sma);  // Corrected
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

    double sum = 0.0;  // Removed static
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
                                         int period, int currentIndex,
                                         double volatilityThreshold) {
    if (currentIndex < period) {
        throw std::invalid_argument("Not enough data points to calculate ATR.");
    }

    std::vector<double> trueRanges;
    bool inFlatSequence = false;
    int flatSequenceStart = -1;

    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double highLowRange = highs[i] - lows[i];
        double highClosePrevRange = std::abs(highs[i] - closes[i - 1]);
        double lowClosePrevRange = std::abs(lows[i] - closes[i - 1]);

        double trueRange = std::max({highLowRange, highClosePrevRange, lowClosePrevRange});

        if (trueRange < volatilityThreshold) {
            if (!inFlatSequence) {
                flatSequenceStart = i;
                inFlatSequence = true;
            }
        } else {
            if (inFlatSequence) {
                std::cout << "Flat price data detected from index " << flatSequenceStart
                          << " to " << i - 1 << std::endl;
                inFlatSequence = false;
            }
        }

        trueRanges.push_back(trueRange);
    }

    if (inFlatSequence) {
        std::cout << "Flat price data detected from index " << flatSequenceStart
                  << " to " << currentIndex << std::endl;
    }

    return std::accumulate(trueRanges.begin(), trueRanges.end(), 0.0) / period;
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

            // Assuming runStrategyWithParams() returns a profit value
            double profit = runStrategyWithParams(tenkanS, kijunS);

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