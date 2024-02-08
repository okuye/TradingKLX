#ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H

#include <vector>
#include "TechnicalIndicators.h"  // Include the TechnicalIndicators class
#include "PriceData.h"            // Assume this contains the definition of PriceData

// Structure to hold trading signals
struct TradingSignal {
    bool buy;
    bool sell;
    size_t index;  // Position in the dataset
};

class TradingStrategy {
public:
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier);
    
    // Evaluates trading signals based on price data and technical indicators
    std::vector<TradingSignal> evaluateSignals(const std::vector<PriceData>& priceData);

private:
    TechnicalIndicators indicators;  // Object to access technical indicators
    double accountBalance;           // Trader's account balance
    double riskPerTrade;             // Risk allocated per trade
    double stopLossMultiplier;       // Multiplier to determine stop-loss level

    // Private methods for risk management, signal evaluation, etc., if needed
};

#endif // TRADINGSTRATEGY_H
