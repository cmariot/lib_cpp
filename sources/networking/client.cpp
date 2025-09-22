#include "networking/client.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <iostream>
#include <map>

// ...existing code... (no file-scoped fallback handlers)

Client::Client() = default;

Client::~Client() {
    disconnect();
}

void Client::connect(const std::string& address, const size_t& port) {
    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) throw std::runtime_error("socket()");
    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(static_cast<uint16_t>(port));
    if (::inet_pton(AF_INET, address.c_str(), &serv.sin_addr) <= 0) {
        ::close(_sock);
        _sock = -1;
        throw std::runtime_error("inet_pton()");
    }

    if (::connect(_sock, reinterpret_cast<sockaddr*>(&serv), sizeof(serv)) < 0) {
        ::close(_sock);
        _sock = -1;
        throw std::runtime_error("connect()");
    }

    _running = true;
    // background reader
    std::thread([this](){
        while (_running) {
            uint32_t netlen;
            ssize_t r = ::recv(_sock, &netlen, sizeof(netlen), MSG_WAITALL);
            if (r <= 0) break;
            uint32_t len = ntohl(netlen);
            if (len == 0) continue;
            std::vector<uint8_t> buf(len);
            r = ::recv(_sock, buf.data(), len, MSG_WAITALL);
            if (r <= 0) break;
            // first 4 bytes = type (network order)
            if (buf.size() >= 4) {
                int32_t net_t;
                std::memcpy(&net_t, buf.data(), 4);
                int32_t t = ntohl(net_t);
                Message m(static_cast<int>(t));
                if (buf.size() > 4) {
                    // payload after type
                    m.payload().clear();
                    m.payload().reserve(buf.size() - 4);
                    for (size_t i = 4; i < buf.size(); ++i) m.payload() << buf[i];
                }
                std::lock_guard<std::mutex> lg(_m);
                _inbox.push(std::move(m));
            }
        }
        _running = false;
    }).detach();
}

void Client::disconnect() {
    _running = false;
    if (_sock >= 0) {
        ::shutdown(_sock, SHUT_RDWR);
        ::close(_sock);
        _sock = -1;
    }
}

void Client::defineAction(const Message::Type& messageType, const MessageHandler& action) {
    std::lock_guard<std::mutex> lg(_m);
    _handlers[messageType] = action;
}

void Client::send(const Message& message) {
    if (_sock < 0) return;
    // framing: [len(uint32_t)][type(int32_t)][payload]
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
        ssize_t w = ::send(_sock, len_ptr + (sizeof(netlen) - to_write), static_cast<size_t>(to_write), 0);
        if (w <= 0) return; // write error
        to_write -= static_cast<size_t>(w);
    }
    // send payload
    const uint8_t *data_ptr = buf.data();
    to_write = len;
    while (to_write > 0) {
        ssize_t w = ::send(_sock, data_ptr + (len - to_write), to_write, 0);
        if (w <= 0) return;
        to_write -= static_cast<size_t>(w);
    }
}

void Client::update() {
    std::lock_guard<std::mutex> lg(_m);
    while (!_inbox.empty()) {
        Message m = std::move(_inbox.front()); _inbox.pop();
        auto it = _handlers.find(m.type());
        if (it != _handlers.end()) {
            try {
                it->second(m);
            } catch (...) {
                // swallow handler exceptions to avoid crashing the update loop
            }
        }
    }
}
