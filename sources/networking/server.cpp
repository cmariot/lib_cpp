#include "networking/server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include "networking/debug.hpp"

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Server::Server() = default;

Server::~Server() {
    stop();
    std::lock_guard<std::mutex> lg(_m);
    for (auto &p : _clients) {
        ::shutdown(p.second, SHUT_RDWR);
        ::close(p.second);
    }
    _clients.clear();
    if (_listen_sock >= 0) {
        ::shutdown(_listen_sock, SHUT_RDWR);
        ::close(_listen_sock);
        _listen_sock = -1;
    }
}

void Server::stop() {
    _running = false;
    if (_worker.joinable()) _worker.join();
    {
        std::lock_guard<std::mutex> lg(_m);
        for (auto &p : _clients) {
            ::shutdown(p.second, SHUT_RDWR);
            ::close(p.second);
        }
        _clients.clear();
        _fd_to_id.clear();
        _recv_buffers.clear();
    }
    if (_listen_sock >= 0) {
        ::shutdown(_listen_sock, SHUT_RDWR);
        ::close(_listen_sock);
        _listen_sock = -1;
    }
}

void Server::start(const size_t& p_port) {
    NET_LOG("SERVER: start this=" << this << " port=" << p_port);
    _listen_sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_sock < 0) throw std::runtime_error("socket()");

    int opt = 1;
    setsockopt(_listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(static_cast<uint16_t>(p_port));

    if (::bind(_listen_sock, reinterpret_cast<sockaddr*>(&serv), sizeof(serv)) < 0) {
        ::close(_listen_sock);
        _listen_sock = -1;
        throw std::runtime_error("bind()");
    }

    // store the bound port (useful if p_port == 0)
    {
        sockaddr_in addr{};
        socklen_t len = sizeof(addr);
        if (::getsockname(_listen_sock, reinterpret_cast<sockaddr*>(&addr), &len) == 0) {
            _bound_port = ntohs(addr.sin_port);
        }
    }

    if (::listen(_listen_sock, 16) < 0) {
        ::close(_listen_sock);
        _listen_sock = -1;
        throw std::runtime_error("listen()");
    }

    if (set_nonblocking(_listen_sock) < 0) {
        ::close(_listen_sock);
        _listen_sock = -1;
        throw std::runtime_error("set_nonblocking()");
    }

    _running = true;
    _worker = std::thread([this]() {
        while (_running) {
            // Accept new clients
            while (true) {
                int client = ::accept(_listen_sock, nullptr, nullptr);
                if (client < 0) break;
                    NET_LOG("SERVER: accepted client fd=" << client);
                if (set_nonblocking(client) < 0) {
                    ::close(client);
                    continue;
                }
                ClientID id;
                {
                    std::lock_guard<std::mutex> lg(_m);
                    id = _next_client_id++;
                    _clients[id] = client;
                    _fd_to_id[client] = id;
                    _recv_buffers[id] = {};
                }
            }

            // Build pollfds
            std::vector<pollfd> fds;
            {
                std::lock_guard<std::mutex> lg(_m);
                fds.reserve(_clients.size() + 1);
                pollfd lf{};
                lf.fd = _listen_sock;
                lf.events = POLLIN;
                fds.push_back(lf);
                for (auto &p : _clients) {
                    pollfd pf{};
                    pf.fd = p.second;
                    pf.events = POLLIN;
                    fds.push_back(pf);
                }
            }

            int ret = ::poll(fds.data(), static_cast<nfds_t>(fds.size()), 200);
            if (ret < 0) continue;
            if (ret == 0) continue;

            // Iterate fds
            for (size_t i = 0; i < fds.size(); ++i) {
                if (!(fds[i].revents & POLLIN)) continue;
                int fd = fds[i].fd;
                if (fd == _listen_sock) continue; // accept handled above

                // read available data into buffer
                uint8_t tmp[4096];
                ssize_t r = ::recv(fd, tmp, sizeof(tmp), 0);
                if (r > 0) {
                    NET_LOG("SERVER: recv fd=" << fd << " bytes=" << r);
                }
                if (r <= 0) {
                    // disconnected or error
                    ClientID id = -1;
                    {
                        std::lock_guard<std::mutex> lg(_m);
                        auto it = _fd_to_id.find(fd);
                        if (it != _fd_to_id.end()) id = it->second;
                    }
                    if (id != -1) {
                        std::lock_guard<std::mutex> lg(_m);
                        ::shutdown(fd, SHUT_RDWR);
                        ::close(fd);
                        _fd_to_id.erase(fd);
                        _clients.erase(id);
                        _recv_buffers.erase(id);
                    }
                    continue;
                }

                // append to client's buffer and extract any complete frames
                ClientID id = -1;
                std::vector<std::vector<uint8_t>> extracted_msgs;
                {
                    std::lock_guard<std::mutex> lg(_m);
                    auto it = _fd_to_id.find(fd);
                    if (it != _fd_to_id.end()) id = it->second;
                    if (id != -1) {
                        auto &buf = _recv_buffers[id];
                        buf.insert(buf.end(), tmp, tmp + r);
                        NET_LOG("SERVER: client id=" << id << " buffer_size=" << buf.size());
                        // extract complete frames while holding the lock, push them to extracted_msgs
                        while (true) {
                            if (buf.size() < 4) break;
                            uint32_t netlen;
                            std::memcpy(&netlen, buf.data(), 4);
                            uint32_t msglen = ntohl(netlen);
                            NET_LOG("SERVER: parsed msglen=" << msglen << " (buf_size=" << buf.size() << ")");
                            if (msglen > 10 * 1024 * 1024) { // 10MB cap
                                // bad frame, drop connection
                                ::shutdown(fd, SHUT_RDWR);
                                ::close(fd);
                                _fd_to_id.erase(fd);
                                _clients.erase(id);
                                _recv_buffers.erase(id);
                                id = -1;
                                break;
                            }
                            if (buf.size() < 4 + msglen) break; // wait for full frame
                            // extract message bytes (type+payload)
                            std::vector<uint8_t> msgbuf(buf.begin() + 4, buf.begin() + 4 + msglen);
                            // consume from buffer
                            buf.erase(buf.begin(), buf.begin() + 4 + msglen);
                            extracted_msgs.push_back(std::move(msgbuf));
                        }
                    }
                }

                // process extracted messages outside the lock
                if (id != -1) {
                    for (auto &msgbuf : extracted_msgs) {
                        if (msgbuf.size() < 4) continue;
                        int32_t net_t;
                        std::memcpy(&net_t, msgbuf.data(), 4);
                        int32_t t = ntohl(net_t);
                                NET_LOG("SERVER: message type=" << t << " payload_len=" << (msgbuf.size()-4));
                        Message m(static_cast<int>(t));
                        if (msgbuf.size() > 4) {
                            m.payload().clear();
                            m.payload().reserve(msgbuf.size() - 4);
                            for (size_t i = 4; i < msgbuf.size(); ++i) m.payload() << msgbuf[i];
                        }

                        // find handler (lock briefly)
                        MessageHandler h = nullptr;
#ifdef LIBFTPP_NETWORK_DEBUG
                        size_t handlers_count = 0;
                        bool found = false;
#endif
                        {
                            std::lock_guard<std::mutex> lg(_m);
#ifdef LIBFTPP_NETWORK_DEBUG
                            handlers_count = _handlers.size();
#endif
                            auto it = _handlers.find(m.type());
#ifdef LIBFTPP_NETWORK_DEBUG
                            if (it != _handlers.end()) { h = it->second; found = true; }
#else
                            if (it != _handlers.end()) h = it->second;
#endif
                        }
#ifdef LIBFTPP_NETWORK_DEBUG
                        NET_LOG("SERVER: handlers_count=" << handlers_count << " found=" << found);
#endif
                        if (h) {
                            try {
                                NET_LOG("SERVER: invoking handler for id=" << id);
                                h(id, m);
                                NET_LOG("SERVER: handler returned for id=" << id);
                            } catch (const std::exception &e) {
                                NET_LOG("SERVER: handler threw: " << e.what());
                            } catch (...) {
                                NET_LOG("SERVER: handler threw unknown exception");
                            }
                        }
                    }
                }
            }
        }
    });
}

void Server::defineAction(const Message::Type& messageType, const MessageHandler& action) {
    std::lock_guard<std::mutex> lg(_m);
    _handlers[messageType] = action;
    NET_LOG("SERVER: defineAction type=" << messageType << " this=" << this << " handlers_count=" << _handlers.size());
}

size_t Server::getPort() const {
    return _bound_port;
}

void Server::sendTo(const Message& message, ClientID clientID) {
    int sock = -1;
    {
        std::lock_guard<std::mutex> lg(_m);
        auto it = _clients.find(clientID);
        if (it == _clients.end()) return;
        sock = it->second;
    }
    std::vector<uint8_t> buf;
    int32_t t = static_cast<int32_t>(message.type());
    int32_t net_t = htonl(t);
    const uint8_t *tp = reinterpret_cast<const uint8_t*>(&net_t);
    buf.insert(buf.end(), tp, tp + sizeof(net_t));
    const auto &p = message.payload();
    const uint8_t *pdata = p.data();
    size_t psz = p.size();
    if (pdata && psz > 0) buf.insert(buf.end(), pdata, pdata + psz);
    uint32_t len = static_cast<uint32_t>(buf.size());
    uint32_t netlen = htonl(len);
    // send length
    const uint8_t *len_ptr = reinterpret_cast<const uint8_t*>(&netlen);
    size_t to_write = sizeof(netlen);
    while (to_write > 0) {
        ssize_t w = ::send(sock, len_ptr + (sizeof(netlen) - to_write), to_write, 0);
        if (w <= 0) {
            // remove client
            std::lock_guard<std::mutex> lg(_m);
            auto it = _fd_to_id.find(sock);
            if (it != _fd_to_id.end()) {
                ClientID id = it->second;
                ::shutdown(sock, SHUT_RDWR);
                ::close(sock);
                _fd_to_id.erase(sock);
                _clients.erase(id);
                _recv_buffers.erase(id);
            }
            return;
        }
        to_write -= static_cast<size_t>(w);
    }
    // send payload
    const uint8_t *data_ptr = buf.data();
    to_write = len;
    while (to_write > 0) {
        ssize_t w = ::send(sock, data_ptr + (len - to_write), to_write, 0);
        if (w <= 0) {
            std::lock_guard<std::mutex> lg(_m);
            auto it = _fd_to_id.find(sock);
            if (it != _fd_to_id.end()) {
                ClientID id = it->second;
                ::shutdown(sock, SHUT_RDWR);
                ::close(sock);
                _fd_to_id.erase(sock);
                _clients.erase(id);
                _recv_buffers.erase(id);
            }
            return;
        }
        to_write -= static_cast<size_t>(w);
    }
}

void Server::sendToArray(const Message& message, std::vector<ClientID> clientIDs) {
    for (auto id : clientIDs) sendTo(message, id);
}

void Server::sendToAll(const Message& message) {
    std::vector<ClientID> copy;
    {
        std::lock_guard<std::mutex> lg(_m);
        for (auto &p : _clients) copy.push_back(p.first);
    }
    for (auto id : copy) sendTo(message, id);
}

void Server::update() {
    // With poll-based worker loop, nothing to do here. API kept for compatibility.
}
