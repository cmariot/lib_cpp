#include "../libftpp.hpp"
#include <cassert>

extern "C" int observer_basic_test(void) {
    try {
        Observer<int> obs;
        int called = 0;
        obs.subscribe(1, [&](){ called += 1; });
        obs.subscribe(2, [&](){ called += 10; });
        obs.notify(1);
        assert(called == 1);
        obs.notify(2);
        assert(called == 11);
        obs.notify(3); // no subscribers, no-op
        assert(called == 11);
        return 0;
    } catch (...) {
        return 255;
    }
}
