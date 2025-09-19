#pragma once
#include <optional>
#include <string>

#include "print_banner.h"
#include "print_message.h"
#include "user.h"
#include "users_list.h"

enum class MenuReturnState { Continue, ERROR, Exit };
class MenuState {
    std::optional<std::reference_wrapper<User>> curr_user;

   public:
    void login(User& u) { curr_user.emplace(u); }
    void logout() { curr_user.reset(); }

    bool is_logged_in() const { return curr_user.has_value(); }

    User* get_user() { return curr_user ? &curr_user->get() : nullptr; }
};

class Menu {
   public:
    virtual MenuReturnState display(MenuState& state) = 0;
    virtual ~Menu() = default;
};

class MenuManager {
    Menu* menu_type;
    MenuState& state_ref;
    MenuReturnState return_state;

   public:
    UsersList* curr_users;
    MenuManager(MenuState& state, UsersList* u_list);
    ~MenuManager();
    void set_menu(Menu* menu);
    MenuReturnState run_menu();
};

class WelcomeMenu : public Menu {
   private:
    MenuManager& m_manager;
    UsersList& curr_list;

   public:
    WelcomeMenu(MenuManager&, UsersList&);
    MenuReturnState display(MenuState& state) override;
};

class LoginMenu : public Menu {
    MenuManager& m_manager;
    UsersList& curr_list;

   public:
    LoginMenu(UsersList& u_list, MenuManager&);
    MenuReturnState display(MenuState& state) override;
};

class SignUp : public Menu {
   private:
    MenuManager& m_manager;
    UsersList& curr_list;

   public:
    SignUp(UsersList&, MenuManager&);
    MenuReturnState display(MenuState& state) override;
};

class UserMenu : public Menu {
    // User& user;
    MenuManager& m_manager;

   public:
    UserMenu(MenuManager&);
    MenuReturnState display(MenuState& state) override;
};

class PayPillsMenu : public Menu {
    MenuManager& m_manager;

   public:
    PayPillsMenu(MenuManager&);
    MenuReturnState display(MenuState& state) override;
};

class TransactionMenu : public Menu {
   private:
    MenuManager& m_manager;
    UsersList& users_list;
    std::optional<std::reference_wrapper<User>> recv_user;

   public:
    TransactionMenu(MenuManager&, UsersList&);
    MenuReturnState display(MenuState&) override;
};
