//
// Created by olakunle kuye on 14/08/2024.
//
#include "Utilities.h"

std::string jsonToString(const Json::Value& jsonValue) {
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, jsonValue);
}