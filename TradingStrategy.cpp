#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include "PerformanceAssessor.h"
#include <vector>
#include <algorithm> // For std::max and std::min
#include <iostream>

// Constructor Implementation
TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier)
        : accountBalance(initialBalance),
          riskPerTrade(riskPerTrade),
          stopLossMultiplier(stopLossMultiplier),
          smaPeriod(50),
          bollingerBandsPeriod(20),
          bollingerBandsMultiplier(2.0),
          hasLoggedInsufficientData(false),
          inPosition(false),
          highsWindow(52),
          lowsWindow(52),
          tenkanWindow(9),
          kijunWindow(26),
          closes(52)  // Initialize closes with a size of 52
{
    portfolioBalanceHistory.push_back(accountBalance);
}

// Function to record the current portfolio balance
void TradingStrategy::recordPortfolioBalance() {
    portfolioBalanceHistory.push_back(accountBalance);
}


void TradingStrategy::onNewData(double high, double low, double close) {
    highsWindow.addDataPoint(high);
    lowsWindow.addDataPoint(low);
    closes.addDataPoint(close);  // Add data to closes SlidingWindow

    if (highsWindow.size() >= 52 && lowsWindow.size() >= 52) {
        try {
            std::vector<double> highsVector = highsWindow.toVector();  // Convert to std::vector
            std::vector<double> lowsVector = lowsWindow.toVector();    // Convert to std::vector
            std::vector<double> closesVector = closes.toVector();      // Convert to std::vector

            if (highsWindow.size() >= 9) {
                double tenkan = technicalIndicators.calculateTenkanSen(highsVector, lowsVector, 9, highsWindow.size() - 1, memo);
                tenkanS.push_back(tenkan);
            }

            if (highsWindow.size() >= 26) {
                double kijun = technicalIndicators.calculateKijunSen(highsVector, lowsVector, 26, highsWindow.size() - 1, memo);
                kijunS.push_back(kijun);
            }

            if (tenkanS.size() >= 26 && kijunS.size() >= 26) {
                double senkouSpanA = technicalIndicators.calculateSenkouSpanA(highsVector, lowsVector, 52, highsWindow.size() - 1, memo);
                double senkouSpanB = technicalIndicators.calculateSenkouSpanB(highsVector, lowsVector, 52, highsWindow.size() - 1, memo);
                senkouA.push_back(senkouSpanA);
                senkouB.push_back(senkouSpanB);
            }

            if (closes.size() >= bollingerBandsPeriod) {
                std::pair<double, double> bands = technicalIndicators.calculateBollingerBandsWithMemoization(closesVector, bollingerBandsPeriod, bollingerBandsMultiplier, bbMemo);
                lowerBB.push_back(bands.first);
                upperBB.push_back(bands.second);
            }

        } catch (const std::out_of_range& e) {
            std::cerr << "Error in accessing SlidingWindow data: " << e.what() << std::endl;
        }

        std::cout << "Indicators Updated - TenkanS: " << tenkanS.size()
                  << ", KijunS: " << kijunS.size()
                  << ", SenkouA: " << senkouA.size()
                  << ", SenkouB: " << senkouB.size()
                  << ", BollingerBands: " << lowerBB.size() << std::endl;
    } else {
        if (!hasLoggedInsufficientData) {
            std::cerr << "Not enough data for Ichimoku calculations. Current size: " << highsWindow.size() << std::endl;
            hasLoggedInsufficientData = true;
        }
    }

    recordPortfolioBalance();
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

    if (startingIndex >= minSize) {
        std::cerr << "Starting index (" << startingIndex << ") >= minSize (" << minSize << "). No iterations." << std::endl;
        return signals;
    }

    for (size_t i = startingIndex; i < minSize; ++i) {
        TradingSignal signal;
        signal.index = static_cast<int>(i);

        double atr;
        try {
            std::vector<double> highsVector = highsWindow.toVector();
            std::vector<double> lowsVector = lowsWindow.toVector();

            atr = technicalIndicators.calculateATR(highsVector, lowsVector, closesVector, atrPeriod, i, 0.0001);
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

// Getter for account balance
double TradingStrategy::getAccountBalance() const {
    return accountBalance;
}

// Setter for account balance
void TradingStrategy::setAccountBalance(double balance) {
    accountBalance = balance;
    recordPortfolioBalance();
}

// Getter for risk per trade
double TradingStrategy::getRiskPerTrade() const {
    return riskPerTrade;
}

// Setter for risk per trade
void TradingStrategy::setRiskPerTrade(double risk) {
    riskPerTrade = risk;
}

// Getter for stop loss multiplier
double TradingStrategy::getStopLossMultiplier() const {
    return stopLossMultiplier;
}

// Setter for stop loss multiplier
void TradingStrategy::setStopLossMultiplier(double multiplier) {
    stopLossMultiplier = multiplier;
}

// Getter for signals
const std::vector<TradingSignal>& TradingStrategy::getSignals() const {
    return signals;
}

// Add a signal
void TradingStrategy::addSignal(const TradingSignal& signal) {
    signals.push_back(signal);
}

// Getter for portfolio balance history
const std::vector<double>& TradingStrategy::getPortfolioBalanceHistory() const {
    return portfolioBalanceHistory;
}

// Convert signals to trades
std::vector<Trade> TradingStrategy::getTrades() const {
    std::vector<Trade> trades;
    for (const auto& signal : signals) {
        Trade trade(signal.entryPrice, signal.exitPrice, signal.buy);
        trades.push_back(trade);
    }
    return trades;
}