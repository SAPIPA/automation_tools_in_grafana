#pragma once

#include <string>
#include <string_view>

namespace app {

enum class UserType { kFirstTime, kKnown };

std::string SayHelloTo(std::string_view name, UserType type);

}  // namespace app