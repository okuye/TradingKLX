#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include <vector>
#include <deque>
#include "PriceData.h"
#include "TechnicalIndicators.h"
#include "TradingSignal.h"
#include "SlidingWindow.h"
#include <memory>  // For shared_ptr
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

struct Trade; // Forward declaration

class TradingStrategy {
private:
    double accountBalance;
    double riskPerTrade;
    double stopLossMultiplier;

    int smaPeriod;
    int bollingerBandsPeriod;
    double bollingerBandsMultiplier;

    bool hasLoggedInsufficientData;
    bool inPosition;

    SlidingWindow highsWindow;
    SlidingWindow lowsWindow;
    SlidingWindow tenkanWindow;
    SlidingWindow kijunWindow;
    SlidingWindow closes;  // Change closes from std::vector to SlidingWindow

    std::vector<double> tenkanS;
    std::vector<double> kijunS;
    std::vector<double> senkouA;
    std::vector<double> senkouB;
    std::vector<double> lowerBB;
    std::vector<double> upperBB;

    std::vector<TradingSignal> signals;
    std::vector<double> portfolioBalanceHistory;

    IchimokuMemo memo;
    BollingerBandsMemo bbMemo;
    TechnicalIndicators technicalIndicators;

    std::shared_ptr<spdlog::logger> logger;

    void recordPortfolioBalance();

public:
    // Constructor
    TradingStrategy(double initialBalance, double riskPerTrade, double stopLossMultiplier);

    // Core Methods
    void onNewData(double high, double low, double close);
    std::vector<TradingSignal> evaluateSignals();

    // Getters and Setters
    double getAccountBalance() const;
    void setAccountBalance(double balance);

    double getRiskPerTrade() const;
    void setRiskPerTrade(double risk);

    double getStopLossMultiplier() const;
    void setStopLossMultiplier(double multiplier);

    const std::vector<TradingSignal>& getSignals() const;
    void addSignal(const TradingSignal& signal);

    const std::vector<double>& getPortfolioBalanceHistory() const;

    std::vector<Trade> getTrades() const;
};

#endif // TRADING_STRATEGY_H