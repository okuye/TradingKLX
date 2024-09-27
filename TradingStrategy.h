#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include <vector>
#include "PriceData.h"
#include "TechnicalIndicators.h"
#include "SlidingWindow.h"
#include "TradingSignal.h"

class TradingStrategy {
private:
    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;
    int smaPeriod;
    int bollingerBandsPeriod;  // Declare the Bollinger Bands period
    double bollingerBandsMultiplier;  // Declare the Bollinger Bands multiplier
    std::vector<TradingSignal> signals;

    // Memoization objects
    IchimokuMemo memo;
    BollingerBandsMemo bbMemo;

public:
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier);

    void onNewData(double high, double low, double close);
    std::vector<TradingSignal> evaluateSignals(const std::vector<PriceData> &priceData,
                                               const std::vector<double> &closes,
                                               const std::vector<double> &highs,
                                               const std::vector<double> &lows,
                                               const std::vector<double> &tenkanS,
                                               const std::vector<double> &kijunS,
                                               const std::vector<double> &senkouA,
                                               const std::vector<double> &senkouB,
                                               const std::vector<double> &lowerBB,
                                               const std::vector<double> &upperBB);
    double getAccountBalance() const;
    const std::vector<TradingSignal>& getSignals() const;
    std::vector<TradingSignal> getTradingSignals() const;
};

#endif  // TRADING_STRATEGY_H