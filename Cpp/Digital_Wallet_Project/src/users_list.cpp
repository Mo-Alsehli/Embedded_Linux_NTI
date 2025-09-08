#include "users_list.h"

UsersList::UsersList(size_t max) : max_users(max) {}

bool UsersList::add_user(const User& user) {
    if (users.size() >= max_users) return false;
    users.push_back(user);
    return true;
}

std::optional<User*> UsersList::search_users(const User& match) const {
    for (auto u : users) {
        if (u == match) return &u;
    }
    return std::nullopt;
}

std::vector<std::string> UsersList::save_updated_users() {
    std::vector<std::string> users_db;
    std::string user;
    users_db.push_back("username,password,balance");
    for (User u : users) {
        user = u.get_username() + "," + u.get_userpasswd() + "," + std::to_string(u.get_balance());
        users_db.push_back(user);
    }

    return users_db;
}

size_t UsersList::size() const { return users.size(); }
