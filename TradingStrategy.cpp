#include "TradingStrategy.h"
#include <iostream> // For debug output

TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier) 
    : accountBalance(initialBalance), riskPerTrade(riskPerTrade), stopLossMultiplier(stopLossMultiplier) {
    // Initialize indicator parameters
    smaPeriod = 50;
    bollingerBandsPeriod = 20;
    bollingerBandsMultiplier = 2;

    // Load configuration from file or environment
    loadConfiguration("config.json");
}

void TradingStrategy::loadConfiguration(const std::string& configFile) {
    // Load configuration logic here
    // For now, we're using hardcoded values
    std::cout << "Configuration loaded from " << configFile << std::endl;
}

std::vector<TradingSignal> TradingStrategy::evaluateSignals(const std::vector<PriceData>& priceData) {
    std::vector<TradingSignal> signals;
    
    for (size_t i = 1; i < priceData.size(); ++i) {
        TradingSignal signal = {false, false, i, 0.0, 0.0};  // Initialize signal

        double currentSMA = indicators.calculateSMA(priceData, i, smaPeriod);
        double previousSMA = indicators.calculateSMA(priceData, i - 1, smaPeriod);
        double atr = indicators.calculateATR(priceData, 14, i);  // Assuming ATR is calculated by the indicators object
        double positionSize = (accountBalance * riskPerTrade) / atr;  // Calculate position size based on risk and ATR
        double stopLossLevel = priceData[i].close - (atr * stopLossMultiplier);  // Calculate stop-loss level

        // Buy signal condition: price crosses above the moving average
        if (priceData[i].close > currentSMA && priceData[i - 1].close <= previousSMA) {
            signal.buy = true;
            signal.positionSize = positionSize;
            signal.stopLossLevel = stopLossLevel;
        }

        // Sell signal condition: price crosses below the moving average
        if (priceData[i].close < currentSMA && priceData[i - 1].close >= previousSMA) {
            signal.sell = true;
            // Adjust positionSize and stopLossLevel as needed for sell signals
        }

        if (signal.buy || signal.sell) {
            signals.push_back(signal);
            // Update account balance based on position size (simplified for illustration)
            accountBalance -= positionSize;
        }
    }

    return signals;
}
