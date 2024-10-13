#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include "PerformanceAssessor.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <omp.h>

// Constructor Implementation
TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier)
        : accountBalance(initialBalance),
          riskPerTrade(riskPerTrade),
          stopLossMultiplier(stopLossMultiplier),
          smaPeriod(50),
          bollingerBandsPeriod(20),
          bollingerBandsMultiplier(2.0),
          hasLoggedInsufficientData(false),  // Initialize this field
          inPosition(false),
          highsWindow(52),
          lowsWindow(52),
          closes(52),
          tenkanWindow(9),  // Initialize SlidingWindow with the size of 9
          kijunWindow(26)   // Initialize SlidingWindow with the size of 26
{
    portfolioBalanceHistory.push_back(accountBalance);
}

// Function to record the current portfolio balance
void TradingStrategy::recordPortfolioBalance() {
    portfolioBalanceHistory.push_back(accountBalance);
}

// ATR Calculation
double TradingStrategy::calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex) {
    if (currentIndex < period) {
        throw std::invalid_argument("Not enough data points to calculate ATR.");
    }

    double sumTrueRange = 0.0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double highLowRange = highs[i] - lows[i];
        double highClosePrevRange = std::abs(highs[i] - closes[i - 1]);
        double lowClosePrevRange = std::abs(lows[i] - closes[i - 1]);
        double trueRange = std::max(std::max(highLowRange, highClosePrevRange), lowClosePrevRange);
        sumTrueRange += trueRange;
    }
    return sumTrueRange / period;
}

// Evaluate signals
std::vector<TradingSignal> TradingStrategy::evaluateSignals() {
    signals.clear();
    double entryPrice = 0.0;
    double positionSize = 0.0;
    int atrPeriod = 14;

    std::vector<double> closesVector = closes.toVector();  // Convert to std::vector

    // Check if there's enough data for all indicators
    if (closes.size() < smaPeriod || highsWindow.size() < smaPeriod || lowsWindow.size() < smaPeriod ||
        tenkanS.size() < 26 || kijunS.size() < 26 || senkouA.size() < 52 || senkouB.size() < 52 ||
        lowerBB.size() < bollingerBandsPeriod || upperBB.size() < bollingerBandsPeriod) {
        if (!hasLoggedInsufficientData) {
            std::cerr << "Insufficient data to evaluate signals." << std::endl;
            hasLoggedInsufficientData = true;
        }
        return signals;
    }

    size_t minSize = std::min({
                                      closes.size(),
                                      highsWindow.size(),
                                      lowsWindow.size(),
                                      tenkanS.size(),
                                      kijunS.size(),
                                      senkouA.size(),
                                      senkouB.size(),
                                      lowerBB.size(),
                                      upperBB.size()
                              });

    size_t startingIndex = std::max({
                                            static_cast<size_t>(52),
                                            static_cast<size_t>(26),
                                            static_cast<size_t>(9),
                                            static_cast<size_t>(atrPeriod)
                                    });

    if (minSize <= startingIndex) {
        std::cerr << "Insufficient data to evaluate signals. MinSize: " << minSize << ", StartingIndex: " << startingIndex << std::endl;
        return signals;
    }

    for (size_t i = startingIndex; i < minSize; ++i) {
        TradingSignal signal;
        signal.index = static_cast<int>(i);

        double atr;
        try {
            std::vector<double> highsVector = highsWindow.toVector();
            std::vector<double> lowsVector = lowsWindow.toVector();

            atr = calculateATR(highsVector, lowsVector, closesVector, atrPeriod, i);
        } catch (const std::invalid_argument& e) {
            std::cerr << "ATR Calculation Error at index " << i << ": " << e.what() << std::endl;
            continue;
        }

        std::cout << "Index: " << i << ", ATR: " << atr << std::endl;

        if (closesVector[i] > senkouA[i] && closesVector[i] > senkouB[i] && closesVector[i] > lowerBB[i] && !inPosition) {
            signal.buy = true;
            positionSize = accountBalance * riskPerTrade;
            entryPrice = closesVector[i];
            inPosition = true;

            signal.stopLossLevel = entryPrice - (atr * stopLossMultiplier);
            signal.entryPrice = entryPrice;
        }

        if (inPosition && (closesVector[i] < senkouA[i] || closesVector[i] < senkouB[i] || closesVector[i] < upperBB[i])) {
            signal.sell = true;
            signal.exitPrice = closesVector[i];
            signal.profit = (signal.exitPrice - entryPrice) * positionSize;
            accountBalance += signal.profit;
            inPosition = false;

            std::cout << "Sell signal generated at index " << i
                      << ": Exit Price = " << signal.exitPrice
                      << ", Profit = " << signal.profit << std::endl;

            recordPortfolioBalance();
        }

        if (signal.buy || signal.sell) {
            signal.positionSize = positionSize;
            signals.push_back(signal);
        }
    }

    return signals;
}

void TradingStrategy::onNewData(double high, double low, double close) {
    highsWindow.addDataPoint(high);
    lowsWindow.addDataPoint(low);
    closes.addDataPoint(close);

    size_t dataSize = closes.size();  // Data size for all calculations

    // Simplified data checks
    if (dataSize < smaPeriod) {
        std::cerr << "Insufficient data for SMA calculation. Available data: " << dataSize << ", Required: " << smaPeriod << std::endl;
        return;
    }

    if (dataSize < 52) {
        std::cerr << "Insufficient data for Senkou Span calculation. Available data: " << dataSize << ", Required: 52" << std::endl;
        return;
    }

    // Perform SMA calculation
    double sma = technicalIndicators.calculateSMA(closes.toVector(), smaPeriod);
    double stdDev = technicalIndicators.calculateStdDev(closes.toVector(), dataSize - smaPeriod, dataSize, sma);
    auto [lowerBand, upperBand] = technicalIndicators.calculateBollingerBandsWithMemoization(closes.toVector(), bollingerBandsPeriod, stdDev, bbMemo);
    lowerBB.push_back(lowerBand);
    upperBB.push_back(upperBand);

    // Perform Tenkan-Sen and Kijun-Sen calculations
    size_t currentIndex = highsWindow.size() - 1;
    try {
        double tenkanSen = technicalIndicators.calculateTenkanSen(highsWindow.toVector(), lowsWindow.toVector(), 9, currentIndex, memo);
        tenkanS.push_back(tenkanSen);

        if (highsWindow.size() >= 26 && lowsWindow.size() >= 26) {
            double kijunSen = technicalIndicators.calculateKijunSen(highsWindow.toVector(), lowsWindow.toVector(), 26, currentIndex, memo);
            kijunS.push_back(kijunSen);
        }

        if (highsWindow.size() >= 52 && lowsWindow.size() >= 52) {
            double senkouSpanA = technicalIndicators.calculateSenkouSpanA(tenkanS, kijunS, currentIndex, memo);
            double senkouSpanB = technicalIndicators.calculateSenkouSpanB(highsWindow.toVector(), lowsWindow.toVector(), 26, currentIndex, memo);
            senkouA.push_back(senkouSpanA);
            senkouB.push_back(senkouSpanB);
        }
    } catch (const std::out_of_range& e) {
        std::cerr << "Indicator Calculation Error: " << e.what() << std::endl;
    }
}

// Function to get trades
std::vector<Trade> TradingStrategy::getTrades() const {
    std::vector<Trade> trades;
    for (const auto& signal : signals) {
        if (signal.sell || signal.buy) {
            trades.push_back(Trade(signal.entryPrice, signal.exitPrice, signal.buy));
        }
    }
    return trades;
}
