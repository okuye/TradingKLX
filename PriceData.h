#ifndef PRICEDATA_H
#define PRICEDATA_H
#include <string>

struct PriceData {
    // Ask data
    double askOpen, askHigh, askLow, askClose;
    double askVolume;  // Volume for Ask prices

    // Bid data
    double bidOpen, bidHigh, bidLow, bidClose;
    double bidVolume;  // Volume for Bid prices

    // Common timestamp for both Ask and Bid data
    std::string timestamp;

    // Constructor to initialize the data
    PriceData(std::string ts, double aO, double aH, double aL, double aC, double aV, 
              double bO, double bH, double bL, double bC, double bV)
        : timestamp(ts), askOpen(aO), askHigh(aH), askLow(aL), askClose(aC), askVolume(aV), 
          bidOpen(bO), bidHigh(bH), bidLow(bL), bidClose(bC), bidVolume(bV) {}
};

#endif // PRICEDATA_H