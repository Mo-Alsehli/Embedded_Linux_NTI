#include "user.h"

void User::set_username(const std::string& uname) { username = uname; }

void User::set_userpasswd(const std::string& passwd) { password = passwd; }

std::string User::get_username() const { return username; }

std::string User::get_userpasswd() const { return password; }

double User::get_balance() const { return balance; }

void User::deposit(double amount) { balance += amount; }

bool User::check_credentials(const User& other) const { return username == other.username && password == other.password; }

bool User::operator==(const User& other) const { return username == other.username && password == other.password; }

void User::withdraw(double amount) {
    if (amount > balance) {
        printMessage("ERROR::Insufficient Balance", MsgType::ERROR);
        return;
    }
    balance -= amount;
}