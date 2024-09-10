#ifndef PRICEDATA_H
#define PRICEDATA_H
#include <string>

struct PriceData {
    double askOpen, askHigh, askLow, askClose;
    double askVolume;  

    double bidOpen, bidHigh, bidLow, bidClose;
    double bidVolume;  

    std::string timestamp;

    PriceData(std::string ts, double aO, double aH, double aL, double aC, double aV, 
              double bO, double bH, double bL, double bC, double bV)
        : timestamp(ts), askOpen(aO), askHigh(aH), askLow(aL), askClose(aC), askVolume(aV), 
          bidOpen(bO), bidHigh(bH), bidLow(bL), bidClose(bC), bidVolume(bV) {}
};

#endif // PRICEDATA_H
