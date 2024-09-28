#ifndef TRADING_SIGNAL_H
#define TRADING_SIGNAL_H

struct TradingSignal {
    bool buy;
    bool sell;
    int index;
    double positionSize;
    double stopLossLevel;
    double profit;
    double entryPrice;  // Added entry price
    double exitPrice;   // Added exit price
};

#endif // TRADING_SIGNAL_H
