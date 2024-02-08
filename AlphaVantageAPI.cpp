#include "AlphaVantageAPI.h"
#include <curl/curl.h>
#include <iostream>

// Callback function to handle data received by libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

AlphaVantageAPI::AlphaVantageAPI(const std::string& apiKey) : apiKey(apiKey) {}

std::string AlphaVantageAPI::fetchData(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval) {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    if (curl) {
        std::string url = buildURL(function, from_symbol, to_symbol, interval);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // Timeout for the request in seconds

        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        } else if (http_code == 429) { // HTTP Too Many Requests
            std::cerr << "API rate limit exceeded. Please try again later." << std::endl;
            return "";
        }
    }
    return readBuffer;
}

std::string AlphaVantageAPI::buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval) {
    return "https://www.alphavantage.co/query?function=" + function + "&from_symbol=" + from_symbol + "&to_symbol=" + to_symbol + "&interval=" + interval + "&apikey=" + apiKey;
}
