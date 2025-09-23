#pragma once
#include <optional>
#include <string>

#include "print_banner.h"
#include "print_message.h"
#include "user.h"
#include "user_repository.h"
#include "users_list.h"

enum class ReturnStatus { Continue, ERROR, Exit };
class MenuState {
    std::optional<std::reference_wrapper<User>> curr_user;

   public:
    UserRepository& user_repo;
    MenuState(UserRepository& r) : user_repo(r){};
    void login(User& u) { curr_user.emplace(u); }
    void logout() { curr_user.reset(); }

    bool is_logged_in() const { return curr_user.has_value(); }

    User* get_user() { return curr_user ? &curr_user->get() : nullptr; }
};

class MenuManager;

class Menu {
   protected:
    MenuManager& m_manager;

   public:
    Menu(MenuManager& m) : m_manager(m){};
    virtual ReturnStatus display(MenuState& state) = 0;
    virtual ~Menu() = default;
};
class MenuManager {
    Menu* menu_type;
    ReturnStatus return_state;
    // UserRepository& user_repo;

   public:
    MenuState& state_ref;
    // UsersList* curr_users;
    MenuManager(MenuState& state);
    ~MenuManager();
    void set_menu(Menu* menu);
    ReturnStatus run_menu();
};

class WelcomeMenu : public Menu {
   private:
    // UsersList& curr_list;

   public:
    WelcomeMenu(MenuManager&);
    ReturnStatus display(MenuState& state) override;
};

class LoginMenu : public Menu {
    // MenuManager& m_manager;
    // UsersList& curr_list;

   public:
    LoginMenu(MenuManager&);
    ReturnStatus display(MenuState& state) override;
};

class SignUp : public Menu {
   private:
    // MenuManager& m_manager;
    // UsersList& curr_list;

   public:
    SignUp(MenuManager&);
    ReturnStatus display(MenuState& state) override;
};

class UserMenu : public Menu {
    // User& user;
    // MenuManager& m_manager;

   public:
    UserMenu(MenuManager&);
    ReturnStatus display(MenuState& state) override;
};

class PayPillsMenu : public Menu {
    // MenuManager& m_manager;

   public:
    PayPillsMenu(MenuManager&);
    ReturnStatus display(MenuState& state) override;
};

class TransactionMenu : public Menu {
   private:
    // MenuManager& m_manager;
    // UsersList& users_list;
    std::optional<std::reference_wrapper<User>> recv_user;

   public:
    TransactionMenu(MenuManager&);
    ReturnStatus display(MenuState&) override;
};
