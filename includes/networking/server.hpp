#ifndef LIBFTPP_NETWORKING_SERVER_HPP
#define LIBFTPP_NETWORKING_SERVER_HPP

#include "networking/message.hpp"
#include <functional>
#include <map>
#include <vector>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <atomic>

/**
 * @file includes/networking/server.hpp
 * @brief Small TCP server used by the tests.
 *
 * Server accepts incoming connections and maintains a mapping between a
 * small client identifier and its socket. Each connection has a receive
 * buffer; framing is parsed and complete messages are passed to handlers.
 *
 * Notes:
 * - Messages are framed as: [uint32_t len][int32_t type][payload] where
 *   len and type are in network byte order.
 * - The server extracts complete frames while holding the internal mutex
 *   then invokes handlers outside the lock to avoid reentrancy and
 *   deadlocks.
 */
class Server {
public:
    using ClientID = long long;
    using MessageHandler = std::function<void(ClientID, const Message&)>;

    Server();
    ~Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    /**
     * @brief Start listening on the specified port.
     * @param p_port port to bind; use 0 for an ephemeral port
     */
    void start(const size_t& p_port);

    /** Stop the server and join the worker thread. */
    void stop();

    /**
     * @brief Register a handler for a message type.
     * @param messageType application-defined message type
     * @param action callback invoked when a message of messageType arrives
     */
    void defineAction(const Message::Type& messageType, const MessageHandler& action);

    /** Send a message to a single client id. */
    void sendTo(const Message& message, ClientID clientID);

    /** Send a message to a list of clients. */
    void sendToArray(const Message& message, std::vector<ClientID> clientIDs);

    /** Broadcast a message to all connected clients. */
    void sendToAll(const Message& message);

    /**
     * @brief Process server-side queued events. The tests call update()
     * periodically to let the server run short tasks on the caller thread.
     */
    void update();

    /** returns the bound port (useful when starting with port 0) */
    size_t getPort() const;

private:
    int _listen_sock = -1;
    std::mutex _m;
    std::map<ClientID, int> _clients; // clientID -> sock
    std::unordered_map<int, ClientID> _fd_to_id; // sock -> clientID
    std::map<ClientID, std::vector<uint8_t>> _recv_buffers; // partial recv buffer per client
    std::map<Message::Type, MessageHandler> _handlers;
    ClientID _next_client_id = 1;
    std::atomic<bool> _running{false};
    std::thread _worker;
    size_t _bound_port = 0;
};

#endif // LIBFTPP_NETWORKING_SERVER_HPP
