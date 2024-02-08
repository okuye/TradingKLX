#include "TradingStrategy.h"


TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier) 
    : accountBalance(initialBalance), riskPerTrade(riskPerTrade), stopLossMultiplier(stopLossMultiplier) {
    // Initialize indicator parameters
    smaPeriod = 50;
    bollingerBandsPeriod = 20;
    bollingerBandsMultiplier = 2;

    // Initialize any data structures or connections needed
    priceHistory.reserve(1000); // Pre-allocate memory for price history if needed

    // Load configuration from file or environment
    loadConfiguration("config.json");
}

std::vector<TradingSignal> TradingStrategy::evaluateSignals(const std::vector<PriceData>& priceData) {
    std::vector<TradingSignal> signals;
    
    // Logic to evaluate trading signals based on technical indicators and price data
    for (size_t i = 1; i < priceData.size(); ++i) {
        TradingSignal signal = {false, false, i};

        // Example: Buy signal based on some technical indicator criteria
        if (/* condition for a buy signal, e.g., price crossing above SMA */) {
            signal.buy = true;

            // Example of incorporating risk management within signal evaluation
            double atr = indicators.calculateATR(priceData, 14, i);  // Assuming ATR is one of the indicators
            double positionSize = (accountBalance * riskPerTrade) / atr;
            double stopLossLevel = priceData[i].close - (atr * stopLossMultiplier);

            // Update account balance or execute trade based on positionSize and stopLossLevel
            // This part would typically interact with a trade execution module or update simulation parameters
        }

        // Example: Sell signal based on some technical indicator criteria
        if (/* condition for a sell signal, e.g., price crossing below SMA */) {
            signal.sell = true;
            // Similar logic for sell signal as for buy signal
        }

        if (signal.buy || signal.sell) {
            signals.push_back(signal);
        }
    }

    return signals;
}

// std::vector<TradingSignal> TradingStrategy::evaluateSignals(const std::vector<PriceData>& priceData) {
//     std::vector<TradingSignal> signals;
    
//     // Example of signal evaluation logic based on moving averages (simple case)
//     for (size_t i = 1; i < priceData.size(); ++i) {
//         TradingSignal signal = {false, false, i};

//         // Example condition for a buy signal: price crosses above the moving average
//         if (priceData[i].close > indicators.calculateSMA(priceData, i, 20) &&
//             priceData[i - 1].close <= indicators.calculateSMA(priceData, i - 1, 20)) {
//             signal.buy = true;
//         }

//         // Example condition for a sell signal: price crosses below the moving average
//         if (priceData[i].close < indicators.calculateSMA(priceData, i, 20) &&
//             priceData[i - 1].close >= indicators.calculateSMA(priceData, i - 1, 20)) {
//             signal.sell = true;
//         }

//         signals.push_back(signal);
//     }

//     return signals;
// }
