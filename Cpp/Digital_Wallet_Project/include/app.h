#pragma once

#include "menu.h"
#include "user.h"
#include "user_repository.h"
#include "users_list.h"

class Application {
   public:
    UserRepository user_repo;
    MenuState state;
    MenuManager menu_manager;
    Application();
    // ReturnStatus init_menu_manager();
    void app_run();
    ~Application();
};