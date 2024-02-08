#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include <tuple>
#include <nlohmann/json.hpp>

class JSONParser {
public:
    static std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> parseJsonForPrices(const std::string& jsonData, const std::string& timeSeriesKey = "Time Series FX (5min)");

private:
    static bool validateJson(const nlohmann::json& j, const std::string& timeSeriesKey);
};

#endif // JSON_PARSER_H
