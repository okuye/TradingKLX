#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include <iostream>

TradingStrategy::TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier)
    : accountBalance(initialBalance), riskPerTrade(riskPerTrade), stopLossMultiplier(stopLossMultiplier) {
    smaPeriod = 50;
    bollingerBandsPeriod = 20;
    bollingerBandsMultiplier = 2;
}

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
    signals.clear(); // Clear previous signals
    double entryPrice = 0.0;
    double positionSize = 0.0;
    bool inPosition = false;
    int atrPeriod = 14;
    
    TechnicalIndicators indicators;

    for (size_t i = 1; i < priceData.size(); ++i) {
        TradingSignal signal = {false, false, static_cast<int>(i), 0.0, 0.0, 0.0};

        if (i < atrPeriod || i < smaPeriod) {
            continue;
        }

        double atr = std::max(indicators.calculateATR(highs, lows, closes, atrPeriod, static_cast<int>(i)), 0.0001);
        positionSize = std::min((accountBalance * riskPerTrade) / atr, accountBalance * 0.03 / closes[i]);

        if (positionSize < 0.01) {
            continue;
        }

        if (closes[i] > indicators.calculateSMA(closes, static_cast<int>(i), smaPeriod) &&
            closes[i - 1] <= indicators.calculateSMA(closes, static_cast<int>(i - 1), smaPeriod) &&
            (tenkanS[i] > kijunS[i] && closes[i] > senkouA[i] && closes[i] > senkouB[i]) && !inPosition) {
            signal.buy = true;
            signal.positionSize = positionSize;
            signal.stopLossLevel = closes[i] - (atr * stopLossMultiplier);
            entryPrice = closes[i];
            inPosition = true;
            std::cout << "Buy signal at index " << i << " with entry price: " << entryPrice << std::endl;
        }

        if (closes[i] < indicators.calculateSMA(closes, static_cast<int>(i), smaPeriod) &&
            closes[i - 1] >= indicators.calculateSMA(closes, static_cast<int>(i - 1), smaPeriod) &&
            (tenkanS[i] < kijunS[i] && closes[i] < senkouA[i] && closes[i] < senkouB[i]) && inPosition) {
            signal.sell = true;
            signal.profit = (closes[i] - entryPrice) * positionSize;
            accountBalance += signal.profit;
            inPosition = false;
            std::cout << "Sell signal at index " << i << " with profit: " << signal.profit << std::endl;
        }

        if (signal.buy || signal.sell) {
            signals.push_back(signal);
        }
    }

    return signals;
}

double TradingStrategy::getAccountBalance() const {
    return accountBalance;
}

// Implement the getSignals method
const std::vector<TradingSignal>& TradingStrategy::getSignals() const {
    return signals;
}
