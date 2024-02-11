#include "AlphaVantageAPI.h"
#include <curl/curl.h>
#include <iostream>
#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::seconds
#include <cstdlib> // For std::getenv

// Callback function to handle data received by libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

AlphaVantageAPI::AlphaVantageAPI(long timeoutSeconds) : timeoutSeconds(timeoutSeconds) {
    char* apiKeyEnv = std::getenv("ALPHA_VANTAGE_API_KEY");
    if (apiKeyEnv == nullptr) {
        throw std::runtime_error("API key environment variable 'ALPHA_VANTAGE_API_KEY' not set.");
    }
    apiKey = std::string(apiKeyEnv);
}

std::string AlphaVantageAPI::fetchData(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval) {
    std::string url = buildURL(function, from_symbol, to_symbol, interval);
    return fetchWithRetry(url);
}

std::string AlphaVantageAPI::buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval) {
    return "https://www.alphavantage.co/query?function=" + function + "&from_symbol=" + from_symbol + "&to_symbol=" + to_symbol + "&interval=" + interval + "&apikey=" + apiKey;
}

std::string AlphaVantageAPI::fetchWithRetry(const std::string& url, int attempts) {
    for (int attempt = 0; attempt < attempts; ++attempt) {
        CURL* curl = curl_easy_init();
        std::string readBuffer;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);

            CURLcode res = curl_easy_perform(curl);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK && http_code == 200) {
                // Check for error message in JSON response
                size_t errorPos = readBuffer.find("\"Note\":");
                if (errorPos != std::string::npos) {
                    std::cerr << "API returned an error message: " << readBuffer.substr(errorPos) << std::endl;
                    return "";
                }
                return readBuffer;
            } else {
                std::cerr << "Request failed, curl error code: " << res << ", HTTP status code: " << http_code << std::endl;
                if (http_code == 429 || res != CURLE_OK) { // Retry on rate limit or CURL errors
                    int waitTime = (attempt + 1) * 2;
                    std::cerr << "Retrying in " << waitTime << " seconds." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(waitTime));
                } else {
                    return ""; // Stop retrying on client or server errors
                }
            }
        } else {
            std::cerr << "curl_easy_init() failed, unable to initiate a curl handle." << std::endl;
            return "";
        }
    }
    std::cerr << "Failed to fetch data after " << attempts << " attempts." << std::endl;
    return "";
}
