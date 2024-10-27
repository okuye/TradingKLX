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
const double HIGH_VOLATILITY_THRESHOLD = 0.05;
const double STOP_LOSS_MULTIPLIER_ADJUST = 1.2;
const double STOP_LOSS_MULTIPLIER_DEFAULT = 2.0;
const double TAKE_PROFIT_MULTIPLIER = 3.0;
const double DEFAULT_POSITION_SIZE = 1000.0;

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
    if (index >= closes.size()) {
        spdlog::error("Index out of range in calculateExitPrice: {}", index);
        throw std::out_of_range("Index out of range in calculateExitPrice");
    }
    std::vector<double> highs = highsWindow.toVector();
    std::vector<double> lows = lowsWindow.toVector();
    std::vector<double> closesVec = closes.toVector();
    double atr = technicalIndicators.calculateATR(highs, lows, closesVec, atrPeriod, static_cast<int>(index));
    double currentPrice = closesVec.at(index);
    double exitPrice = currentPrice + atr;
    spdlog::info("Calculated exit price at index {}: {}", index, exitPrice);
    return exitPrice;
}

// Function to calculate position size
double TradingStrategy::calculatePositionSize() const {
    return DEFAULT_POSITION_SIZE;
}

// Function to evaluate signals and generate trades
std::vector<TradingSignal> TradingStrategy::evaluateSignals() {
    std::vector<TradingSignal> generatedSignals;
    spdlog::debug("Evaluating signals...");

    if (closes.size() < 52 || highsWindow.size() < 52 || lowsWindow.size() < 52) {
        if (!insufficientDataLogged) {
            spdlog::warn("Insufficient data to evaluate signals. Required: 52, Available: {}", closes.size());
            insufficientDataLogged = true;
        }
        return generatedSignals;
    }

    if (lowerBB.size() < static_cast<size_t>(bollingerBandsPeriod) || upperBB.size() < static_cast<size_t>(bollingerBandsPeriod)) {
        if (!insufficientDataLogged) {
            spdlog::warn("Insufficient data for Bollinger Bands. Skipping signal evaluation.");
            insufficientDataLogged = true;
        }
        return generatedSignals;
    }

    spdlog::info("Data sufficient, proceeding with signal evaluation.");
    size_t latestIndex = closes.size() - 1;
    TradingSignal signal;
    try {
        signal.entryPrice = closes.at(latestIndex);
        spdlog::info("Entry price set at index {}: {}", latestIndex, signal.entryPrice);
        double volatility = technicalIndicators.calculateStandardDeviation(closes, latestIndex, std::min(size_t(20), closes.size()));

        signal.stopLossLevel = (volatility > HIGH_VOLATILITY_THRESHOLD)
                               ? signal.entryPrice - (volatility * STOP_LOSS_MULTIPLIER_ADJUST * STOP_LOSS_MULTIPLIER_DEFAULT)
                               : signal.entryPrice - (volatility * STOP_LOSS_MULTIPLIER_DEFAULT);

        signal.exitPrice = calculateExitPrice(latestIndex);
        signal.profit = (signal.exitPrice - signal.entryPrice) * calculatePositionSize();
        signal.buy = signal.profit > 0;
        signal.sell = !signal.buy;
        generatedSignals.push_back(signal);

        spdlog::info("Generated signal - Entry: {}, Exit: {}, Profit: {}, Buy: {}, Sell: {}",
                     signal.entryPrice, signal.exitPrice, signal.profit, signal.buy, signal.sell);
    } catch (const std::exception& e) {
        spdlog::error("Error generating signal: {}", e.what());
    }

    signals.insert(signals.end(), generatedSignals.begin(), generatedSignals.end());
    return generatedSignals;
}

// Refined onNewData function with conditional logging
void TradingStrategy::onNewData(double high, double low, double close) {
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

    try {
        auto [lowerBand, upperBand] = technicalIndicators.calculateBollingerBandsWithMemoization(
                closes.toVector(), bollingerBandsPeriod, bollingerBandsMultiplier, bbMemo, static_cast<int>(dataSize - 1)
        );
        lowerBB.push_back(lowerBand);
        upperBB.push_back(upperBand);
        spdlog::info("Bollinger Bands - Lower: {}, Upper: {}", lowerBand, upperBand);
    } catch (const std::exception& e) {
        spdlog::error("Error calculating Bollinger Bands: {}", e.what());
        return;
    }

    if (highsWindow.size() >= 9) {
        try {
            double tenkanSen = technicalIndicators.calculateTenkanSen(highsWindow.toVector(), lowsWindow.toVector(), 9, highsWindow.size() - 1, ichimokuMemo);
            tenkanS.push_back(tenkanSen);
            spdlog::info("Tenkan-Sen: {}", tenkanSen);
        } catch (const std::exception& e) {
            spdlog::error("Tenkan-Sen Calculation Error: {}", e.what());
        }
    }

    if (highsWindow.size() >= 26) {
        try {
            double kijunSen = technicalIndicators.calculateKijunSen(highsWindow.toVector(), lowsWindow.toVector(), 26, highsWindow.size() - 1, ichimokuMemo);
            kijunS.push_back(kijunSen);
            spdlog::info("Kijun-Sen: {}", kijunSen);
        } catch (const std::exception& e) {
            spdlog::error("Kijun-Sen Calculation Error: {}", e.what());
        }
    }

    if (highsWindow.size() >= 52 && lowsWindow.size() >= 52) {
        try {
            double senkouAVal = technicalIndicators.calculateSenkouSpanA(highsWindow.toVector(), lowsWindow.toVector(), highsWindow.size() - 1, ichimokuMemo);
            double senkouBVal = technicalIndicators.calculateSenkouSpanB(highsWindow.toVector(), lowsWindow.toVector(), 52, highsWindow.size() - 1, ichimokuMemo);
            senkouA.push_back(senkouAVal);
            senkouB.push_back(senkouBVal);
            spdlog::info("Senkou Span A: {}, Senkou Span B: {}", senkouAVal, senkouBVal);
        } catch (const std::exception& e) {
            spdlog::error("Senkou Span Calculation Error: {}", e.what());
        }
    }
    recordPortfolioBalance();
}

// Function to get trades from generated signals
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
