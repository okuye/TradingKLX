#ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H

#include <vector>
#include "TechnicalIndicators.h" // Include the TechnicalIndicators class
#include "PriceData.h" // Assume a PriceData struct is defined elsewhere

// Structure to hold trading signals
struct TradingSignal {
    bool buy;
    bool sell;
    size_t index; // Position in the dataset
    double positionSize;
    double stopLossLevel;
};

class TradingStrategy {
public:
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier);
    std::vector<TradingSignal> evaluateSignals(const std::vector<PriceData>& priceData);

private:
    TechnicalIndicators indicators;
    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;
    int smaPeriod;
    int bollingerBandsPeriod;
    double bollingerBandsMultiplier;

    void loadConfiguration(const std::string& configFile);
};

#endif // TRADINGSTRATEGY_H
