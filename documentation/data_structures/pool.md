# Pool Data Structure

## Description

La classe `Pool` est une implémentation du design pattern "Object Pool" qui permet de gérer efficacement un ensemble d'objets réutilisables. Cette structure de données est particulièrement utile pour optimiser les performances lorsque la création et la destruction fréquentes d'objets sont coûteuses.

### Avantages

- Réduction des allocations/désallocations mémoire
- Diminution de la fragmentation mémoire
- Amélioration des performances pour les objets fréquemment créés/détruits

## Class `Pool<TType>`

### Template Parameters

- `TType` : Type des objets gérés par le pool

### Constructeurs et Destructeur

```cpp
Pool() noexcept;                         // Constructeur par défaut
~Pool() noexcept;                        // Destructeur
Pool(Pool&& other) noexcept;             // Constructeur de déplacement
Pool& operator=(Pool&& other) noexcept;  // Opérateur d'affectation par déplacement
Pool(const Pool&) = delete;              // Copie interdite
Pool& operator=(const Pool&) = delete;   // Affectation interdite
```

### Méthodes Publiques

#### Gestion de la Taille

```cpp
void resize(const size_t& numberOfObjectsStored);
```

- Alloue un nombre spécifié d'objets dans le pool
- Lance une exception `std::invalid_argument` si numberOfObjectsStored est 0
- Libère les objets existants avant le redimensionnement

#### Acquisition et Libération

```cpp
template<typename ... TArgs>
typename Pool<TType>::Object acquire(TArgs&& ... p_args);
```

- Récupère un objet du pool
- Construit l'objet avec les arguments fournis
- Lance une exception si le pool est vide

```cpp
void release(typename Pool<TType>::Object& p_object);
```

- Retourne un objet au pool pour réutilisation future
- Appelle le destructeur de l'objet

#### Informations sur l'État

```cpp
size_t availableCount() const noexcept;  // Nombre d'objets disponibles
size_t usedCount() const noexcept;       // Nombre d'objets en utilisation
size_t capacity() const noexcept;        // Capacité totale du pool
```

#### Nettoyage

```cpp
void clear() noexcept;  // Vide le pool et libère tous les objets
```

## Class `Pool<TType>::Object`

La classe `Object` est une classe wrapper qui encapsule les objets gérés par le pool. Elle fournit une interface sécurisée pour accéder aux objets alloués.

### Constructeurs et Destructeur `Pool<TType>::Object`

```cpp
Object();                // Constructeur par défaut
Object(TType* p_object); // Constructeur avec pointeur
~Object();              // Destructeur
```

### Méthodes Publiques `Pool<TType>::Object`

#### État de l'Objet

```cpp
bool isConstructed() const;  // Vérifie si l'objet est construit
```

#### Opérateurs d'Accès

```cpp
TType* operator->();                 // Accès style pointeur (non-const)
const TType* operator->() const;     // Accès style pointeur (const)
TType& operator*();                  // Déréférencement (non-const)
const TType& operator*() const;      // Déréférencement (const)
```

## Exemple d'Utilisation

```cpp
// Création d'un pool pour des std::string
Pool<std::string> stringPool;

// Allocation de 10 emplacements
stringPool.resize(10);

// Acquisition d'un objet
auto str1 = stringPool.acquire("Hello");
auto str2 = stringPool.acquire("World");

// Utilisation des objets
std::cout << *str1 << " " << *str2 << std::endl;

// Libération des objets
stringPool.release(str1);
stringPool.release(str2);
```

## Notes d'Implémentation

- Le pool utilise `operator new` avec alignement approprié pour l'allocation mémoire
- La gestion des exceptions est assurée lors de l'acquisition d'objets
- Support du déplacement (move semantics) pour une gestion efficace des ressources
- Pas de support de la copie pour éviter les problèmes de propriété des ressources
- Gestion automatique de la mémoire lors de la destruction du pool
