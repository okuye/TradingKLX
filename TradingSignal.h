#ifndef TRADING_SIGNAL_H
#define TRADING_SIGNAL_H

#include <cstddef>  // Include this to define size_t

struct TradingSignal {
    bool buy;          // Indicates if this is a buy signal
    bool sell;         // Indicates if this is a sell signal
    std::size_t index; // The index in the data where this signal is generated
    double positionSize;  // The size of the position to take on this signal
    double stopLossLevel; // The stop-loss level for this signal
    double profit; 
};

#endif