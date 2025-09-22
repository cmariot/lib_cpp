#include "../test_utils.hpp"
#include "../libftpp.hpp"
#include <thread>
#include <chrono>

extern "C" int broadcast_test(void) {
    Server srv;
    srv.start(0);
    const size_t bound = srv.getPort();

    std::atomic<int> recv_count{0};

    Client c1, c2;
    c1.connect("127.0.0.1", bound);
    c2.connect("127.0.0.1", bound);

    // small pause to allow the server to accept connections
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // register handlers on clients so they count received messages
    c1.defineAction(10, [&recv_count](const Message &m){ (void)m; recv_count.fetch_add(1); });
    c2.defineAction(10, [&recv_count](const Message &m){ (void)m; recv_count.fetch_add(1); });

    Message m(10);
    m << std::string("hello all");
    srv.sendToAll(m);

    // allow some time for delivery
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // dispatch client inboxes
    c1.update();
    c2.update();

    srv.stop();
    c1.disconnect();
    c2.disconnect();

    if (recv_count.load() < 2) {
        std::cout << "broadcast recv_count=" << recv_count.load() << std::endl;
        return 255;
    }
    return 0;
}
