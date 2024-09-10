#include "TradingStrategy.h"
#include <iostream> // For debug output

// Constructor: Initialize the strategy with the new risk management parameters
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

// Evaluate signals and manage portfolio risk
std::vector<TradingSignal> TradingStrategy::evaluateSignals(const std::vector<PriceData>& priceData,
                                                            const std::vector<double>& closes,
                                                            const std::vector<double>& highs,
                                                            const std::vector<double>& lows,
                                                            const std::vector<double>& tenkanS,
                                                            const std::vector<double>& kijunS,
                                                            const std::vector<double>& senkouA,
                                                            const std::vector<double>& senkouB,
                                                            const std::vector<double>& lowerBB,
                                                            const std::vector<double>& upperBB) {
    std::vector<TradingSignal> signals;
    double entryPrice = 0.0;
    double positionSize = 0.0;
    bool inPosition = false;  // Track if we are currently in a position

    for (size_t i = 1; i < priceData.size(); ++i) {
        TradingSignal signal = {false, false, i, 0.0, 0.0, 0.0};  // Initialize signal with 0 profit

        // Calculate ATR and cap it to prevent over-sizing positions
        double atr = std::max(indicators.calculateATR(highs, lows, closes, 14, i), 0.0001);
        
        // Cap position size at a reasonable level, such as 3% of portfolio balance
        positionSize = std::min((accountBalance * riskPerTrade) / atr, accountBalance * 0.03 / closes[i]);

        // Skip trades with extremely small positions
        if (positionSize < 0.01) {
            continue;
        }

        // Buy signal: SMA crossover and Ichimoku confirmation
        if (closes[i] > indicators.calculateSMA(closes, i, smaPeriod) &&
            closes[i - 1] <= indicators.calculateSMA(closes, i - 1, smaPeriod) &&
            (tenkanS[i] > kijunS[i] && closes[i] > senkouA[i] && closes[i] > senkouB[i]) && !inPosition) {
            signal.buy = true;
            signal.positionSize = positionSize;
            signal.stopLossLevel = closes[i] - (atr * stopLossMultiplier);
            entryPrice = closes[i];  // Record entry price
            inPosition = true;  // We are now in a position
        }

        // Sell signal: SMA crossover and Ichimoku confirmation
        if (closes[i] < indicators.calculateSMA(closes, i, smaPeriod) &&
            closes[i - 1] >= indicators.calculateSMA(closes, i - 1, smaPeriod) &&
            (tenkanS[i] < kijunS[i] && closes[i] < senkouA[i] && closes[i] < senkouB[i]) && inPosition) {
            signal.sell = true;
            signal.profit = (closes[i] - entryPrice) * positionSize;  // Calculate profit
            accountBalance += signal.profit;  // Update account balance
            inPosition = false;  // Exit position
            positionSize = 0;  // Reset position size after selling
        }

        if (signal.buy || signal.sell) {
            signals.push_back(signal);
        }
    }

    return signals;
}

// Getter method to return the current account balance
double TradingStrategy::getAccountBalance() const {
    return accountBalance;
}