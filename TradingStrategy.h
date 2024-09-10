#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include <vector>
#include "PriceData.h"
#include "TechnicalIndicators.h"
#include "TradingSignal.h"

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

    // Getter method for account balance
    double getAccountBalance() const;

private:
    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;

    int smaPeriod;
    int bollingerBandsPeriod;
    int bollingerBandsMultiplier;

    TechnicalIndicators indicators;

    void loadConfiguration(const std::string& configFile);
};

#endif