// TradingSignal.h
#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
#ifndef TRADING_SIGNAL_H
#define TRADING_SIGNAL_H

struct TradingSignal {
    bool buy = false;
    bool sell = false;
    int index = -1;
    double positionSize = 0.0;
    double stopLossLevel = 0.0;
    double profit = 0.0;
    double entryPrice = 0.0;  // Added entry price
    double exitPrice = 0.0;   // Added exit price
};

#endif // TRADING_SIGNAL_H