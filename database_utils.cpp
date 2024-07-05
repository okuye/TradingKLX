#include "database_utils.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

void storeDataInMongo(mongocxx::client& client, const std::string& dbName, const std::string& collectionName, const std::string& jsonData) {
    auto db = client[dbName];
    auto collection = db[collectionName];
    auto documents = json::parse(jsonData);

    for (const auto& doc : documents) {
        bsoncxx::builder::stream::document document{};
        document << "data" << bsoncxx::from_json(doc.dump());
        collection.insert_one(document.view());
    }
}

std::string fetchDataFromMongo(mongocxx::client& client, const std::string& dbName, const std::string& collectionName) {
    auto db = client[dbName];
    auto collection = db[collectionName];
    mongocxx::cursor cursor = collection.find({});

    json result = json::array();
    for (auto&& doc : cursor) {
        result.push_back(json::parse(bsoncxx::to_json(doc)));
    }

    return result.dump();
}
