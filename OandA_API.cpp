#include "OandA_API.hpp"

OandA_API::OandA_API(const std::string& api_key) : api_key(api_key) {}

std::vector<double> OandA_API::getPrices(const std::string& instrument) {
    std::vector<double> prices;
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        std::string url = "https://api-fxpractice.oanda.com/v3/instruments/" + instrument + "/candles?count=2&price=M&granularity=M1";
        struct curl_slist* headers = nullptr;
        std::string bearer_token = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, bearer_token.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK) {
            auto json = nlohmann::json::parse(readBuffer);
            for(const auto& candle : json["candles"]) {
                prices.push_back(candle["mid"]["c"].get<double>());
            }
        }

        if (headers) {
            curl_slist_free_all(headers);
        }
    }
    return prices;
}

size_t OandA_API::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
