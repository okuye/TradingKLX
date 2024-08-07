#ifndef TECHNICALINDICATORS_H
#define TECHNICALINDICATORS_H

#include <vector>
#include <unordered_map>
#include "PriceData.h"

struct IchimokuMemo {
    std::unordered_map<int, double> tenkanSenMemo, kijunSenMemo, senkouSpanAMemo, senkouSpanBMemo;
};

struct BollingerBandsMemo {
    std::unordered_map<int, double> smaMemo, stdDevMemo, upperBandMemo, lowerBandMemo;
};

class TechnicalIndicators {
public:
    double calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateSenkouSpanA(int index, IchimokuMemo& memo);
    double calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo);
    void calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo);
    double calculateSMA(const std::vector<double>& data, int start, int end);
    double calculateStdDev(const std::vector<double>& data, int start, int end, double mean);
    double calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int index);
};

#endif // TECHNICALINDICATORS_H
