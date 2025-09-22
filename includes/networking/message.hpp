#ifndef LIBFTPP_NETWORKING_MESSAGE_HPP
#define LIBFTPP_NETWORKING_MESSAGE_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <cstring>
#include "data_structures/data_buffer.hpp"
#include <arpa/inet.h>

class Message {
public:
    /** Logical message type (application defined). */
    using Type = int;

    /**
     * @brief Construct a message with the given type.
     * @param t logical message type (default 0)
     */
    Message(Type t = 0) : _type(t) {}

    /** Return the message type. */
    Type type() const { return _type; }

    /** Access to the underlying binary payload buffer (DataBuffer). */
    DataBuffer &payload() { return _buf; }
    const DataBuffer &payload() const { return _buf; }

    /**
     * @brief Serialize a trivially-copyable value into the payload.
     *
     * This method performs a binary copy (memcpy) of the value into the
     * payload. It is constrained to trivially-copyable types with a
     * static_assert.
     *
     * @tparam T trivially-copyable type
     * @param value value to serialize
     * @return reference to *this for chaining
     */
    template<typename T>
    Message &operator<<(const T &value) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        _buf << value;
        return *this;
    }

    /**
     * @brief Serialize a std::string with a 32-bit network-order length prefix.
     *
     * The string length is encoded as a uint32_t in network byte order
     * (htonl) followed by the raw bytes of the string.
     */
    Message &operator<<(const std::string &s) {
        uint32_t len = static_cast<uint32_t>(s.size());
        uint32_t netlen = htonl(len);
        _buf << netlen;
        // write raw bytes
        const uint8_t *ptr = reinterpret_cast<const uint8_t*>(s.data());
        for (size_t i = 0; i < len; ++i) _buf << ptr[i];
        return *this;
    }

    /**
     * @brief Deserialize a trivially-copyable value from the payload.
     *
     * Advances the internal read position.
     */
    template<typename T>
    void read(T &out) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        _buf >> out;
    }

    /**
     * @brief Extraction operator (stream-like) to read a value.
     * @tparam T trivially-copyable type
     * @param out destination
     * @return reference to *this
     */
    template<typename T>
    Message &operator>>(T &out) {
        read(out);
        return *this;
    }

    /**
     * @brief Convenience pop helper that returns the next value.
     * @tparam T trivially-copyable type
     * @return the extracted value
     */
    template<typename T>
    T pop() {
        T v{};
        read(v);
        return v;
    }

    /**
     * @brief Read a 32-bit network-order length-prefixed string.
     *
     * Reads a uint32_t (network order) then the specified number of bytes.
     * Throws std::runtime_error on malformed data.
     */
    void readString(std::string &out) {
        uint32_t netlen = 0;
        _buf >> netlen;
        uint32_t len = ntohl(netlen);
        if (_buf.readPosition() + len > _buf.size()) throw std::runtime_error("message too small");
        out.resize(len);
        for (size_t i = 0; i < len; ++i) {
            uint8_t b = 0;
            _buf >> b;
            out[i] = static_cast<char>(b);
        }
    }

    /**
     * @brief Convenience to pop a std::string from the payload.
     * @return the extracted string
     */
    std::string popString() {
        std::string s;
        readString(s);
        return s;
    }

private:
    Type _type;
    DataBuffer _buf;
};

#endif // LIBFTPP_NETWORKING_MESSAGE_HPP
