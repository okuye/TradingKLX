// database_utils.cpp
#include "database_utils.h"
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>

std::string fetchDataFromMongo(const mongocxx::client& client, const std::string& dbName, const std::string& collectionName) {
    auto collection = client[dbName][collectionName];
    auto cursor = collection.find({});
    std::string data;
    for (auto&& doc : cursor) {
        data += bsoncxx::to_json(doc);
    }
    return data;
}

void storeDataInMongo(const mongocxx::client& client, const std::string& dbName, const std::string& collectionName, const std::string& jsonData) {
    auto collection = client[dbName][collectionName];
    bsoncxx::stdx::optional<bsoncxx::document::value> document = bsoncxx::from_json(jsonData);
    if(document) {
        collection.insert_one(document->view());
    }
}
