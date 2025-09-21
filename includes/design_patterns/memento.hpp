#ifndef MEMENTO_HPP
# define MEMENTO_HPP

# include <map>
# include <string>
# include <any>

/**
 * @file memento.hpp
 * @brief Simple Memento pattern helper.
 *
 * The Memento base class provides a Snapshot container (string -> any)
 * that derived classes can use to persist and restore state. Derived
 * classes must implement two protected hooks:
 *
 * - void _saveToSnapshot(Snapshot& snapshot) const;
 * - void _loadFromSnapshot(const Snapshot& snapshot);
 *
 * These hooks are kept protected so that only Memento (and friends)
 * can invoke them through the public save()/load() methods.
 */

class Memento {

    public:

        /**
         * @brief Opaque container for saved state.
         *
         * Snapshot internally stores heterogeneous values using std::any and
         * maps them by a string key. It exposes template save/get helpers.
         */
        class Snapshot {

            private:

                std::map<std::string, std::any>    _data;
                friend class Memento;

            public:

                Snapshot() = default;
                ~Snapshot() = default;

                /**
                 * Save a value under a string key.
                 * @tparam T value type
                 * @param key string key
                 * @param value value to store
                 */
                template<typename T>
                void    save(const std::string& key, const T& value) {
                    _data[key] = value;
                }

                /**
                 * Retrieve a previously saved value by key.
                 * Throws std::out_of_range if key doesn't exist and
                 * std::bad_any_cast if the stored type doesn't match T.
                 */
                template<typename T>
                T       get(const std::string& key) const {
                    return std::any_cast<T>(_data.at(key));
                }

        };

    protected:

        /**
         * Hook called by save() to fill the snapshot. Implement in derived classes.
         */
        virtual void    _saveToSnapshot(Snapshot& snapshot) const = 0;

        /**
         * Hook called by load() to restore state from the snapshot. Implement in derived classes.
         */
        virtual void    _loadFromSnapshot(const Snapshot& snapshot) = 0;

    public:

        Memento() = default;
        virtual ~Memento() = default;

        /**
         * Create a Snapshot containing the object's state by calling the
         * protected _saveToSnapshot hook.
         */
        Snapshot    save() const {
            Snapshot snapshot;
            _saveToSnapshot(snapshot);
            return snapshot;
        }

        /**
         * Restore object state from an existing snapshot by calling the
         * protected _loadFromSnapshot hook.
         */
        void        load(const Snapshot& snapshot) {
            _loadFromSnapshot(snapshot);
        }

};

#endif // MEMENTO_HPP
