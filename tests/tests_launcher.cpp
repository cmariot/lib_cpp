#include "framework/includes/libunit.hpp"
#include "test_utils.hpp"

extern "C" int data_multiple_types_test(void);
extern "C" int data_buffer_overflow_test(void);
extern "C" int pool_basic_test(void);

int main() {
    t_test *tests = NULL;

    load_test(&tests, "DataBuffer", "data_multiple_types", (void*)data_multiple_types_test, 0);
    load_test(&tests, "DataBuffer", "data_buffer_overflow", (void*)data_buffer_overflow_test, 0);
    load_test(&tests, "Pool", "pool_basic", (void*)pool_basic_test, 0);

    return launch_tests(&tests);
}