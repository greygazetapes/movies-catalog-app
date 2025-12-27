#include "sql_database.h"
#include <iostream>
#include <stdexcept>

Database::Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database_name)
{
    try {
        driver = get_driver_instance();
        if (!driver) {
            // std::cerr << "драйвер не был получен" << std::endl;
            return;
        }
        conn = std::unique_ptr<sql::Connection>(driver->connect(host, user, password));
        if (!conn) {
            // throw std::runtime_error("соединение не было создано");
        }
        conn->setSchema(database_name);
    }
    catch (const sql::SQLException& e) {
        // std::cerr << "ошибка sql: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // std::cerr << "ошибка std: " << e.what() << std::endl;
    }
}
