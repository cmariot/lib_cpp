#include "../libftpp.hpp"
#include <iostream>
#include <cassert>

extern "C" int pool_basic_test(void) {
    try {
        // Crée un pool pouvant contenir 3 int
        Pool<int> intPool;
        intPool.resize(3);

        assert(intPool.capacity() == 3);
        assert(intPool.availableCount() == 3);

        // Acquisition de 3 entiers du pool
        auto &obj1 = intPool.acquire(42);
        auto &obj2 = intPool.acquire(1337);
        auto &obj3 = intPool.acquire(777);

        assert(intPool.availableCount() == 0);
        assert(intPool.usedCount() == 3);

        // Vérifie les valeurs
        assert(*obj1 == 42);
        assert(*obj2 == 1337);
        assert(*obj3 == 777);

        // Libération des objets (remise dans le pool)
        intPool.release(obj1);
        intPool.release(obj2);
        intPool.release(obj3);

        assert(intPool.availableCount() == 3);

        // Test d'erreur: acquérir plus que la capacité
        bool threw = false;
        try {
            auto &a = intPool.acquire(1);
            auto &b = intPool.acquire(2);
            auto &c = intPool.acquire(3);
            // all three acquired
            (void)a; (void)b; (void)c;
            // fourth should throw
            auto &d = intPool.acquire(4);
            (void)d;
        } catch (const std::exception& e) {
            threw = true;
        }
        // We expect throwing when requesting 4th when only 3 capacity
        assert(threw == true);

        // Test with non-trivial type
        Pool<std::string> strPool;
        strPool.resize(2);
        auto &s1 = strPool.acquire("hello");
        auto &s2 = strPool.acquire("world");
        assert(*s1 == "hello");
        assert(*s2 == "world");
        strPool.release(s1);
        strPool.release(s2);

        return 0;
    } catch (...) {
        return 255;
    }
}