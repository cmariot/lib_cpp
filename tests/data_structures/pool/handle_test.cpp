#include "../libftpp.hpp"
#include <cassert>

extern "C" int pool_handle_test(void) {
    try {
        Pool<int> p;
        p.resize(1);
        {
            auto h = p.acquireHandle(5);
            assert(h.valid());
            assert(*h == 5);
            // at this scope, pool usedCount should be 1
            assert(p.usedCount() == 1);
        }
        // handle destroyed -> released
        assert(p.usedCount() == 0);

        // try_acquire when empty should return nullptr
        auto* a = p.try_acquire(1);
        assert(a != nullptr);
        // pool is now used
        assert(p.usedCount() == 1);
        // second try should return nullptr
        auto* b = p.try_acquire(2);
        assert(b == nullptr);
        // cleanup
        p.release(*a);
        return 0;
    } catch (...) {
        return 255;
    }
}
