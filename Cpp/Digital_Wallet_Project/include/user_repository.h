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
    UsersList& users_list;
    const std::string db_name = "./db/users_db.csv";

   public:
    UserRepository(UsersList& users_list);
    std::vector<std::string> parse_line(const std::string& line);
    void load_users();
    void update_users();
};