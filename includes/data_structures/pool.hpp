#ifndef POOL_HPP
# define POOL_HPP


# include <iostream>
# include <string>
# include <vector>
# include <stdexcept>
# include <cstddef>

template <typename TType>
/**
 * @brief Object pool template.
 *
 * Pool implements a fixed-capacity object pool. Memory for TType
 * objects is preallocated with proper alignment. Users acquire
 * objects via Pool::acquire(...) which constructs a TType in-place
 * in the preallocated memory. Objects must be returned to the pool
 * with Pool::release(...). The pool does not deallocate the raw
 * memory until clear() or the pool destructor is called.
 *
 * @tparam TType type stored in the pool. Can be non-trivial.
 */
class Pool {

    public:
        /**
         * @brief Lightweight handle for an object slot inside the pool.
         *
         * Pool::Object wraps the preallocated memory pointer and tracks
         * construction and usage state. It purposely disables copy and
         * only allows move (so the container can move elements safely).
         */
        class Object {

            public:
                /** Default constructor (invalid slot). */
                Object();

                /** Construct an Object wrapper for a raw memory block.
                 * @param p_object pointer to preallocated memory for TType
                 * @param p_index index inside the pool storage
                 */
                Object(TType * p_object, size_t p_index);

                /** Destructor. If the contained object was constructed it is
                 * the Pool responsibility to destroy it; normally release()
                 * or clear() handle that. */
                ~Object();

                Object(const Object&) = delete;
                Object& operator=(const Object&) = delete;

                Object(Object&&) noexcept = default;
                Object& operator=(Object&&) noexcept = default;

                /** Pointer to the raw memory where TType is constructed. */
                TType * object;

                /** Indicates whether a TType has been constructed at `object`. */
                bool constructed;

                /** True when the slot is checked out via acquire(). */
                bool inUse;

                /** Index of this slot inside the owning Pool::storage. */
                size_t index;

                /** Returns true if an object has been constructed in this slot. */
                bool isConstructed() const;

                /** Returns true if the slot is currently in use (acquired). */
                bool isInUse() const;

                /** Access the managed object as a pointer/reference. */
                TType * operator -> ();
                const TType * operator -> () const;
                TType & operator * ();
                const TType & operator * () const;
        };

        /**
         * @brief RAII handle that holds an acquired object and
         * automatically releases it when destroyed.
         *
         * Move-only type. Constructed by Pool::acquireHandle().
         */
        class Handle {
        public:
            Handle() noexcept;
            ~Handle();

            Handle(const Handle&) = delete;
            Handle& operator=(const Handle&) = delete;

            Handle(Handle&& other) noexcept;
            Handle& operator=(Handle&& other) noexcept;

            // Access underlying object
            TType* operator->();
            TType& operator*();

            // Release ownership explicitly
            void release() noexcept;

            bool valid() const noexcept;

        private:
            friend class Pool<TType>;
            Pool<TType>* pool;
            Object* obj;
            explicit Handle(Pool<TType>* p_pool, Object* p_obj) noexcept;
        };

    public:

        /** Default constructor. */
        Pool() noexcept;

        /** Destructor: destroys constructed objects and frees raw memory. */
        ~Pool() noexcept;

        Pool(Pool&& other) noexcept;
        Pool& operator=(Pool&& other) noexcept;

        Pool(const Pool&) = delete;
        Pool& operator=(const Pool&) = delete;

        /**
         * @brief Resize the pool to hold exactly numberOfObjectsStored slots.
         *
         * Existing storage is cleared. Each slot reserves aligned raw
         * memory sufficient for a TType. Throws std::invalid_argument if
         * numberOfObjectsStored is zero.
         */
        void resize(const size_t& numberOfObjectsStored);

        /**
         * @brief Acquire an object from the pool and construct it in-place.
         *
         * The function returns a reference to a Pool::Object wrapper.
         * If no slots are available a std::runtime_error is thrown.
         *
         * @tparam TArgs Constructor argument types forwarded to TType.
         * @param p_args forwarded constructor arguments.
         * @return reference to the acquired Pool::Object.
         */
        template<typename ... TArgs>
        typename Pool<TType>::Object& acquire(TArgs&& ... p_args);

        /**
         * @brief Non-throwing attempt to acquire a slot. Returns nullptr if none available.
         */
        template<typename ... TArgs>
        typename Pool<TType>::Object* try_acquire(TArgs&& ... p_args) noexcept;

        /**
         * @brief Acquire and return an RAII Handle that will release on destruction.
         *
         * Behaves like acquire(...) but returns a move-only Handle.
         */
        template<typename ... TArgs>
        Handle acquireHandle(TArgs&& ... p_args);

        /**
         * @brief Release a previously acquired object back to the pool.
         *
         * The object's destructor is invoked but raw memory is kept for
         * future reuse. Attempting to release an object that doesn't
         * belong to this pool or that isn't acquired will throw.
         */
        void release(typename Pool<TType>::Object& p_object);

        /** Number of free slots available for acquire(). */
        size_t availableCount() const noexcept;

        /** Number of slots currently in use. */
        size_t usedCount() const noexcept;

        /** Total capacity (number of preallocated slots). */
        size_t capacity() const noexcept;

        /** Destroy constructed objects and free raw memory. */
        void clear() noexcept;

    private:

        // Storage for the pre-allocated objects
        std::vector<Object> storage;

        // List of free objects available for reuse
        std::vector<Object*> available;

};

# include "pool.tpp"


#endif // POOL_HPP