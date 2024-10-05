#ifndef TRADINGKLX_TECHNICALINDICATORS_H
#define TRADINGKLX_TECHNICALINDICATORS_H

#include <vector>
#include <map>

// Memoization classes for Ichimoku and Bollinger Bands
struct IchimokuMemo {
    std::map<int, double> tenkanSenMemo;
    std::map<int, double> kijunSenMemo;
    std::map<int, double> senkouSpanAMemo;
    std::map<int, double> senkouSpanBMemo;
};

struct BollingerBandsMemo {
    std::map<int, double> upperBandMemo;
    std::map<int, double> lowerBandMemo;
    std::map<int, double> smaMemo;
    std::map<int, double> stdDevMemo;
};

class TechnicalIndicators {
public:
    double calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateSenkouSpanA(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    double calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo);
    std::pair<double, double> calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, BollingerBandsMemo& memo);
    double calculateSMA(const std::vector<double>& prices, int period);
    double calculateEMA(const std::vector<double>& prices, int period);
    double calculateStdDev(const std::vector<double>& data, int start, int end, double mean);
    double calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period, int currentIndex, double volatilityThreshold);
};

#endif // TRADINGKLX_TECHNICALINDICATORS_H