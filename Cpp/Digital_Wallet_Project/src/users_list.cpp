// users_list.cpp
#include "users_list.h"

UsersList::UsersList(size_t max) : max_users(max) {}

bool UsersList::add_user(const User& user) {
    if (users.size() >= max_users) return false;
    users.push_back(user);  // deque keeps references stable
    return true;
}

std::optional<std::reference_wrapper<User>> UsersList::login_user(const std::string& username, const std::string& password) {
    for (User& u : users) {
        if (u.get_username() == username && u.get_userpasswd() == password) {
            return u;
        }
    }
    return std::nullopt;
}
std::optional<std::reference_wrapper<User>> UsersList::search_user(const std::string& username) {
    for (User& u : users) {
        if (u.get_username() == username) {
            return u;
        }
    }
    return std::nullopt;
}

std::vector<std::string> UsersList::save_updated_users() const {
    std::vector<std::string> users_db;
    users_db.push_back("username,password,balance");
    for (const User& u : users) {
        users_db.push_back(u.get_username() + "," + u.get_userpasswd() + "," + std::to_string(u.get_balance()));
    }
    return users_db;
}

size_t UsersList::size() const { return users.size(); }
