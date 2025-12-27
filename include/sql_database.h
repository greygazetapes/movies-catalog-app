#pragma once
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include <string>
#include "Movies.h"
#include "Movie.h"

class Database {
private:
    sql::Driver* driver;
    std::unique_ptr<sql::Connection> conn;
public:
    Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database_name);
    bool isConnected() const { return conn != nullptr; }
    sql::Connection* getConnection() const { return conn.get(); }
};

