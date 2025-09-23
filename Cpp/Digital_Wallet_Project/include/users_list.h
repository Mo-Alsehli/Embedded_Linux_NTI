// users_list.h
#pragma once
#include <deque>
#include <functional>
#include <optional>
#include <string>

#include "user.h"

class UsersList {
    std::deque<User> users;
    size_t max_users;

   public:
    explicit UsersList(size_t max = 100);

    UsersList(const UsersList&) = delete;
    UsersList& operator=(const UsersList&) = delete;
    UsersList(UsersList&&) = default;
    UsersList& operator=(UsersList&&) = default;
    ~UsersList() = default;

    bool add_user(const User& user);
    std::optional<std::reference_wrapper<User>> login_user(const std::string& username, const std::string& password);
    std::optional<std::reference_wrapper<User>> search_user(const std::string& username);
    std::vector<std::string> save_updated_users() const;
    size_t size() const;
};
