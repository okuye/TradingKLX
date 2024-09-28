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
    int bollingerBandsPeriod;
    double bollingerBandsMultiplier;
    std::vector<TradingSignal> signals;
    IchimokuMemo memo;
    BollingerBandsMemo bbMemo;
    bool hasLoggedInsufficientData;

public:
    // Constructor
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier);

    void onNewData(double high, double low, double close);
    std::vector<TradingSignal> evaluateSignals(const std::vector<PriceData>& priceData,
                                               const std::vector<double>& closes,
                                               const std::vector<double>& highs,
                                               const std::vector<double>& lows,
                                               const std::vector<double>& tenkanS,
                                               const std::vector<double>& kijunS,
                                               const std::vector<double>& senkouA,
                                               const std::vector<double>& senkouB,
                                               const std::vector<double>& lowerBB,
                                               const std::vector<double>& upperBB);

    // New getters and setters
    double getAccountBalance() const;
    void setAccountBalance(double balance);

    double getRiskPerTrade() const;
    void setRiskPerTrade(double risk);

    double getStopLossMultiplier() const;
    void setStopLossMultiplier(double multiplier);

    const std::vector<TradingSignal>& getSignals() const;
    void addSignal(const TradingSignal& signal);
};

#endif  // TRADING_STRATEGY_H
