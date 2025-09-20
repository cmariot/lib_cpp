#include "../libftpp.hpp"
#include <iostream>

extern "C" int pool_basic_test(void) {
    try {
        // Crée un pool pouvant contenir 3 int
        Pool<int> intPool;
        intPool.resize(3);

        // Acquisition de 3 entiers du pool
        auto &obj1 = intPool.acquire(42);
        auto &obj2 = intPool.acquire(1337);
        auto &obj3 = intPool.acquire(777);

        // Utilisation des objets
        *obj1 = 42;
        *obj2 = 1337;

        std::cout << "obj1: " << *obj1 << std::endl;
        std::cout << "obj2: " << *obj2 << std::endl;
        std::cout << "obj3: " << *obj3 << std::endl;

        // Libération des objets (remise dans le pool)
        intPool.release(obj1);
        intPool.release(obj2);
        intPool.release(obj3);

        std::cout << "Objets disponibles après release: " << intPool.availableCount() << std::endl;

        return 0;
    } catch (...) {
        return 255;
    }
}