#include "Utilities.h"

std::string jsonToString(const Json::Value& json) {
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, json);
}