# DataBuffer Data Structure

## Description

La classe `DataBuffer` est un conteneur polymorphique permettant de stocker et manipuler des objets sous forme de bytes (format binaire). Cette structure de données est particulièrement utile pour la sérialisation/désérialisation d'objets, la communication réseau et le stockage binaire efficace.

### Avantages

- Sérialisation binaire efficace et rapide
- Support pour tout type de données trivialement copiable
- Gestion automatique de la mémoire
- Interface intuitive avec les opérateurs de flux (<<, >>)
- Support spécial pour les types complexes comme std::string

## Class DataBuffer

### Constructeurs et Destructeur

```cpp
DataBuffer();                            // Constructeur par défaut
explicit DataBuffer(size_t initialCapacity); // Constructeur avec capacité initiale
~DataBuffer();                           // Destructeur
DataBuffer(DataBuffer&& other) noexcept; // Constructeur de déplacement
DataBuffer& operator=(DataBuffer&& other) noexcept; // Opérateur d'affectation par déplacement
DataBuffer(const DataBuffer&) = delete;   // Copie interdite
DataBuffer& operator=(const DataBuffer&) = delete; // Affectation interdite
```

### Méthodes Publiques

#### Accès aux Données

```cpp
const uint8_t* data() const noexcept;    // Accès aux données brutes
size_t size() const noexcept;            // Taille actuelle des données
size_t capacity() const noexcept;        // Capacité totale du buffer
bool empty() const noexcept;             // Vérification si le buffer est vide
```

#### Gestion de la Mémoire

```cpp
void clear() noexcept;                   // Vide le buffer
void reserve(size_t newCapacity);        // Pré-alloue de l'espace
```

#### Opérateurs de Sérialisation

```cpp
// Pour les types trivialement copiables
template<typename T>
DataBuffer& operator<<(const T& value);   // Sérialisation

template<typename T>
DataBuffer& operator>>(T& value);         // Désérialisation

// Support spécial pour std::string
DataBuffer& operator<<(const std::string& str);
DataBuffer& operator>>(std::string& str);
```

## Exemples d'Utilisation

### Exemple Basique

```cpp
DataBuffer buffer;

// Sérialisation de différents types
int number = 42;
float pi = 3.14159f;
std::string message = "Hello, World!";

buffer << number << pi << message;

// Désérialisation dans le même ordre
int readNumber;
float readPi;
std::string readMessage;

buffer >> readNumber >> readPi >> readMessage;
```

### Exemple avec Structures Personnalisées

```cpp
struct Point {
    float x, y;
    // La structure doit être trivialement copiable
};

DataBuffer buffer;
Point point = {1.0f, 2.0f};
buffer << point;

Point readPoint;
buffer >> readPoint;
```

### Exemple de Communication Réseau

```cpp
// Envoi
DataBuffer sendBuffer;
sendBuffer << header << payload;
send(socket, sendBuffer.data(), sendBuffer.size());

// Réception
DataBuffer recvBuffer;
recv(socket, recvBuffer.data(), maxSize);
recvBuffer >> header >> payload;
```

## Notes d'Implémentation

- Utilise std::vector<uint8_t> comme stockage sous-jacent
- Vérifie statiquement que les types sont trivialement copiables
- Gestion efficace de la mémoire avec redimensionnement automatique
- Gestion des erreurs avec exceptions pour les lectures hors limites
- Support du move semantics pour les transferts efficaces de grands buffers
- Pas de support de la copie pour éviter les duplications accidentelles de données volumineuses

## Limitations et Considérations

- Ne fonctionne qu'avec des types trivialement copiables par défaut
- Nécessite une implémentation spéciale pour les types complexes
- Les données sérialisées peuvent ne pas être portables entre différentes architectures
- La taille du buffer est limitée par la mémoire disponible
- L'ordre de désérialisation doit correspondre exactement à l'ordre de sérialisation

## Bonnes Pratiques

1. **Vérification des Types**

   ```cpp
   static_assert(std::is_trivially_copyable<T>::value,
       "Type must be trivially copyable for binary serialization");
   ```

2. **Gestion des Erreurs**

   ```cpp
   try {
       buffer >> value;
   } catch (const std::out_of_range& e) {
       // Gérer l'erreur de lecture
   }
   ```

3. **Optimisation de la Mémoire**

   ```cpp
   // Pré-allouer si la taille est connue
   DataBuffer buffer(expectedSize);
   ```
