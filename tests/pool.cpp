#include "../libftpp.hpp"
#include <iostream>

int main() {
    // Create a pool for integers, can hold up to 5 integers
    Pool<int> intPool(5);

    // Acquire an integer from the pool
    int* pInt = intPool.acquire();
    if (pInt) {
        *pInt = 42;
        std::cout << "Acquired integer: " << *pInt << std::endl;
    }

    // Release the integer back to the pool
    intPool.release(pInt);

    // Acquire 5 more integers
    for (int i = 0; i < 6; ++i) {
        int* pNewInt = intPool.acquire();
        if (pNewInt) {
            *pNewInt = i + 1;
            std::cout << "Acquired integer: " << *pNewInt << std::endl;
        }
        else {
            std::cout << "Failed to acquire integer, pool is full." << std::endl;
        }
    }

    return 0;
}