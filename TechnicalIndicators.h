#ifndef TECHNICALINDICATORS_H
#define TECHNICALINDICATORS_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <iostream>

struct IchimokuMemo {
    std::unordered_map<int, double> tenkanSenMemo;
    std::unordered_map<int, double> kijunSenMemo;
    std::unordered_map<int, double> senkouSpanAMemo;
    std::unordered_map<int, double> senkouSpanBMemo;
};

struct BollingerBandsMemo {
    std::unordered_map<int, double> upperBandMemo;
    std::unordered_map<int, double> lowerBandMemo;
    std::unordered_map<int, double> smaMemo;
    std::unordered_map<int, double> stdDevMemo;
};

class TechnicalIndicators {
public:
    double calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateSenkouSpanA(int index, IchimokuMemo& memo);
    double calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo);
    
    std::pair<double, double> calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo);
    
    double calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex);
    double calculateSMA(const std::vector<double>& data, int currentIndex, int period);
    double calculateStdDev(const std::vector<double>& data, int start, int end, double mean);
    
    std::pair<int, int> gridSearchIchimokuOptimization(const std::vector<double>& highs, const std::vector<double>& lows);
    double runStrategyWithParams(const std::vector<double>& tenkanS, const std::vector<double>& kijunS);
};

#endif
