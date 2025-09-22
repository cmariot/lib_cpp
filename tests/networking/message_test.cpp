#include "../test_utils.hpp"
#include "../libftpp.hpp"

extern "C" int message_test(void) {
    Message m(42);
    int32_t a = 12345;
    m << a;
    std::string s = "hello";
    m << s;

    int32_t ra = 0;
    m.read(ra);
    if (!(ra == a)) {
        std::cout << "Assertion failed: ra == a; ra=" << ra << " a=" << a << std::endl;
        return 255;
    }

    std::string rs;
    m.readString(rs);
    if (!(rs == s)) {
        std::cout << "Assertion failed: rs == s; rs='" << rs << "' s='" << s << "'" << std::endl;
        return 255;
    }

    return 0;
}
