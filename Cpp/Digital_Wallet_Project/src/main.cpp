#include <unistd.h>

#include <iostream>
#include <string>

// User defined Header files
#include "app.h"
#include "menu.h"
#include "print_banner.h"
#include "print_message.h"
#include "user.h"
#include "user_repository.h"
#include "users_list.h"
// #include "utilites.h"

int main() {
    UsersList u_list(20);

    UserRepository repo(u_list);
    repo.load_users();
    // Shared menu state
    MenuState state;
    Application app(state, &u_list);

    app.app_run();

    repo.update_users();

    return 0;
}
