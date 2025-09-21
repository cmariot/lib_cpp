#ifndef SINGLETON_HPP
# define SINGLETON_HPP

# include <memory>
# include <stdexcept>

/**
 * @file singleton.hpp
 * @brief Tiny Singleton manager for templated types.
 *
 * Usage: class YourClass { friend class Singleton<YourClass>; private: ... };
 */

template<typename TType>
class Singleton {
public:
    /** Return pointer to the managed instance or nullptr if not instantiated. */
    static TType* instance() noexcept { return _instance.get(); }

    /** Instantiate the singleton with forwarded parameters. Throws if already set. */
    template<typename ... TArgs>
    static void instantiate(TArgs&& ... p_args) {
        if (_instance) throw std::runtime_error("Singleton already instantiated");
        _instance.reset(new TType(std::forward<TArgs>(p_args)...));
    }

    // Non-instantiable
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    static std::unique_ptr<TType> _instance;
};

template<typename TType>
std::unique_ptr<TType> Singleton<TType>::_instance = nullptr;

#endif // SINGLETON_HPP
