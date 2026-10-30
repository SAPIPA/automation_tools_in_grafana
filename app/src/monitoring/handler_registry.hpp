#pragma once

#include <array>
#include <string_view>

namespace app {

struct HandlerInfo {
    std::string_view name;
    std::string_view method;
    std::string_view path;
};

inline constexpr std::array<HandlerInfo, 3> kHandlers{{
    {"handler-create-item", "POST", "/items"},
    {"handler-get-item", "GET", "/items/{id}"},
    {"handler-update-item", "PUT", "/items/{id}"},
}};

}  // namespace app