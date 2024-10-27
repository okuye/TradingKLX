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
    bool tenkanSenLogged = false;
    bool kijunSenLogged = false;
    bool senkouLogged = false;

    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;

    int smaPeriod;
    int bollingerBandsPeriod;
    double bollingerBandsMultiplier;

    bool inPosition;
    bool hasLoggedInsufficientData;

    SlidingWindow highsWindow;
    SlidingWindow lowsWindow;
    SlidingWindow closes;

    SlidingWindow tenkanWindow;
    SlidingWindow kijunWindow;

    std::vector<double> lowerBB;
    std::vector<double> upperBB;

    std::vector<double> tenkanS;
    std::vector<double> kijunS;
    std::vector<double> senkouA;
    std::vector<double> senkouB;

    std::vector<TradingSignal> signals;
    std::vector<double> portfolioBalanceHistory;

    BollingerBandsMemo bbMemo;
    IchimokuMemo ichimokuMemo;  // Fixed variable name
    TechnicalIndicators technicalIndicators;

    void recordPortfolioBalance();
    int atrPeriod;
    double positionSize;

public:
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier, int atrPeriod);

    void setAtrPeriod(int period) {
        atrPeriod = period;
    }

    void onNewData(double high, double low, double close);
    std::vector<TradingSignal> evaluateSignals();

    std::vector<Trade> getTrades() const;

    double calculatePositionSize() const;
    double calculateExitPrice(size_t index);
    double calculateStandardDeviation(const SlidingWindow& data, size_t index, size_t period) const;
    void updateIndicatorVector(std::vector<double>& vec, double newValue, size_t maxSize = 100);
};

#endif // TRADING_STRATEGY_H
