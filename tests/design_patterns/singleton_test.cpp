#include "../libftpp.hpp"
#include <cassert>

class MySingleton {
    friend class Singleton<MySingleton>;
private:
    MySingleton(int v) : value(v) {}
public:
    int value;
};

extern "C" int singleton_basic_test(void) {
    try {
        Singleton<MySingleton>::instantiate(42);
        MySingleton* inst = Singleton<MySingleton>::instance();
        if (!inst) return 255;
        if (inst->value != 42) return 255;
        bool threw = false;
        try { Singleton<MySingleton>::instantiate(1); } catch (...) { threw = true; }
        if (!threw) return 255;
        return 0;
    } catch (...) {
        return 255;
    }
}
