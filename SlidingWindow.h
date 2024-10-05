#ifndef TRADINGKLX_SLIDINGWINDOW_H
#define TRADINGKLX_SLIDINGWINDOW_H
#include <deque>
#include <vector>

class SlidingWindow {
public:

    SlidingWindow(size_t size) : maxSize(size) {}

    void addDataPoint(double value) {
        if (window.size() >= maxSize) {
            window.pop_front();  // Remove the oldest element
        }
        window.push_back(value);
    }

    size_t size() const {
        return window.size();
    }

    const std::deque<double>& getData() const {
        return window;
    }

    std::vector<double> toVector() const {
        return std::vector<double>(window.begin(), window.end());
    }

    std::vector<double> getSafeDataAsVector(int start, int end) const {
        if (start < 0 || end > static_cast<int>(window.size()) || start >= end) {
            throw std::out_of_range("Invalid window range.");
        }
        return std::vector<double>(window.begin() + start, window.begin() + end);
    }

private:
    size_t maxSize;  // Maximum number of elements in the sliding window
    std::deque<double> window;  // Store the data points in a deque
};
#endif // TRADINGKLX_SLIDINGWINDOW_H