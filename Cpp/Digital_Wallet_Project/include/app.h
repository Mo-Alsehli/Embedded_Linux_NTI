#pragma once

#include "menu.h"
#include "user.h"
#include "users_list.h"

class Application {
   private:
    MenuManager menu_manager;

   public:
    Application(MenuState&, UsersList*);
    MenuReturnState init_menu_manager();
    void app_run();
};