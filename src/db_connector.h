#ifndef DB_CONNECTOR_H
#define DB_CONNECTOR_H

#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <cstdlib>

class DBConnector {
private:
    MYSQL* conn;
    const char* host;
    const char* user;
    const char* pass;
    const char* db;

public:
    DBConnector() {
        host = std::getenv("DB_HOST") ? std::getenv("DB_HOST") : "";
        user = std::getenv("DB_USER") ? std::getenv("DB_USER") : "";
        pass = std::getenv("DB_PASS") ? std::getenv("DB_PASS") : "";
        db   = std::getenv("DB_NAME") ? std::getenv("DB_NAME") : "";

        conn = mysql_init(nullptr);
        if (!conn) {
            std::cerr << "MySQL init failed.\n";
            exit(1);
        }

        if (!mysql_real_connect(conn, host, user, pass, db, 3306, nullptr, 0)) {
            std::cerr << "MySQL connection failed: " << mysql_error(conn) << "\n";
            exit(1);
        }

        std::cout << "✅ Connected to MySQL database successfully!\n";
    }

    bool insertCustomer(const std::string& name, const std::string& email,
                        const std::string& phone, const std::string& password_hash) {
        std::string query =
            "INSERT INTO customers (name, email, phone, password_hash) VALUES ('" +
            name + "', '" + email + "', '" + phone + "', '" + password_hash + "')";
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Insert failed: " << mysql_error(conn) << "\n";
            return false;
        }
        std::cout << "✅ Customer inserted successfully!\n";
        return true;
    }

    ~DBConnector() {
        if (conn) mysql_close(conn);
    }
};

#endif
