#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include <tuple>


std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> parseJsonForPrices(const std::string& jsonData);


std::vector<double> parseJsonForClosePrices(const std::string& jsonData);

#endif // JSON_PARSER_H
