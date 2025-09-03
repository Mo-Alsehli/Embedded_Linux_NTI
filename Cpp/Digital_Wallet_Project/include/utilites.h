#pragma once

#include <optional>

#include "user.h"
#include "users_list.h"

std::optional<User> login(UsersList& u_list);
void menu_page(User&);
