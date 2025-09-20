#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <iostream>

// Simple helper macros for tests
#define ASSERT_TRUE(expr) if (!(expr)) { std::cout << "Assertion failed: " #expr << std::endl; return 255; }
#define ASSERT_EQ(a,b) if (!((a) == (b))) { std::cout << "Assertion failed: " #a " == " #b << std::endl; return 255; }

#endif // TEST_UTILS_HPP
