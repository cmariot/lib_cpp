#include "framework/includes/libunit.hpp"
#include "test_utils.hpp"

extern "C" int data_multiple_types_test(void);
extern "C" int data_buffer_overflow_test(void);
extern "C" int data_buffer_more_tests(void);
extern "C" int pool_handle_test(void);
extern "C" int pool_basic_test(void);
extern "C" int loopback_test(void);
extern "C" int message_helpers_test(void);
extern "C" int message_test(void);
extern "C" int broadcast_test(void);
extern "C" int state_machine_basic_test(void);
extern "C" int observer_basic_test(void);
extern "C" int memento_basic_test(void);
extern "C" int singleton_basic_test(void);

int main() {
    t_test *tests = NULL;

    load_test(&tests, "DataBuffer", "data_multiple_types", (void*)data_multiple_types_test, 0);
    load_test(&tests, "DataBuffer", "data_buffer_overflow", (void*)data_buffer_overflow_test, 0);
    load_test(&tests, "DataBuffer", "data_buffer_more_tests", (void*)data_buffer_more_tests, 0);
    load_test(&tests, "Pool", "pool_handle", (void*)pool_handle_test, 0);
    load_test(&tests, "Pool", "pool_basic", (void*)pool_basic_test, 0);
    load_test(&tests, "Networking", "loopback", (void*)loopback_test, 0);
    load_test(&tests, "Networking", "message_helpers", (void*)message_helpers_test, 0);
    load_test(&tests, "Networking", "message", (void*)message_test, 0);
    load_test(&tests, "Networking", "broadcast", (void*)broadcast_test, 0);
    load_test(&tests, "DesignPatterns", "state_machine_basic", (void*)state_machine_basic_test, 0);
    load_test(&tests, "DesignPatterns", "observer_basic", (void*)observer_basic_test, 0);
    load_test(&tests, "DesignPatterns", "memento_basic", (void*)memento_basic_test, 0);
    load_test(&tests, "DesignPatterns", "singleton_basic", (void*)singleton_basic_test, 0);

    return launch_tests(&tests);
}