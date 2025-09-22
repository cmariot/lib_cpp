#include "../test_utils.hpp"
#include "../libftpp.hpp"

extern "C" int message_helpers_test(void) {
    Message m(7);
    m << int32_t(2025);
    m << std::string("ok");

    int32_t x = m.pop<int32_t>();
    if (x != 2025) { std::cout << "bad int: " << x << std::endl; return 255; }
    std::string s = m.popString();
    if (s != "ok") { std::cout << "bad string: " << s << std::endl; return 255; }
    return 0;
}
