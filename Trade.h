#ifndef TRADE_H
#define TRADE_H

struct Trade {
    double entryPrice;
    double exitPrice;
    bool buy;

    Trade(double entry, double exit, bool isBuy)
            : entryPrice(entry), exitPrice(exit), buy(isBuy) {}

    double profit() const {
        return (buy ? exitPrice - entryPrice : entryPrice - exitPrice);
    }
};

#endif // TRADE_H
