#ifndef MEMENTO_HPP
# define MEMENTO_HPP

# include <map>
# include <string>
# include <any>

class Memento {

    public:

        class Snapshot {

            private:

                std::map<std::string, std::any>    _data;
                friend class Memento;

            public:

                Snapshot() = default;
                ~Snapshot() = default;

                template<typename T>
                void    save(const std::string& key, const T& value) {
                    _data[key] = value;
                }

                template<typename T>
                T       get(const std::string& key) const {
                    return std::any_cast<T>(_data.at(key));
                }

        };

    protected:

        virtual void    _saveToSnapshot(Snapshot& snapshot) const = 0;
        virtual void    _loadFromSnapshot(const Snapshot& snapshot) = 0;

    public:

        Memento() = default;
        virtual ~Memento() = default;

        Snapshot    save() const {
            Snapshot snapshot;
            _saveToSnapshot(snapshot);
            return snapshot;
        }

        void        load(const Snapshot& snapshot) {
            _loadFromSnapshot(snapshot);
        }

};

#endif // MEMENTO_HPP
