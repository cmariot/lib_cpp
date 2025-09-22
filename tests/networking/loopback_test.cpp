#include "../test_utils.hpp"
#include "../libftpp.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>

extern "C" int loopback_test(void) {
    using namespace std::chrono_literals;
    Server srv;
    const size_t port = 0; // let OS pick an ephemeral port
    std::atomic<bool> called{false};
    std::mutex cv_m;
    std::condition_variable cv;
    srv.defineAction(1, [&called,&cv](Server::ClientID id, const Message &m) {
        std::cout << "TEST: handler invoked for client=" << id << " type=" << m.type() << std::endl;
        std::flush(std::cout);
        called = true;
        cv.notify_one();
    });
    srv.start(port);
    const size_t bound = srv.getPort();
    std::cout << "TEST: server started on port " << bound << std::endl;
    std::flush(std::cout);

    Client c;
    std::cout << "TEST: client connecting..." << std::endl;
    std::flush(std::cout);
    c.connect("127.0.0.1", bound);
    std::cout << "TEST: client connected" << std::endl;
    std::flush(std::cout);

    Message m(1);
    m << int32_t(7);
    std::cout << "TEST: client sending message type=" << m.type() << std::endl;
    std::flush(std::cout);
    c.send(m);
    std::cout << "TEST: client send complete" << std::endl;
    std::flush(std::cout);

    // wait for handler notification with timeout
    {
        std::unique_lock<std::mutex> lk(cv_m);
        bool signaled = cv.wait_for(lk, 2s, [&called]{ return called.load(); });
        if (!signaled) std::cout << "TEST: wait_for timed out" << std::endl;
        else std::cout << "TEST: wait_for signaled" << std::endl;
        std::flush(std::cout);
    }

    srv.stop();
    c.disconnect();
    ASSERT_TRUE(called.load());
    return 0;
}
