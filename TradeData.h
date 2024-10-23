#ifndef TRADEDATA_H
#define TRADEDATA_H

#include <string>

struct TradeData {
    std::string symbol;
    std::string date;
    int hour;
    double openBid, highBid, lowBid, closeBid;
    double openAsk, highAsk, lowAsk, closeAsk;
    int totalTicks;
};

#endif // TRADEDATA_H