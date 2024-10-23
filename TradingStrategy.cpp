// TradingStrategy.cpp
#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include "PerformanceAssessor.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <omp.h>
#include <mutex>
#include <spdlog/spdlog.h>
#include <stdexcept>

// Define constants
const double HIGH_VOLATILITY_THRESHOLD = 0.05;    // Example value
const double STOP_LOSS_MULTIPLIER_ADJUST = 1.2;  // Adjustment multiplier for high volatility
const double STOP_LOSS_MULTIPLIER_DEFAULT = 2.0;// Default multiplier for stop loss
const double TAKE_PROFIT_MULTIPLIER = 3.0;       // Example multiplier for take profit
const double DEFAULT_POSITION_SIZE = 1000.0;     // Default position size

// Constructor Implementation
TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod)
        : accountBalance(initialBalance),
          riskPerTrade(riskPerTrade),
          stopLossMultiplier(stopLossMultiplier),
          smaPeriod(50),
          bollingerBandsPeriod(20),
          bollingerBandsMultiplier(2.0),
          atrPeriod(atrPeriod),
          hasLoggedInsufficientData(false),
          inPosition(false),
          highsWindow(52),
          lowsWindow(52),
          closes(52),
          tenkanWindow(9),
          kijunWindow(26),
          insufficientDataLogged(false),
          tenkanSenLogged(false) {
    portfolioBalanceHistory.push_back(accountBalance);
}

// Function to record the current portfolio balance
void TradingStrategy::recordPortfolioBalance() {
    portfolioBalanceHistory.push_back(accountBalance);
}

// Function to calculate exit price using TechnicalIndicators' ATR
double TradingStrategy::calculateExitPrice(size_t index) {
    // Ensure the index is within the range of the sliding windows
    if (index >= closes.size()) {
        spdlog::error("Index out of range in calculateExitPrice: {}", index);
        throw std::out_of_range("Index out of range in calculateExitPrice");
    }

    // Retrieve data from sliding windows
    std::vector<double> highs = highsWindow.toVector();
    std::vector<double> lows = lowsWindow.toVector();
    std::vector<double> closesVec = closes.toVector();

    // Calculate ATR using TechnicalIndicators
    double atr = technicalIndicators.calculateATR(highs, lows, closesVec, atrPeriod, static_cast<int>(index));

    double currentPrice = closesVec.at(index);

    // Example logic: Exit price is current price plus ATR
    double exitPrice = currentPrice + atr;

    spdlog::info("Calculated exit price at index {}: {}", index, exitPrice);
    return exitPrice;
}

// Function to calculate position size
double TradingStrategy::calculatePositionSize() const {
    // Your logic for position size calculation
    // For now, it returns a default value
    return DEFAULT_POSITION_SIZE;
}

std::vector<TradingSignal> TradingStrategy::evaluateSignals() {
    std::vector<TradingSignal> generatedSignals;

    spdlog::debug("closes.size(): {}", closes.size());
    spdlog::debug("highsWindow.size(): {}", highsWindow.size());
    spdlog::debug("lowsWindow.size(): {}", lowsWindow.size());

    // Check if there's enough data to evaluate signals
    if (closes.size() < 52 || highsWindow.size() < 52 || lowsWindow.size() < 52) {
        if (!insufficientDataLogged) {
            spdlog::warn("Insufficient data to evaluate signals. Skipping evaluation.");
            insufficientDataLogged = true;
        }
        return generatedSignals;  // Return empty signals vector
    }

    // Additional checks for Bollinger Bands (typically a 20-period window)
    if (lowerBB.size() < static_cast<size_t>(bollingerBandsPeriod) || upperBB.size() < static_cast<size_t>(bollingerBandsPeriod)) {
        if (!insufficientDataLogged) {
            spdlog::warn("Insufficient data for Bollinger Bands evaluation. Skipping evaluation.");
            insufficientDataLogged = true;
        }
        return generatedSignals;  // Return empty signals vector
    }

    spdlog::info("Data sufficient, proceeding with signal evaluation.");

    // Generate signal for the latest data point only
    size_t latestIndex = closes.size() - 1;
    TradingSignal signal;

    try {
        // Entry price based on the latest close
        signal.entryPrice = closes.at(latestIndex);
        spdlog::info("Entry price set at index {}: {}", latestIndex, signal.entryPrice);

        // Adjust stop-loss based on volatility (e.g., last 20 periods)
        size_t volatilityPeriod = std::min(static_cast<size_t>(20), closes.size());
        double volatility = technicalIndicators.calculateStandardDeviation(closes, latestIndex, volatilityPeriod);
        if (volatility > HIGH_VOLATILITY_THRESHOLD) {
            spdlog::warn("High volatility detected (stdDev: {}), adjusting stop-loss level.", volatility);
            // Adjust stop-loss
            signal.stopLossLevel = signal.entryPrice - (volatility * STOP_LOSS_MULTIPLIER_ADJUST * STOP_LOSS_MULTIPLIER_DEFAULT);
        } else {
            // Default stop-loss
            signal.stopLossLevel = signal.entryPrice - (volatility * STOP_LOSS_MULTIPLIER_DEFAULT);
        }

        // Calculate exit price
        signal.exitPrice = calculateExitPrice(latestIndex);
        signal.profit = (signal.exitPrice - signal.entryPrice) * calculatePositionSize();

        // Determine buy/sell signals based on your strategy
        // Placeholder logic: If profit is positive, consider it a buy signal; otherwise, a sell signal
        if (signal.profit > 0) {
            signal.buy = true;
        } else {
            signal.sell = true;
        }

        generatedSignals.push_back(signal);
        spdlog::info("Signal generated: entryPrice={}, exitPrice={}, profit={}, buy={}, sell={}",
                     signal.entryPrice, signal.exitPrice, signal.profit, signal.buy, signal.sell);
    } catch (const std::exception& e) {
        spdlog::error("Error generating signal: {}", e.what());
    }

    return generatedSignals;
}

void TradingStrategy::onNewData(double high, double low, double close) {
    // Add new data points to the sliding windows
    highsWindow.addDataPoint(high);
    lowsWindow.addDataPoint(low);
    closes.addDataPoint(close);

    size_t dataSize = closes.size();

    if (dataSize < 52) {
        if (!insufficientDataLogged) {
            spdlog::warn("Insufficient data for indicators. Available data: {}, Required: 52", dataSize);
            insufficientDataLogged = true;
        }
        return;
    }
    insufficientDataLogged = false;

    // Calculate Bollinger Bands when new data arrives
    try {
        auto [lowerBand, upperBand] = technicalIndicators.calculateBollingerBandsWithMemoization(
                closes.toVector(), bollingerBandsPeriod, bollingerBandsMultiplier, bbMemo, static_cast<int>(dataSize - 1)
        );
        lowerBB.push_back(lowerBand);
        upperBB.push_back(upperBand);
        spdlog::info("New Bollinger Bands - Lower: {}, Upper: {}", lowerBand, upperBand);
    } catch (const std::exception& e) {
        spdlog::error("Error calculating Bollinger Bands: {}", e.what());
        return;  // Exit the function if we can't calculate Bollinger Bands
    }

    bool enoughDataForIchimoku = (highsWindow.size() >= 52 && lowsWindow.size() >= 52 && tenkanS.size() >= 26);
    bool enoughDataForBollinger = (lowerBB.size() >= static_cast<size_t>(bollingerBandsPeriod) && upperBB.size() >= static_cast<size_t>(bollingerBandsPeriod));

    if (!(enoughDataForIchimoku || enoughDataForBollinger)) {
        spdlog::warn("Insufficient data for any indicator. Available data: {}, Required: 52", dataSize);
        return;
    }

    // Perform SMA calculation and Standard Deviation
    double sma = technicalIndicators.calculateSMA(closes.toVector(), smaPeriod);
    double stdDev = technicalIndicators.calculateStdDev(closes.toVector(), static_cast<int>(dataSize - smaPeriod), static_cast<int>(dataSize), sma);

    if (stdDev < 0.0001) {
        spdlog::warn("Low volatility detected (stdDev: {}), skipping trading decisions.", stdDev);
        return;
    }

    if (stdDev > 0.001) {
        spdlog::warn("High volatility detected (stdDev: {}), adjusting stop-loss level.", stdDev);
        stopLossMultiplier *= STOP_LOSS_MULTIPLIER_ADJUST;
    }

    // Ichimoku Indicator Calculation
    if (highsWindow.size() >= 9) {
        try {
            double tenkanSen = technicalIndicators.calculateTenkanSen(highsWindow.toVector(), lowsWindow.toVector(), 9, highsWindow.size() - 1, memo);
            tenkanS.push_back(tenkanSen);
            spdlog::info("Tenkan-Sen calculated: {}", tenkanSen);
        } catch (const std::exception& e) {
            spdlog::error("Tenkan-Sen Calculation Error: {}", e.what());
        }
    }

    if (highsWindow.size() >= 26) {
        try {
            double kijunSen = technicalIndicators.calculateKijunSen(highsWindow.toVector(), lowsWindow.toVector(), 26, highsWindow.size() - 1, memo);
            kijunS.push_back(kijunSen);
            spdlog::info("Kijun-Sen calculated: {}", kijunSen);
        } catch (const std::exception& e) {
            spdlog::error("Kijun-Sen Calculation Error: {}", e.what());
        }
    }

    if (highsWindow.size() >= 52 && lowsWindow.size() >= 52) {
        try {
            double senkouAVal = technicalIndicators.calculateSenkouSpanA(highsWindow.toVector(), lowsWindow.toVector(), highsWindow.size() - 1, memo);
            double senkouBVal = technicalIndicators.calculateSenkouSpanB(highsWindow.toVector(), lowsWindow.toVector(), 52, highsWindow.size() - 1, memo);
            senkouA.push_back(senkouAVal);
            senkouB.push_back(senkouBVal);
            spdlog::info("Senkou Span A: {}, Senkou Span B: {}", senkouAVal, senkouBVal);
        } catch (const std::exception& e) {
            spdlog::error("Senkou Span Calculation Error: {}", e.what());
        }
    }

    // Record portfolio balance
    recordPortfolioBalance();
}

// Helper function to update indicator vectors
void TradingStrategy::updateIndicatorVector(std::vector<double>& vec, double newValue, size_t maxSize) {
    vec.push_back(newValue);
    if (vec.size() > maxSize) {
        vec.erase(vec.begin());
    }
}

// Function to get trades
std::vector<Trade> TradingStrategy::getTrades() const {
    std::vector<Trade> trades;
    for (const auto& signal : signals) {
        if (signal.sell || signal.buy) {
            trades.emplace_back(signal.entryPrice, signal.exitPrice, signal.buy);
        }
    }
    return trades;
}

double TradingStrategy::calculateStandardDeviation(const SlidingWindow& data, size_t index, size_t period) const {
    return technicalIndicators.calculateStandardDeviation(data, index, period);
}