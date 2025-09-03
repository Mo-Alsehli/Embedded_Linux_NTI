#include "app.h"

#include <iostream>

Application::Application(MenuState& curr_state, UsersList* lst) : menu_manager(curr_state, lst) {}

void Application::app_run() { menu_manager.run_menu(); }
