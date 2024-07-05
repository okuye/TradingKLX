#ifndef DATABASE_UTILS_H
#define DATABASE_UTILS_H

#include <string>
#include <mongocxx/client.hpp>

void storeDataInMongo(mongocxx::client& client, const std::string& dbName, const std::string& collectionName, const std::string& jsonData);
std::string fetchDataFromMongo(mongocxx::client& client, const std::string& dbName, const std::string& collectionName);

#endif // DATABASE_UTILS_H
