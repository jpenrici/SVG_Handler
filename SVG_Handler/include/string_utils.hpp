/*
 * Manipulates string to separate Tags and attributes.
 */

#pragma once

#include <string_view>
#include <vector>

namespace StringUtils {

    auto sanitize(std::string_view str) -> std::string;
    auto validate(std::string_view svg) -> bool;
    auto process(std::string_view svg) -> std::vector<std::string>;

}

auto test_string_utils() -> bool;
