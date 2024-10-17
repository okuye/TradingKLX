#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include "Trade.h"
#include <vector>
#include <deque>
#include "PriceData.h"
#include "TechnicalIndicators.h"
#include "TradingSignal.h"
#include "SlidingWindow.h"
#include <memory>

class TradingStrategy {
private:
    bool insufficientDataLogged;
    bool tenkanSenLogged = false;  // Flag to log Tenkan-Sen insufficient data once
    bool kijunSenLogged = false;   // Flag to log Kijun-Sen insufficient data once
    bool senkouLogged = false;     // Flag to log Senkou Span insufficient data once

    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;

    int smaPeriod;
    int bollingerBandsPeriod;
    double bollingerBandsMultiplier;

    bool inPosition;
    bool hasLoggedInsufficientData; // Add this field

    SlidingWindow highsWindow;
    SlidingWindow lowsWindow;
    SlidingWindow closes;

    SlidingWindow tenkanWindow; // Add this field
    SlidingWindow kijunWindow;  // Add this field

    std::vector<double> lowerBB;
    std::vector<double> upperBB;

    std::vector<double> tenkanS;
    std::vector<double> kijunS;
    std::vector<double> senkouA;
    std::vector<double> senkouB;

    std::vector<TradingSignal> signals;
    std::vector<double> portfolioBalanceHistory;

    BollingerBandsMemo bbMemo;
    IchimokuMemo memo;
    TechnicalIndicators technicalIndicators;

    void recordPortfolioBalance();
    int atrPeriod;  // ATR period, now configurable dynamically
public:
    // Constructor
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod);

//    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod);


    // Method to set ATR period dynamically
    void setAtrPeriod(int period) {
        atrPeriod = period;
    }

    double calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int index);

    void onNewData(double high, double low, double close);
    std::vector<TradingSignal> evaluateSignals();

    std::vector<Trade> getTrades() const;
};
#endif // TRADING_STRATEGY_H
