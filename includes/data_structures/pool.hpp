#ifndef POOL_HPP
#define POOL_HPP

    # include <iostream>
    # include <string>
    # include <vector>

// Exemple de classe simple
struct Foo {
    int value;
    Foo(int v) : value(v) {}
};

class Pool {

    /*

    Le pooling (ou “resource pooling”) consiste à pré-allouer un certain nombre d’objets ou de ressources au début, pour les réutiliser plusieurs fois au lieu de les créer et les détruire sans arrêt.

    - On garde ces objets en mémoire pré-réservée.
    - Quand on a besoin d’un objet, on en prend un dans le pool.
    - Quand on n’en a plus besoin, on le remet dans le pool pour qu’il soit réutilisé.
    - On ne libère pas la mémoire à chaque fois — on la garde jusqu’à ce que le pool soit détruit.

    📌 Avantage principal : éviter le coût de l’allocation/désallocation fréquente et réduire la fragmentation mémoire.

    */

    // public:

    //     // Allocates a certain number of TType objects withing the Pool
    //     void resize(const size_t & numberOfObjectStored);

    //     // Creates a Pool::Object containing a pre-allocated object, using the constructor with parameters as defined by TArgs definition
    //     template<typename ... TArgs> Pool::Object<TType> acquire(TArgs&& p_args);


    //     class Object {
    //         /*
    //         This class is used to manage the lifetime of the TType object
    //         acquired from the Pool. It ensures that the object is properly
    //         released back to the pool when it goes out of scope.
    //         */

    //         public:
    //             // Constructor
    //             Object(TType* p_object);

    //             // Destructor
    //             ~Object();

    //             // Accessor for the TType object
    //             TType& get() const;

    //         private:
    //             TType* m_object; // Pointer to the TType object
    //     };

    public:

        // Constructor that pre-allocates a number of Foo objects
        Pool(size_t size) {
            storage.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                storage.push_back(new Foo(0)); // allocation simple
                available.push_back(storage.back());
            }
        }

        // Acquire a Foo object from the pool, initializing it with a value
        Foo* acquire(int v) {

            if (available.empty())
                return nullptr;

            // Get the last available Foo object
            Foo* obj = available.back();

            // Remove it from the available list and set its value
            available.pop_back();
            obj->value = v;
            
            return obj;
        }

        // Release a Foo object back to the pool
        void release(Foo* obj) {
            available.push_back(obj);
        }

        // Destructor to clean up the pool
        ~Pool() {
            for (auto p : storage) delete p;
        }

    private:

        // Storage for the pre-allocated objects
        std::vector<Foo*> storage;

        // List of free objects available for reuse
        std::vector<Foo*> available;

};



#endif // POOL_HPP