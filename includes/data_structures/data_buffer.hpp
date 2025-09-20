#ifndef DATA_BUFFER_HPP
# define DATA_BUFFER_HPP

# include <vector>
# include <cstring>
# include <stdexcept>
# include <type_traits>
# include <string>
# include <cstdint>
# include <cstddef>

class DataBuffer {

    public:
        // Constructeurs et destructeur
        DataBuffer();
        explicit DataBuffer(size_t initialCapacity);
        ~DataBuffer();

        // Suppression de la copie (pour éviter les copies accidentelles de grandes quantités de données)
        DataBuffer(const DataBuffer&) = delete;
        DataBuffer& operator=(const DataBuffer&) = delete;

        // Support du déplacement
        DataBuffer(DataBuffer&& other) noexcept;
        DataBuffer& operator=(DataBuffer&& other) noexcept;

        // Méthodes d'accès
        const uint8_t* data() const noexcept;
        size_t size() const noexcept;
        size_t capacity() const noexcept;
        bool empty() const noexcept;

        // Méthodes de manipulation
        void clear() noexcept;
        void reserve(size_t newCapacity);

        // Opérateur d'insertion (sérialisation)
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

        // Opérateur d'extraction (désérialisation)
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

        // Support spécial pour les std::string
        DataBuffer& operator<<(const std::string& str);
        DataBuffer& operator>>(std::string& str);

    private:

        std::vector<uint8_t> m_buffer;    // Le buffer de bytes
        size_t m_size;                    // Taille actuelle des données
        size_t m_readPosition;            // Position de lecture courante
};

#endif // DATA_BUFFER_HPP