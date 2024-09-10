#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include <vector>
#include "TradingSignal.h"
#include "PriceData.h"

class TradingStrategy {
public:
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier);

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

    double getAccountBalance() const;

    // Add this method to retrieve the signals
    const std::vector<TradingSignal>& getSignals() const;

private:
    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;
    int smaPeriod;
    int bollingerBandsPeriod;
    double bollingerBandsMultiplier;

    std::vector<TradingSignal> signals; // Add this to store generated signals
};

#endif
