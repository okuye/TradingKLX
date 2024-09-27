#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include <iostream>
#include <vector>
#include <cstddef>
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

// Declare technicalIndicators as an object of the class TechnicalIndicators
TechnicalIndicators technicalIndicators;

void TradingStrategy::onNewData(double high, double low, double close) {
    // Add the new data to the windows
    highsWindow.addDataPoint(high);
    lowsWindow.addDataPoint(low);

    // Debugging: Print current window sizes
//    std::cout << "HighsWindow size: " << highsWindow.size() << ", LowsWindow size: " << lowsWindow.size() << std::endl;

    // Ensure there is enough data to calculate the indicators
    if (highsWindow.size() < 52 || lowsWindow.size() < 52) {
                std::cerr << "Not enough data for Senkou Span B calculation. Current size: "
                          << highsWindow.size() << std::endl;
                return;  // Early exit until sufficient data is available
            }


    // Convert deque to vector for Tenkan-sen and Kijun-sen calculations
    std::vector<double> highsVector(highsWindow.getData().begin(), highsWindow.getData().end());
    std::vector<double> lowsVector(lowsWindow.getData().begin(), lowsWindow.getData().end());

    // Log vector sizes to ensure they are being populated
    std::cout << "HighsVector size: " << highsVector.size()
              << ", LowsVector size: " << lowsVector.size() << std::endl;

    // Calculate Tenkan-sen if we have at least 9 periods of data
    if (highsWindow.size() >= 9) {
        double tenkan = technicalIndicators.calculateTenkanSen(highsVector, lowsVector, 9, highsWindow.size() - 1, memo);
        tenkanWindow.addDataPoint(tenkan);
    }

    // Calculate Kijun-sen if we have at least 26 periods of data
    if (highsWindow.size() >= 26) {
        double kijun = technicalIndicators.calculateKijunSen(highsVector, lowsVector, 26, highsWindow.size() - 1, memo);
        kijunWindow.addDataPoint(kijun);
    }

    // Calculate Senkou Span A and B when enough data is available
    if (tenkanWindow.size() >= 9 && kijunWindow.size() >= 26) {
        double senkouSpanA = technicalIndicators.calculateSenkouSpanA(tenkanWindow, kijunWindow, memo);
        double senkouSpanB = technicalIndicators.calculateSenkouSpanB(highsWindow, lowsWindow, memo);

        std::cout << "Senkou Span A: " << senkouSpanA << ", Senkou Span B: " << senkouSpanB << std::endl;
    }
}

// Constructor: Initializes the strategy with initial balance, risk per trade, and stop-loss multiplier
TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier)
        : accountBalance(initialBalance), riskPerTrade(riskPerTrade), stopLossMultiplier(stopLossMultiplier) {
    smaPeriod = 50;  // Default value for SMA period
    bollingerBandsPeriod = 20;  // Default value for Bollinger Bands period
    bollingerBandsMultiplier = 2;  // Default value for Bollinger Bands multiplier
}



std::vector<TradingSignal> TradingStrategy::evaluateSignals(
        const std::vector<PriceData> &priceData,
        const std::vector<double> &closes,
        const std::vector<double> &highs,
        const std::vector<double> &lows,
        const std::vector<double> &tenkanS,
        const std::vector<double> &kijunS,
        const std::vector<double> &senkouA,
        const std::vector<double> &senkouB,
        const std::vector<double> &lowerBB,
        const std::vector<double> &upperBB) {

    signals.clear();  // Clear previous signals
    double entryPrice = 0.0;
    double positionSize = 0.0;
    bool inPosition = false;
    int atrPeriod = 14;
    int smaPeriod = 20;  // SMA period for generating buy/sell signals
    int senkouSpanPeriod = 52;  // Ensuring Senkou Span B data validity (updated to 52 periods)
    TechnicalIndicators indicators;  // Object to access technical indicators

    // Set the log file path to avoid permission issues, using /tmp directory
    std::string logFilePath = "/tmp/trading_signals_log.txt";

    // Open log file for writing
    std::ofstream logFile(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open " << logFilePath << " for writing" << std::endl;
        perror("Error opening file");  // Print detailed error info
        return signals;  // Exit if log file can't be opened
    }

    // Flag to ensure only one log at index 52
    static bool hasLoggedInsufficientData = false;

    // Ensure sufficient data exists before starting the loop
    if (priceData.size() < senkouSpanPeriod || closes.size() < senkouSpanPeriod ||
        highs.size() < senkouSpanPeriod || lows.size() < senkouSpanPeriod ||
        lowerBB.size() < senkouSpanPeriod || upperBB.size() < senkouSpanPeriod ||
        tenkanS.size() < 9 || kijunS.size() < 26) {
        logFile << "Insufficient data to start evaluating signals. Waiting for full data set." << std::endl;
        logFile.close();
        return signals;  // Return empty signals
    }

    // Start evaluation after the required data has been accumulated (i.e., from index senkouSpanPeriod)
    for (std::size_t i = senkouSpanPeriod; i < priceData.size(); ++i) {
        TradingSignal signal;  // Initialize an empty TradingSignal object
        signal.index = static_cast<int>(i);  // Set the index

        // Ensure enough data for Senkou Span A and B, Tenkan, Kijun, and Bollinger Bands
        if (i >= senkouA.size() || i >= senkouB.size() || i >= tenkanS.size() ||
            i >= kijunS.size() || i >= lowerBB.size() || i >= upperBB.size()) {

            // Log insufficient data at index 52 only once
            if (i == senkouSpanPeriod && !hasLoggedInsufficientData) {
                logFile << "Insufficient data for Senkou spans, Ichimoku values, or Bollinger Bands at index " << i << std::endl;
                hasLoggedInsufficientData = true;  // Set flag to avoid duplicate logs
            }
            continue;  // Skip this iteration if data is insufficient
        }

        // Log detailed information about the sizes of vectors at the current index
        logFile << "Data at index " << i << ":\n";
        logFile << "senkouA.size(): " << senkouA.size() << "\n";
        logFile << "senkouB.size(): " << senkouB.size() << "\n";
        logFile << "tenkanS.size(): " << tenkanS.size() << "\n";
        logFile << "kijunS.size(): " << kijunS.size() << "\n";
        logFile << "lowerBB.size(): " << lowerBB.size() << "\n";
        logFile << "upperBB.size(): " << upperBB.size() << "\n";

        // Example Buy Signal Logic
        if (closes[i] > indicators.calculateSMA(closes, static_cast<int>(i), smaPeriod) &&
            (tenkanS[i] > kijunS[i] && closes[i] > senkouA[i] && closes[i] > senkouB[i]) &&
            closes[i] > lowerBB[i] && !inPosition) {

            signal.buy = true;
            positionSize = accountBalance * riskPerTrade;
            signal.positionSize = positionSize;

            double atr = indicators.calculateATR(highs, lows, closes, atrPeriod, i, 0.0001);
            signal.stopLossLevel = closes[i] - (atr * stopLossMultiplier);
            entryPrice = closes[i];
            inPosition = true;

            logFile << "Buy signal generated at index " << i << "\n";
        }

        // Example Sell Signal Logic
        if (closes[i] < indicators.calculateSMA(closes, static_cast<int>(i), smaPeriod) &&
            (tenkanS[i] < kijunS[i] && closes[i] < senkouA[i] && closes[i] < senkouB[i]) &&
            closes[i] < upperBB[i] && inPosition) {

            signal.sell = true;
            signal.profit = (closes[i] - entryPrice) * positionSize;
            accountBalance += signal.profit;
            inPosition = false;

            logFile << "Sell signal generated at index " << i << "\n";
        }

        // Append signal if either buy or sell condition met
        if (signal.buy || signal.sell) {
            signals.push_back(signal);
        }
    }

    logFile.close();  // Close the log file
    return signals;
}

// Get current account balance
double TradingStrategy::getAccountBalance() const {
    return accountBalance;
}

// Return evaluated trading signals
const std::vector<TradingSignal> &TradingStrategy::getSignals() const {
    return signals;
}

// Return trading signals for further analysis
std::vector<TradingSignal> TradingStrategy::getTradingSignals() const {
    return signals;
}