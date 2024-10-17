// TradingStrategyTest.cpp

#include "gtest/gtest.h"
#include "TradingStrategy.h"

class TradingStrategyTest : public ::testing::Test {
protected:
    std::unique_ptr<TradingStrategy> strategy;

    void SetUp() override {
        double initialBalance = 10000.0;
        double riskPerTrade = 0.02;
        double stopLossMultiplier = 1.5;
        int atrPeriod = 14;  // Add the atrPeriod parameter
        strategy = std::make_unique<TradingStrategy>(initialBalance, riskPerTrade, stopLossMultiplier, atrPeriod);
    }
};

// Example test for evaluateSignals() function
TEST_F(TradingStrategyTest, EvaluateSignalsBuySignal) {
    // Simulate some data
    strategy->onNewData(1.15, 1.10, 1.12);
    strategy->onNewData(1.16, 1.11, 1.13);
    strategy->onNewData(1.17, 1.12, 1.14);

    std::vector<TradingSignal> signals = strategy->evaluateSignals();
    ASSERT_FALSE(signals.empty());

    // Check if the first signal is a buy signal
    EXPECT_TRUE(signals[0].buy);
}

// Example test for evaluateSignals() function with extended data
TEST_F(TradingStrategyTest, EvaluateSignalsWithExtendedData) {
    // Simulate extended data
    for (int i = 0; i < 60; ++i) {
        strategy->onNewData(1.15 + i * 0.01, 1.10 + i * 0.01, 1.12 + i * 0.01);
    }

    auto signals = strategy->evaluateSignals();
    ASSERT_FALSE(signals.empty());

    // Check if there are buy and sell signals
    EXPECT_TRUE(signals[0].buy || signals[0].sell);
}

// Example test for evaluateSignals() function sell signal
TEST_F(TradingStrategyTest, EvaluateSignalsSellSignal) {
    // Simulate some data
    strategy->onNewData(1.20, 1.10, 1.15);
    strategy->onNewData(1.19, 1.11, 1.14);
    strategy->onNewData(1.18, 1.12, 1.13);

    std::vector<TradingSignal> signals = strategy->evaluateSignals();
    ASSERT_FALSE(signals.empty());

    // Check if the signal is a sell signal
    EXPECT_TRUE(signals[0].sell);
}
