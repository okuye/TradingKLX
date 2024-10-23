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

#endif // PRICEDATA_H