#ifndef POOL_HPP
# define POOL_HPP


# include <iostream>
# include <string>
# include <vector>
# include <stdexcept>

template <typename TType>
class Pool {

    /*

    Le pooling (ou “resource pooling”) consiste à pré-allouer un certain nombre d’objets ou de ressources au début, pour les réutiliser plusieurs fois au lieu de les créer et les détruire sans arrêt.

    - On garde ces objets en mémoire pré-réservée.
    - Quand on a besoin d’un objet, on en prend un dans le pool.
    - Quand on n’en a plus besoin, on le remet dans le pool pour qu’il soit réutilisé.
    - On ne libère pas la mémoire à chaque fois — on la garde jusqu’à ce que le pool soit détruit.

    Avantage principal : éviter le coût de l’allocation/désallocation fréquente et réduire la fragmentation mémoire.

    */


    public:

        class Object {

            public:
                Object();
                Object(TType * p_object);
                ~Object();

                // Store the pre-allocated object
                TType * object;
                bool constructed;

                // Accès à l'état de construction
                bool isConstructed() const;

                // Access the object using pointer syntax (non const)
                TType * operator -> ();

                // Access the object using pointer syntax (const)
                const TType * operator -> () const;

                // Access the object using dereference syntax (non const)
                TType & operator * ();

                // Access the object using dereference syntax (const)
                const TType & operator * () const;
        };


    public:

        Pool() noexcept;

        ~Pool() noexcept;

        // Move constructor
        Pool(Pool&& other) noexcept;

        // Move assignment
        Pool& operator=(Pool&& other) noexcept;

        // Empêche la copie et l'affectation
        Pool(const Pool&) = delete;
        Pool& operator=(const Pool&) = delete;

        // Allocates a certain number of TType objects within the Pool
        void resize(const size_t& numberOfObjectsStored);

        // Creates a Pool::Object containing a pre-allocated object,
        // using the constructor with parameters as defined by TArgs definition
        template<typename ... TArgs>
        typename Pool<TType>::Object acquire(TArgs&& ... p_args);

        // Returns the object to the pool for reuse
        void release(typename Pool<TType>::Object& p_object);

        // Nombre d'objets disponibles dans le pool
        size_t availableCount() const noexcept;

        // Nombre d'objets utilisés
        size_t usedCount() const noexcept;

        // Capacité totale du pool
        size_t capacity() const noexcept;

        // Vide le pool (libère tous les objets)
        void clear() noexcept;

    private:

        // Storage for the pre-allocated objects
        std::vector<Object> storage;

        // List of free objects available for reuse
        std::vector<Object*> available;


};

# include "pool.tpp"


#endif // POOL_HPP