// database_utils.h
#ifndef DATABASE_UTILS_H
#define DATABASE_UTILS_H

#include <mongocxx/client.hpp>
#include <string>

std::string fetchDataFromMongo(const mongocxx::client& client, const std::string& dbName, const std::string& collectionName);
void storeDataInMongo(const mongocxx::client& client, const std::string& dbName, const std::string& collectionName, const std::string& jsonData);

#endif // DATABASE_UTILS_H
