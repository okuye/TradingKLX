#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include "PerformanceAssessor.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <omp.h>

// Constructor Implementation
TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod)
        : accountBalance(initialBalance),
          riskPerTrade(riskPerTrade),
          stopLossMultiplier(stopLossMultiplier),
          smaPeriod(50),
          bollingerBandsPeriod(20),
          bollingerBandsMultiplier(2.0),
          atrPeriod(atrPeriod),  // Initialize atrPeriod here
          hasLoggedInsufficientData(false),
          inPosition(false),
          highsWindow(52),
          lowsWindow(52),
          closes(52),
          tenkanWindow(9),
          kijunWindow(26),
          insufficientDataLogged(false) {
    portfolioBalanceHistory.push_back(accountBalance);
}

// Function to record the current portfolio balance
void TradingStrategy::recordPortfolioBalance() {
    portfolioBalanceHistory.push_back(accountBalance);
}

// ATR Calculation with memoization (Optimization)
double TradingStrategy::calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex) {
    if (currentIndex < period) {
        throw std::invalid_argument("Not enough data points to calculate ATR.");
    }

    double sumTrueRange = 0.0;

    // ATR calculation with memoization or caching logic can be introduced here
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        double highLowRange = highs[i] - lows[i];
        double highClosePrevRange = std::abs(highs[i] - closes[i - 1]);
        double lowClosePrevRange = std::abs(lows[i] - closes[i - 1]);
        double trueRange = std::max({highLowRange, highClosePrevRange, lowClosePrevRange});
        sumTrueRange += trueRange;
    }

    return sumTrueRange / period;
}

std::vector<TradingSignal> TradingStrategy::evaluateSignals() {
    signals.clear();
    double entryPrice = 0.0;
    double positionSize = 0.0;
    int atrPeriod = 14;

    std::vector<double> closesVector = closes.toVector();  // Convert to std::vector

    if (closes.size() < 52 || highsWindow.size() < 52 || lowsWindow.size() < 52 ||
        tenkanS.size() < 26 || kijunS.size() < 26 || senkouA.size() < 52 || senkouB.size() < 52 ||
        lowerBB.size() < 20 || upperBB.size() < 20) {
        std::cerr << "Insufficient data to evaluate signals." << std::endl;
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

    // Parallel execution for performance optimization
#pragma omp parallel for
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

        // Calculate standard deviation
        double sma = technicalIndicators.calculateSMA(closesVector, smaPeriod);
        double stdDev = technicalIndicators.calculateStdDev(closesVector, i - smaPeriod, i, sma);

        // Skip trading when volatility (stdDev) is too low (e.g., < 0.0001)
        if (stdDev < 0.0001) {
            std::cerr << "Low volatility at index " << i << ", skipping signal generation." << std::endl;
            continue;
        }

        std::cout << "Index: " << i << ", ATR: " << atr << ", StdDev: " << stdDev << std::endl;

        // Combined conditions for buy signal
        if (closesVector[i] > senkouA[i] && closesVector[i] > senkouB[i] && closesVector[i] > lowerBB[i] && !inPosition) {
            signal.buy = true;
            positionSize = accountBalance * riskPerTrade;
            entryPrice = closesVector[i];
            inPosition = true;

            signal.stopLossLevel = entryPrice - (atr * stopLossMultiplier);
            signal.entryPrice = entryPrice;
        }

        // Combined conditions for sell signal
        if (inPosition && (closesVector[i] < senkouA[i] && closesVector[i] < senkouB[i] && closesVector[i] < upperBB[i])) {
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

    size_t dataSize = closes.size();

    if (dataSize < 52) {
        if (!insufficientDataLogged) {
            std::cerr << "Insufficient data for indicators. Available data: " << dataSize << ", Required: 52" << std::endl;
            insufficientDataLogged = true;
        }
        return;
    }

    insufficientDataLogged = false;

    bool enoughDataForIchimoku = (highsWindow.size() >= 52 && lowsWindow.size() >= 52 && tenkanS.size() >= 26);
    bool enoughDataForBollinger = (lowerBB.size() >= 20 && upperBB.size() >= 20);

    if (!enoughDataForIchimoku && !enoughDataForBollinger) {
        if (!insufficientDataLogged) {
            std::cerr << "Insufficient data for all indicators. Available data: " << dataSize << ", Required: 52" << std::endl;
            insufficientDataLogged = true;
        }
        return;
    }

    // Perform SMA calculation
    double sma = technicalIndicators.calculateSMA(closes.toVector(), smaPeriod);
    double stdDev = technicalIndicators.calculateStdDev(closes.toVector(), dataSize - smaPeriod, dataSize, sma);  // Calculate standard deviation

    // Use standard deviation for decision making
    const double lowVolatilityThreshold = 0.0001;  // You can adjust this value based on your strategy
    const double highVolatilityThreshold = 0.001;  // Adjust based on your market experience

    if (stdDev < lowVolatilityThreshold) {
        std::cerr << "Low volatility detected (stdDev: " << stdDev << "), skipping trading decisions." << std::endl;
        return;  // Skip signal generation in flat market
    }

    if (stdDev > highVolatilityThreshold) {
        std::cerr << "High volatility detected (stdDev: " << stdDev << "), adjusting stop-loss level." << std::endl;
        // You can increase the stop-loss buffer here based on volatility, for example
        stopLossMultiplier *= 1.2;  // This is an arbitrary example, adjust based on your requirements
    }

    auto [lowerBand, upperBand] = technicalIndicators.calculateBollingerBandsWithMemoization(closes.toVector(), bollingerBandsPeriod, bollingerBandsMultiplier, bbMemo);
    lowerBB.push_back(lowerBand);
    upperBB.push_back(upperBand);

    // Perform Tenkan-Sen and Kijun-Sen calculations
    size_t tenkanSenPeriod = 9;
    size_t kijunSenPeriod = 26;

    if (highsWindow.size() < tenkanSenPeriod || lowsWindow.size() < tenkanSenPeriod) {
        static bool tenkanSenLogged = false;
        if (!tenkanSenLogged) {
            std::cerr << "Insufficient data for Tenkan-Sen calculation. Available data: " << highsWindow.size() << ", Required: " << tenkanSenPeriod << std::endl;
            tenkanSenLogged = true;
        }
        return;
    }

    size_t currentIndex = highsWindow.size() - 1;

    try {
        double tenkanSen = technicalIndicators.calculateTenkanSen(highsWindow.toVector(), lowsWindow.toVector(), tenkanSenPeriod, currentIndex, memo);
        tenkanS.push_back(tenkanSen);
    } catch (const std::out_of_range& e) {
        std::cerr << "Tenkan-Sen Calculation Error: " << e.what() << std::endl;
    }

    // Kijun-Sen calculation
    if (highsWindow.size() >= kijunSenPeriod && lowsWindow.size() >= kijunSenPeriod) {
        try {
            double kijunSen = technicalIndicators.calculateKijunSen(highsWindow.toVector(), lowsWindow.toVector(), kijunSenPeriod, currentIndex, memo);
            kijunS.push_back(kijunSen);
        } catch (const std::out_of_range& e) {
            std::cerr << "Kijun-Sen Calculation Error: " << e.what() << std::endl;
        }
    }

    // Senkou Span A and B calculation
    if (highsWindow.size() >= 52 && lowsWindow.size() >= 52) {
        try {
            double senkouSpanA = technicalIndicators.calculateSenkouSpanA(tenkanS, kijunS, currentIndex, memo);
            double senkouSpanB = technicalIndicators.calculateSenkouSpanB(highsWindow.toVector(), lowsWindow.toVector(), kijunSenPeriod, currentIndex, memo);
            senkouA.push_back(senkouSpanA);
            senkouB.push_back(senkouSpanB);
        } catch (const std::out_of_range& e) {
            std::cerr << "Senkou Span Calculation Error: " << e.what() << std::endl;
        }
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
