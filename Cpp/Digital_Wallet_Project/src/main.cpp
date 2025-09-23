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
    Application app;

    app.app_run();

    return 0;
}
