// #include "utilites.h"

// #include <iostream>

// #include "print_banner.h"
// #include "print_message.h"

// std::optional<User> login(UsersList& u_list) {
//     while (1) {
//         std::string user_name;
//         std::string user_passwd;
//         printMessage("Login Page", MsgType::INFO);
//         std::cout << "Please enter user name: ";
//         std::cin >> user_name;
//         std::cout << "Enter Password: ";
//         std::cin >> user_passwd;

//         User user;
//         user.set_username(user_name);
//         user.set_userpasswd(user_passwd);
//         auto result = u_list.search_users(user);
//         if (result) {
//             User logged_user = *result;
//             std::string message = "Welcome " + logged_user.get_username();
//             printBanner(message);
//             return logged_user;
//         } else {
//             printMessage("Invalid username or password.", MsgType::ERROR);
//             std::cout << "[R]etry or [Q]uit? ";
//             std::string choice;
//             std::cin >> choice;

//             if (!choice.empty() && (choice[0] == 'q' || choice[0] == 'Q')) {
//                 printMessage("Login cancelled.", MsgType::WARNING);
//                 return std::nullopt;
//             }
//         }
//     }
// }

// void menu_page(User& u) {
//     while (1) {
//         std::cout << "inside menue" << std::endl;

//         std::cout << "Please Make a Selection\n";
//         std::cout << "[1] View balance\n";
//         std::cout << "[2] Withdraw\n";
//         std::cout << "[3] Deposit\n";
//         std::cout << "[4] Logout\n";

//         std::string query;
//         std::cin >> query;

//         if (query == "1") {
//             float balance = u.get_balance();

//             printMessage("Your Balance: " + std::to_string(balance), MsgType::INFO);
//         } else if (query == "2") {
//             int value;
//             std::cout << "Enter a value to withdraw: ";
//             std::cin >> value;
//             if (std::is_integral<int>::value && value > 0) {
//                 u.withdraw(value);
//             } else {
//                 printMessage("Invalid Value", MsgType::ERROR);
//             }
//         } else if (query == "3") {
//             int value;
//             std::cout << "Enter a value to deposit: ";
//             std::cin >> value;
//             int b = u.deposit(value);
//             printMessage("Deposited Successfully\nYour new balance: " + std::to_string(b), MsgType::INFO);
//         } else if (query == "4") {
//             printMessage("Logged Out", MsgType::INFO);
//             break;
//         }
//     }
// }
