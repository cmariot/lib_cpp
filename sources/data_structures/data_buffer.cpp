#include "data_structures/data_buffer.hpp"

DataBuffer::DataBuffer()
    : m_size(0)
    , m_readPosition(0) {
}

DataBuffer::DataBuffer(size_t initialCapacity)
    : m_size(0)
    , m_readPosition(0) {
    m_buffer.reserve(initialCapacity);
}

DataBuffer::~DataBuffer() = default;

DataBuffer::DataBuffer(DataBuffer&& other) noexcept
    : m_buffer(std::move(other.m_buffer))
    , m_size(other.m_size)
    , m_readPosition(other.m_readPosition) {
    other.m_size = 0;
    other.m_readPosition = 0;
}

DataBuffer& DataBuffer::operator=(DataBuffer&& other) noexcept {
    if (this != &other) {
        m_buffer = std::move(other.m_buffer);
        m_size = other.m_size;
        m_readPosition = other.m_readPosition;
        other.m_size = 0;
        other.m_readPosition = 0;
    }
    return *this;
}

const uint8_t* DataBuffer::data() const noexcept {
    return m_buffer.data();
}

size_t DataBuffer::size() const noexcept {
    return m_size;
}

size_t DataBuffer::capacity() const noexcept {
    return m_buffer.capacity();
}

bool DataBuffer::empty() const noexcept {
    return m_size == 0;
}

void DataBuffer::clear() noexcept {
    m_size = 0;
    m_readPosition = 0;
}

void DataBuffer::reserve(size_t newCapacity) {
    m_buffer.reserve(newCapacity);
}

// Support spécial pour les std::string
DataBuffer& DataBuffer::operator<<(const std::string& str) {
    // Écrire d'abord la taille de la chaîne
    const size_t length = str.length();
    *this << length;

    // Ensuite écrire le contenu de la chaîne
    const size_t currentSize = m_size;

    // Redimensionner le buffer pour accueillir la chaîne
    m_buffer.resize(currentSize + length);

    // Copier le contenu de la chaîne
    std::memcpy(m_buffer.data() + currentSize, str.data(), length);
    m_size = currentSize + length;

    return *this;
}

DataBuffer& DataBuffer::operator>>(std::string& str) {
    // Lire d'abord la taille de la chaîne
    size_t length;
    *this >> length;

    if (m_readPosition + length > m_size) {
        throw std::out_of_range("Not enough data in buffer to read string");
    }

    // Construire la chaîne à partir des données
    str.assign(reinterpret_cast<const char*>(m_buffer.data() + m_readPosition), length);
    m_readPosition += length;

    return *this;
}
