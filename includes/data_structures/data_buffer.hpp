#ifndef DATA_BUFFER_HPP
# define DATA_BUFFER_HPP

# include <vector>
# include <cstring>
# include <stdexcept>
# include <type_traits>
# include <string>
# include <cstdint>
# include <cstddef>

/**
 * @file data_buffer.hpp
 * @brief Binary polymorphic buffer for serializing/deserializing objects.
 *
 * DataBuffer stores raw bytes and provides streaming operators << and >>
 * for trivially-copyable types. A specialization handles std::string by
 * writing the length followed by the bytes of the string.
 */

/**
 * @class DataBuffer
 * @brief A resizable binary buffer with stream-like operators.
 *
 * The class is non-copyable (to avoid accidental expensive copies) but
 * supports move semantics. Use operator<< to serialize trivially-copyable
 * objects and operator>> to deserialize them in the same order.
 */
class DataBuffer {

    public:
        /** Default constructor: constructs an empty buffer. */
        DataBuffer();

        /** Create an empty buffer with reserved capacity. */
        explicit DataBuffer(size_t initialCapacity);

        /** Destructor. */
        ~DataBuffer();

        // Suppression de la copie (pour éviter les copies accidentelles de grandes quantités de données)
        DataBuffer(const DataBuffer&) = delete;
        DataBuffer& operator=(const DataBuffer&) = delete;

        // Support du déplacement
        DataBuffer(DataBuffer&& other) noexcept;
        DataBuffer& operator=(DataBuffer&& other) noexcept;

        // Accessors
        /** Return pointer to internal raw data (may be nullptr if empty). */
        const uint8_t* data() const noexcept;

        /** Current size (number of bytes of serialized data). */
        size_t size() const noexcept;

        /** Current capacity of the underlying vector. */
        size_t capacity() const noexcept;

        /** True if the buffer contains no data. */
        bool empty() const noexcept;

        /** Current read position (offset in bytes). */
        size_t readPosition() const noexcept;

        /** Reset read position to zero (allow rereading). */
        void rewind() noexcept;

        /** Set the read position to an absolute offset. Throws std::out_of_range if pos > size(). */
        void seek(size_t pos);

        // Mutators
        /** Clear the buffer data and reset read position. */
        void clear() noexcept;

        /** Reserve capacity in the underlying storage. */
        void reserve(size_t newCapacity);

        // Serialization operator
        template<typename T>
        DataBuffer& operator<<(const T& value) {
            static_assert(std::is_trivially_copyable<T>::value,
                "Type must be trivially copyable for binary serialization");

            const size_t valueSize = sizeof(T);
            const size_t currentSize = m_size;

            // Redimensionner le buffer pour accueillir les nouvelles données
            m_buffer.resize(currentSize + valueSize);

            // Copier les bytes de l'objet dans le buffer
            std::memcpy(m_buffer.data() + currentSize, &value, valueSize);
            m_size = currentSize + valueSize;

            return *this;
        }

        // Deserialization operator
        template<typename T>
        DataBuffer& operator>>(T& value) {
            static_assert(std::is_trivially_copyable<T>::value,
                "Type must be trivially copyable for binary deserialization");

            const size_t valueSize = sizeof(T);

            if (m_readPosition + valueSize > m_size) {
                throw std::out_of_range("Not enough data in buffer to read value");
            }

            // Copier les bytes du buffer dans l'objet
            std::memcpy(&value, m_buffer.data() + m_readPosition, valueSize);
            m_readPosition += valueSize;

            return *this;
        }

    // Special support for std::string: store length (size_t) then bytes
    DataBuffer& operator<<(const std::string& str);
    DataBuffer& operator>>(std::string& str);

    private:

        std::vector<uint8_t> m_buffer;    // Le buffer de bytes
        size_t m_size;                    // Taille actuelle des données
        size_t m_readPosition;            // Position de lecture courante
};

#endif // DATA_BUFFER_HPP