#pragma once
#include <optional>
#include <vector>

#include "user.h"

class UsersList {
    std::vector<User> users;
    size_t max_users;

   public:
    UsersList(size_t max = 100);

    UsersList(const UsersList&) = delete;
    UsersList& operator=(const UsersList&) = delete;
    UsersList(UsersList&&) = default;
    UsersList& operator=(UsersList&&) = default;
    ~UsersList() = default;

    bool add_user(const User& user);
    std::optional<User> search_users(const User& match) const;
    size_t size() const;
};
