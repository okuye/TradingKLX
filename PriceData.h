#ifndef PRICEDATA_H
#define PRICEDATA_H

#include <string>

struct PriceData {
    double open, high, low, close;
    std::string timestamp;
};

#endif // PRICEDATA_H
