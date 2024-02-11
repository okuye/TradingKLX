// database_utils.cpp
#include "database_utils.h"
#include <mongocxx/client.hpp>
#include <bsoncxx/json.hpp>
#include <iostream> // For error logging

std::string fetchDataFromMongo(const mongocxx::client& client, const std::string& dbName, const std::string& collectionName) {
    try {
        auto collection = client[dbName][collectionName];
        auto cursor = collection.find({});
        std::string data;
        for (auto&& doc : cursor) {
            data += bsoncxx::to_json(doc);
        }
        return data;
    } catch (const mongocxx::exception& e) {
        std::cerr << "Error fetching data from MongoDB: " << e.what() << std::endl;
        // Depending on your application's requirements, you may want to handle this error differently,
        // for example, by re-throwing, returning an empty string, or attempting a retry.
        return ""; // Returning empty string to indicate failure
    }
}

void storeDataInMongo(const mongocxx::client& client, const std::string& dbName, const std::string& collectionName, const std::string& jsonData) {
    try {
        auto collection = client[dbName][collectionName];
        bsoncxx::stdx::optional<bsoncxx::document::value> document = bsoncxx::from_json(jsonData);
        if(document) {
            collection.insert_one(document->view());
        } else {
            std::cerr << "Failed to convert JSON data to BSON document." << std::endl;
            // Handle or log the error as needed
        }
    } catch (const mongocxx::exception& e) {
        std::cerr << "Error storing data in MongoDB: " << e.what() << std::endl;
        // Handle the error appropriately for your application
    }
}
