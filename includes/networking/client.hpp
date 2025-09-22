#ifndef LIBFTPP_NETWORKING_CLIENT_HPP
#define LIBFTPP_NETWORKING_CLIENT_HPP

#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <vector>
#include <functional>
#include <map>
#include <atomic>
#include "networking/message.hpp"

/**
 * @file includes/networking/client.hpp
 * @brief Simple TCP client wrapper used in tests and examples.
 *
 * The Client class manages a single TCP connection to a server. It starts a
 * background reader thread that receives framed messages and enqueues them in
 * an internal inbox. The user must call update() from a safe context (e.g.
 * a main loop or a test) to dispatch received messages to registered
 * handlers.
 *
 * Design notes:
 * - Messages are framed on the wire as: [uint32_t len][int32_t type][payload]
 *   where len and type are in network byte order.
 * - Handlers are invoked from update(), not the reader thread. This keeps
 *   handler code single-threaded and avoids locking issues inside user code.
 * - The class is intentionally small and not feature-complete (no reconnect,
 *   TLS, etc.). It's intended for unit/integration tests in the repo.
 */

class Client {
public:
    using MessageHandler = std::function<void(const Message&)>;

    Client();
    ~Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    /**
     * @brief Connect to a server, starts the background reader thread.
     * @param address IPv4 address as dotted string
     * @param port TCP port
     * @throws std::runtime_error on socket/connect errors
     */
    void connect(const std::string& address, const size_t& port);

    /**
     * @brief Disconnect/stop the background reader and close the socket.
     */
    void disconnect();

    /**
     * @brief Register a handler for a specific message type.
     *
     * The handler will be invoked from update(), not the background reader.
     */
    void defineAction(const Message::Type& messageType, const MessageHandler& action);

    /**
     * @brief Send a message to the connected server.
     *
     * If the socket is closed this is a no-op.
     */
    void send(const Message& message);

    /**
     * @brief Process all queued messages and invoke their handlers.
     *
     * Call this from the thread that registered handlers. The method will
     * catch exceptions thrown by handlers and swallow them to keep the
     * dispatch loop running.
     */
    void update();

private:
    int _sock{-1};
    std::mutex _m;
    std::queue<Message> _inbox;
    std::map<Message::Type, MessageHandler> _handlers;
    std::atomic<bool> _running{false};
};

#endif // LIBFTPP_NETWORKING_CLIENT_HPP
