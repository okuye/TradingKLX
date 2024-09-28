#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include <iostream>
#include <vector>
#include "SlidingWindow.h"
#include "PriceData.h"
#include <fstream>
#include <cstring>  // for strerror
#include <unistd.h> // for getcwd


// Initialize windows for highs, lows, and closes
SlidingWindow tenkanWindow(9);  // Tenkan-sen uses 9 periods
SlidingWindow kijunWindow(26);  // Kijun-sen uses 26 periods
SlidingWindow highsWindow(52);  // For Senkou Span B
SlidingWindow lowsWindow(52);

TechnicalIndicators technicalIndicators;

// Constructor: Initializes the strategy with initial balance, risk per trade, and stop-loss multiplier
TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier)
        : accountBalance(initialBalance), riskPerTrade(riskPerTrade), stopLossMultiplier(stopLossMultiplier) {
    smaPeriod = 50;  // Default value for SMA period
    bollingerBandsPeriod = 20;  // Default value for Bollinger Bands period
    bollingerBandsMultiplier = 2;  // Default value for Bollinger Bands multiplier
}

// Function to process new incoming data points and calculate technical indicators
void TradingStrategy::onNewData(double high, double low, double close) {
    // Add the new data to the sliding windows for highs and lows
    highsWindow.addDataPoint(high);
    lowsWindow.addDataPoint(low);

    // Debugging: Print the current size of the windows
    // std::cout << "HighsWindow size: " << highsWindow.size() << ", LowsWindow size: " << lowsWindow.size() << std::endl;

    // Ensure that there is enough data to calculate the indicators
    if (highsWindow.size() < 52 || lowsWindow.size() < 52) {
        std::cerr << "Not enough data for Senkou Span B calculation. Current size: " << highsWindow.size() << std::endl;
        return;  // Early exit until sufficient data is available
    }

    // Convert deque to vector for Tenkan-sen and Kijun-sen calculations
    std::vector<double> highsVector(highsWindow.getData().begin(), highsWindow.getData().end());
    std::vector<double> lowsVector(lowsWindow.getData().begin(), lowsWindow.getData().end());

    // Log vector sizes to ensure they are being populated correctly
    std::cout << "HighsVector size: " << highsVector.size() << ", LowsVector size: " << lowsVector.size() << std::endl;

    // Calculate Tenkan-sen if there are at least 9 periods of data
    if (highsWindow.size() >= 9) {
        double tenkan = technicalIndicators.calculateTenkanSen(highsVector, lowsVector, 9, highsWindow.size() - 1, memo);
        tenkanWindow.addDataPoint(tenkan);
    }

    // Calculate Kijun-sen if there are at least 26 periods of data
    if (highsWindow.size() >= 26) {
        double kijun = technicalIndicators.calculateKijunSen(highsVector, lowsVector, 26, highsWindow.size() - 1, memo);
        kijunWindow.addDataPoint(kijun);
    }

    // Calculate Senkou Span A and Senkou Span B when enough data is available
    if (tenkanWindow.size() >= 9 && kijunWindow.size() >= 26) {
        double senkouSpanA = technicalIndicators.calculateSenkouSpanA(tenkanWindow, kijunWindow, memo);
        double senkouSpanB = technicalIndicators.calculateSenkouSpanB(highsWindow, lowsWindow, memo);

        std::cout << "Senkou Span A: " << senkouSpanA << ", Senkou Span B: " << senkouSpanB << std::endl;
    }
}




std::vector<TradingSignal> TradingStrategy::evaluateSignals(const std::vector<PriceData>& priceData,
                                                            const std::vector<double>& closes,
                                                            const std::vector<double>& highs,
                                                            const std::vector<double>& lows,
                                                            const std::vector<double>& tenkanS,
                                                            const std::vector<double>& kijunS,
                                                            const std::vector<double>& senkouA,
                                                            const std::vector<double>& senkouB,
                                                            const std::vector<double>& lowerBB,
                                                            const std::vector<double>& upperBB) {
    signals.clear();  // Clear previous signals
    bool inPosition = false;
    double entryPrice = 0.0;
    double positionSize = 0.0;
    int atrPeriod = 14;
    int senkouSpanPeriod = 52;

    // Log sizes for debugging
    std::cout << "Data sizes - closes: " << closes.size()
              << ", highs: " << highs.size() << ", lows: " << lows.size()
              << ", tenkanS: " << tenkanS.size() << ", kijunS: " << kijunS.size()
              << ", lowerBB: " << lowerBB.size() << ", upperBB: " << upperBB.size()
              << ", priceData: " << priceData.size() << std::endl;

// Check that all vectors have the required sizes
    if (priceData.size() < 52 || closes.size() < 52 || highs.size() < 52 || lows.size() < 52 ||
        tenkanS.size() < 9 || kijunS.size() < 26 || senkouA.size() < 52 || senkouB.size() < 52) {
        std::cerr << "Insufficient data to evaluate signals." << std::endl;
        return signals;
    }



    // Main loop to evaluate signals
    for (std::size_t i = senkouSpanPeriod; i < priceData.size(); ++i) {
        TradingSignal signal;
        signal.index = static_cast<int>(i);

        // Buy condition
        if (closes[i] > senkouA[i] && closes[i] > senkouB[i] && closes[i] > lowerBB[i] && !inPosition) {
            signal.buy = true;
            positionSize = accountBalance * riskPerTrade;
            entryPrice = closes[i];
            inPosition = true;
        }

        // Sell condition
        if (closes[i] < senkouA[i] && closes[i] < senkouB[i] && closes[i] < upperBB[i] && inPosition) {
            signal.sell = true;
            signal.profit = (closes[i] - entryPrice) * positionSize;
            accountBalance += signal.profit;
            inPosition = false;
        }

        // Push signals if a trade action occurred
        if (signal.buy || signal.sell) {
            signals.push_back(signal);
        }
    }

    return signals;
}



// Get current account balance
double TradingStrategy::getAccountBalance() const {
    return accountBalance;
}

// Set account balance
void TradingStrategy::setAccountBalance(double balance) {
    accountBalance = balance;
}

// Set risk per trade
void TradingStrategy::setRiskPerTrade(double risk) {
    riskPerTrade = risk;
}

// Set stop-loss multiplier
void TradingStrategy::setStopLossMultiplier(double multiplier) {
    stopLossMultiplier = multiplier;
}

// Return evaluated trading signals
const std::vector<TradingSignal> &TradingStrategy::getSignals() const {
    return signals;
}

// Add a signal manually for testing purposes
void TradingStrategy::addSignal(const TradingSignal& signal) {
    signals.push_back(signal);
}