#include "app.h"

#include <iostream>

Application::Application() : user_repo(), state(user_repo), menu_manager(state) { user_repo.load_users(); }

void Application::app_run() { menu_manager.run_menu(); }

Application::~Application() { user_repo.update_users(); }
