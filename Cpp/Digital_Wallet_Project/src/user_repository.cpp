#include "user_repository.h"

UserRepository::UserRepository(UsersList& curr_users) : users_list(curr_users) {}

std::vector<std::string> UserRepository::parse_line(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string cell;

    while (getline(ss, cell, ',')) {
        result.push_back(cell);
    }

    return result;
}

void UserRepository::load_users() {
    std::vector<std::string> row;
    std::ifstream file(db_name);
    if (!file.is_open()) {
        printMessage("Can't open File " + db_name, MsgType::ERROR);
        return;
    }

    std::string line;
    bool is_first_line = true;

    while (std::getline(file, line)) {
        if (is_first_line) {
            is_first_line = false;
            continue;
        }

        row = parse_line(line);
        std::string name = row[0];
        std::string passwd = row[1];
        double balance = stod(row[2]);

        User curr_user;
        curr_user.set_username(name);
        curr_user.set_userpasswd(passwd);
        curr_user.deposit(balance);

        users_list.add_user(curr_user);
    }
    file.close();
}

void UserRepository::update_users() {
    std::vector<std::string> users_db = users_list.save_updated_users();

    std::ofstream file(db_name);

    for (auto row : users_db) {
        for (char ch : row) {
            file << ch;
        }
        file << '\n';
    }
}
