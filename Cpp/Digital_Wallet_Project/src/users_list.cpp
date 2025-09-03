#include "users_list.h"

UsersList::UsersList(size_t max) : max_users(max) {}

bool UsersList::add_user(const User& user) {
    if (users.size() >= max_users) return false;
    users.push_back(user);
    return true;
}

std::optional<User> UsersList::search_users(const User& match) const {
    for (const auto& u : users) {
        if (u == match) return u;
    }
    return std::nullopt;
}

size_t UsersList::size() const { return users.size(); }
