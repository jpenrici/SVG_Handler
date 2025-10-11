#include "svg_handler.hpp"

#include <print>

auto main() -> int
{
    std::println("Modern C++ 26 : Hello World!");

    if (test()) {
        std::println("Test is working!");
    } else {
        std::println("Test is not working!");
    }

    return 0;
}
