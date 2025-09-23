#include "menu.h"

#include <iostream>

// MenuManager
MenuManager::MenuManager(MenuState& state) : state_ref(state) { menu_type = new WelcomeMenu(*this); };

MenuManager::~MenuManager() { delete menu_type; }

void MenuManager::set_menu(Menu* menu) {
    delete menu_type;
    menu_type = menu;
}

ReturnStatus MenuManager::run_menu() {
    ReturnStatus state = ReturnStatus::Continue;
    while (state == ReturnStatus::Continue) {
        state = menu_type->display(state_ref);
    }
    return state;
}

// Welcome Menu

WelcomeMenu::WelcomeMenu(MenuManager& manager) : Menu(manager){};

ReturnStatus WelcomeMenu::display(MenuState& state) {
    std::cout << "\033[2J\033[1;1H";  // This is to clear the screen
    printBanner("Welcome To Smart Wallet");
    printMessage("Login Page", MsgType::INFO);
    std::cout << "Please Make a Selection: \n";
    std::cout << "(S) Sign Up\n";
    std::cout << "(L) Login\n";
    std::cout << "(Q) Quit\n";

    std::string query;
    std::cout << "==> ";
    std::cin >> query;

    if (query == "L" || query == "l") {
        m_manager.set_menu(new LoginMenu(m_manager));
        return ReturnStatus::Continue;
    } else if (query == "S" || query == "s") {
        m_manager.set_menu(new SignUp(m_manager));
        return ReturnStatus::Continue;
    } else if (query == "Q" || query == "q") {
        printMessage("Goodbye!", MsgType::INFO);

        return ReturnStatus::Exit;
    } else {
        printMessage("Invalid selection. Please try again.", MsgType::WARNING);

        return ReturnStatus::ERROR;
    }
}

// LoginMenu
LoginMenu::LoginMenu(MenuManager& manager) : Menu(manager){};

ReturnStatus LoginMenu::display(MenuState& state) {
    std::string user_name;
    std::string user_passwd;
    std::string query;

    printMessage("Login Page", MsgType::INFO);
    std::cout << "Please Make a Selection: \n";
    std::cout << "(L) Login\n";
    std::cout << "(Q) Quit\n";
    std::cin >> query;

    if (query[0] == 'L' || query[0] == 'l') {
        std::cout << "\033[2J\033[1;1H";  // This is to clear the screen
        //  system("clear");
        printMessage("Login Page::Enter Login Credentials", MsgType::INFO);
        std::cout << "Please enter user name: ";
        std::cin >> user_name;
        std::cout << "Enter Password: ";
        std::cin >> user_passwd;

        // User user;
        // user.set_username(user_name);
        // user.set_userpasswd(user_passwd);
        auto result = state.user_repo.login_user(user_name, user_passwd);
        if (result) {
            state.login(result->get());
            std::cout << "\033[2J\033[1;1H";  // This is to clear the screen
            std::string message = "Welcome " + result->get().get_username();
            // state.curr_user->get().withdraw(500);
            printBanner(message);
            m_manager.set_menu(new UserMenu(m_manager));
            return ReturnStatus::Continue;
        } else {
            printMessage("Invalid username or password.", MsgType::ERROR);
            std::cout << "[R]etry or [Q]uit? ";
            std::string choice;
            std::cin >> choice;

            if (!choice.empty() && (choice[0] == 'q' || choice[0] == 'Q')) {
                printMessage("Login cancelled.", MsgType::WARNING);
                state.logout();
                return ReturnStatus::Exit;
            }
            return ReturnStatus::Continue;
        }
    } else if (query[0] == 'q' || query[0] == 'Q') {
        printMessage("Goodbye!", MsgType::INFO);
        return ReturnStatus::ERROR;
    } else {
        printMessage("Invalid selection. Please try again.", MsgType::WARNING);
        return ReturnStatus::Continue;
    }
}

// Sign Up Menu

SignUp::SignUp(MenuManager& manager) : Menu(manager) {}

ReturnStatus SignUp::display(MenuState& state) {
    std::string user_name;
    std::string user_passwd;
    std::string user_confirm_passwd;
    int init_balance = 0;

    std::cout << "\033[2J\033[1;1H";  // This is to clear the screen
    //  system("clear");
    printMessage("Sign-Up Page::Enter Login Credentials", MsgType::INFO);
    std::cout << "Please enter user name: ";
    std::cin >> user_name;
    std::cout << "Enter Password: ";
    std::cin >> user_passwd;
    std::cout << "Confirm Password: ";
    std::cin >> user_confirm_passwd;

    if (user_passwd != user_confirm_passwd) {
        printMessage("ERROR::Password Didn't Match", MsgType::ERROR);
        m_manager.set_menu(new SignUp(m_manager));
        return ReturnStatus::Continue;
    }

    std::cout << "Enter Initial Balance: ";
    std::cin >> init_balance;

    User new_user;
    new_user.set_username(user_name);
    new_user.set_userpasswd(user_passwd);
    new_user.deposit(init_balance);

    state.user_repo.add_user(new_user);
    printMessage("User: " + user_name + "Created Successfully", MsgType::INFO);
    m_manager.set_menu(new LoginMenu(m_manager));
    return ReturnStatus::Continue;
}

// User Menu.
UserMenu::UserMenu(MenuManager& manager) : Menu(manager){};

ReturnStatus UserMenu::display(MenuState& state) {
    // Ensure we have a valid logged-in user
    if (!state.is_logged_in()) {
        printMessage("No user is currently logged in.", MsgType::ERROR);
        return ReturnStatus::ERROR;
    }

    // Get reference to current user from shared MenuState
    User* user = state.get_user();

    // Display menu options
    std::cout << "Please Make a Selection\n";
    std::cout << "[1] View balance\n";
    std::cout << "[2] Withdraw\n";
    std::cout << "[3] Deposit\n";
    std::cout << "[4] Transfer Money\n";
    std::cout << "[5] Pay Pills\n";
    std::cout << "[6] Logout\n";

    std::string query;
    std::cin >> query;  // Get user input

    if (query == "1") {
        // Option 1: View balance
        float balance = user->get_balance();
        printMessage("Your Balance: " + std::to_string(balance), MsgType::INFO);
        return ReturnStatus::Continue;
    } else if (query == "2") {
        // Option 2: Withdraw
        double value;
        std::cout << "Enter a value to withdraw: ";
        std::cin >> value;
        if (value > 0) {
            user->withdraw(value);  // Withdraw from user's balance
        } else {
            printMessage("Invalid Value", MsgType::ERROR);
        }
        return ReturnStatus::Continue;

    } else if (query == "3") {
        // Option 3: Deposit
        double value;
        std::cout << "Enter a value to deposit: ";
        std::cin >> value;
        if (value > 0) {
            user->deposit(value);
            printMessage("Deposited Successfully\nYour new balance: " + std::to_string(user->get_balance()), MsgType::INFO);
        } else {
            printMessage("Invalid Value", MsgType::ERROR);
        }
        return ReturnStatus::Continue;

    } else if (query == "4") {
        m_manager.set_menu(new TransactionMenu(m_manager));
        return ReturnStatus::Continue;
    } else if (query == "5") {
        m_manager.set_menu(new PayPillsMenu(m_manager));
        return ReturnStatus::Continue;
    } else if (query == "6") {
        // Option 4: Logout
        printMessage("Logged Out", MsgType::INFO);
        m_manager.set_menu(new WelcomeMenu(m_manager));
        return ReturnStatus::Continue;

    } else {
        // Catch-all for invalid input
        printMessage("Invalid selection", MsgType::WARNING);
        return ReturnStatus::Continue;
    }
}

PayPillsMenu::PayPillsMenu(MenuManager& manager) : Menu(manager) {}

ReturnStatus PayPillsMenu::display(MenuState& state) {
    std::cout << "\033[2J\033[1;1H";  // This is to clear the screen

    printMessage("Pay Your Pills Here ", MsgType::INFO);
    User* user = state.get_user();

    std::string query;

    std::cout << "[1] Recharge Mobile \n";
    std::cout << "[2] Pay electricity pills \n";
    std::cout << "[3] Pay College Fees \n";
    std::cout << "[4] quit \n";

    std::cout << "Please Make a Selection: ";
    std::cin >> query;

    if (query == "1") {
        std::string number;
        std::cout << "Enter Mobile Number: ";
        std::cin >> number;
        std::cout << "Enter Recharge Amount: ";
        double amount;
        std::cin >> amount;

        user->withdraw(amount);
        std::cout << number << "Recharged with amount " << amount << "Succesfully\n";

        return ReturnStatus::Continue;
    } else if (query == "4") {
        m_manager.set_menu(new UserMenu(m_manager));

        return ReturnStatus::Continue;
    } else {
        printMessage("Invalid Selection", MsgType::WARNING);
        m_manager.set_menu(new PayPillsMenu(m_manager));
        return ReturnStatus::Continue;
    }
    return ReturnStatus::Exit;
}

// Transaction Class
TransactionMenu::TransactionMenu(MenuManager& manager) : Menu(manager) {}

ReturnStatus TransactionMenu::display(MenuState& state) {
    // std::cout << "\033[2J\033[1;1H";  // This is to clear the screen

    printMessage("Transaction Service", MsgType::INFO);

    std::cout << "Please Make a Selection\n";
    std::cout << "[1] Transfer Money\n";
    std::cout << "[2] Show Recent Transactions\n";
    std::cout << "[q] quit\n";

    std::string query;
    std::cout << "=> ";
    std::cin >> query;
    std::string r_username;
    double t_amount;

    // User r_user;
    if (query == "1") {
        std::cout << "Enter Transactions details ";
        std::cout << "Recibient Username => ";
        std::cin >> r_username;
        std::cout << "Transaction Amount => ";
        std::cin >> t_amount;
        // r_user.set_username(r_username);
        auto result = state.user_repo.search_user(r_username);
        User* logged_user = state.get_user();

        if (result) {
            User& recv_user = result->get();
            if (!logged_user->withdraw(t_amount)) {
                std::cout << "Transaction Faild\n";
                m_manager.set_menu(new TransactionMenu(m_manager));
                return ReturnStatus::Continue;
            }
            recv_user.deposit(t_amount);
            printMessage("Transaction Successful\n From " + logged_user->get_username() + " To " + r_username + "\n Amount " +
                             std::to_string(t_amount) + "$",
                         MsgType::INFO);
            m_manager.set_menu(new TransactionMenu(m_manager));
            return ReturnStatus::Continue;
        } else {
            return ReturnStatus::ERROR;
        }
    } else if (query == "2") {
        return ReturnStatus::Exit;
    } else if (query == "q") {
        m_manager.set_menu(new UserMenu(m_manager));
        return ReturnStatus::Continue;
    } else {
        std::cout << "[ERROR] Invalid Selection\n";
        m_manager.set_menu(new TransactionMenu(m_manager));
        return ReturnStatus::Continue;
    }

    return ReturnStatus::ERROR;  // shouldn't reach here.
}