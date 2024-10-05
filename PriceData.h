#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#ifndef PRICEDATA_H
#define PRICEDATA_H

#include <string>

// PriceData struct for storing market data
struct PriceData {
    std::string timestamp;
    double askOpen, askHigh, askLow, askClose, askVolume;
    double bidOpen, bidHigh, bidLow, bidClose, bidVolume;

    // Default constructor
    PriceData()
            : askOpen(0), askHigh(0), askLow(0), askClose(0), askVolume(0),
              bidOpen(0), bidHigh(0), bidLow(0), bidClose(0), bidVolume(0) {}

    // Constructor with parameters
    PriceData(std::string ts, double aO, double aH, double aL, double aC, double aV,
              double bO, double bH, double bL, double bC, double bV)
            : timestamp(ts), askOpen(aO), askHigh(aH), askLow(aL), askClose(aC), askVolume(aV),
              bidOpen(bO), bidHigh(bH), bidLow(bL), bidClose(bC), bidVolume(bV) {}
};

// Trade struct to store information about each trade
struct Trade {
    double entryPrice;
    double exitPrice;
    double profit;
    bool isBuyTrade; // true if buy trade, false if sell trade

    // Default constructor
    Trade() : entryPrice(0.0), exitPrice(0.0), profit(0.0), isBuyTrade(true) {}

    // Constructor for initializing trade details
    Trade(double entry, double exit, bool isBuy)
            : entryPrice(entry), exitPrice(exit), isBuyTrade(isBuy), profit(exit - entry) {
        if (!isBuyTrade) {
            profit = entry - exit; // reverse profit calculation for sell trades
        }
    }
};

#endif // PRICEDATA_H