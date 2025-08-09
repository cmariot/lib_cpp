#ifndef POOL_HPP
# define POOL_HPP


# include <iostream>
# include <string>
# include <vector>


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

        // Pool();

        // ~Pool();

        // Allocates a certain number of TType objects withing the Pool
        void resize(const size_t & numberOfObjectStored);

        // Creates a Pool::Object containing a pre-allocated object,
        // using the constructor with parameters as defined by TArgs definition
        template<typename ... TArgs>
        Pool::Object<TType> acquire(TArgs && ... p_args)

    private:

        // Storage for the pre-allocated objects
        std::vector<TType*> storage;

        // List of free objects available for reuse
        std::vector<TType*> available;

        class Object {
            public:

                TType * operator->() {
                    return this->object;
                }
            
        };


};



#endif // POOL_HPP