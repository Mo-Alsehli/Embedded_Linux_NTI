#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "users_list.h"

class UserRepository {
   private:
    // std::fstream& curr_db;
    UsersList users_list;
    const std::string db_name = "./db/users_db.csv";

    // private member function
    std::vector<std::string> save_updated_users() const;

   public:
    UserRepository();
    std::vector<std::string> parse_line(const std::string& line);
    bool add_user(const User& user);
    std::optional<std::reference_wrapper<User>> login_user(const std::string& username, const std::string& password);
    std::optional<std::reference_wrapper<User>> search_user(const std::string& username);
    void load_users();
    void update_users();
};