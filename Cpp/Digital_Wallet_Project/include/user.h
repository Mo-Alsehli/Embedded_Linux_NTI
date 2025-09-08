#pragma once
#include <string>

#include "print_message.h"
class User {
    std::string username;
    std::string password;
    double balance = 0.0;

   public:
    User() = default;

    User(const User&) = default;
    User(User&&) = default;
    User& operator=(const User&) = default;
    User& operator=(User&&) = default;
    ~User() = default;

    void set_username(const std::string& uname);
    void set_userpasswd(const std::string& passwd);
    std::string get_username() const;
    std::string get_userpasswd() const;
    double get_balance() const;
    void deposit(double amount);
    void withdraw(double amount);
    bool check_credentials(const User& other) const;
    bool operator==(const User& other) const;
};
