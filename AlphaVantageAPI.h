#ifndef ALPHA_VANTAGE_API_H
#define ALPHA_VANTAGE_API_H

#include <string>

class AlphaVantageAPI {
public:
    AlphaVantageAPI(const std::string& apiKey, long timeoutSeconds = 30);
    std::string fetchData(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval = "5min");

private:
    long timeoutSeconds;
    std::string apiKey;
    std::string buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval);

    // Helper function to handle rate-limited requests with exponential backoff
    std::string fetchWithRetry(const std::string& url, int attempts = 5);
};

#endif // ALPHA_VANTAGE_API_H
