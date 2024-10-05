#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
#ifndef ALPHAVANTAGEAPI_H
#define ALPHAVANTAGEAPI_H

#include <string>

class AlphaVantageAPI {
public:
    AlphaVantageAPI(const std::string& apiKey, long timeoutSeconds = 30);
    std::string fetchData(const std::string& function, const std::string& from_symbol, const std::string& to_symbol);

private:
    std::string buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol);
    void saveDataLocally(const std::string& data);
    std::string loadLocalData();
    std::string fetchWithRetry(const std::string& url, int attempts);

    std::string apiKey;
    long timeoutSeconds;
};

#endif