#include <new>

template <typename TType>
Pool<TType>::Object::Object() : object(nullptr), constructed(false) {}

template <typename TType>
Pool<TType>::Object::Object(TType * p_object) : object(p_object), constructed(false) {}

template <typename TType>
Pool<TType>::Object::~Object() {}

template <typename TType>
bool Pool<TType>::Object::isConstructed() const { return constructed; }

template <typename TType>
TType * Pool<TType>::Object::operator -> () { return object; }

template <typename TType>
const TType * Pool<TType>::Object::operator -> () const { return object; }

template <typename TType>
TType & Pool<TType>::Object::operator * () { return *object; }

template <typename TType>
const TType & Pool<TType>::Object::operator * () const { return *object; }

template <typename TType>
Pool<TType>::Pool() noexcept {}

template <typename TType>
Pool<TType>::~Pool() noexcept {
    clear();
}

template <typename TType>
Pool<TType>::Pool(Pool&& other) noexcept
    : storage(std::move(other.storage))
    , available(std::move(other.available)) {
    for (auto& ptr : available) {
        size_t index = ptr - &other.storage[0];
        ptr = &storage[index];
    }
}

template <typename TType>
void Pool<TType>::resize(const size_t& numberOfObjectsStored) {
    if (numberOfObjectsStored == 0) {
        throw std::invalid_argument("Pool size must be greater than 0");
    }

    clear();
    storage.reserve(numberOfObjectsStored);
    available.reserve(numberOfObjectsStored);

    for (size_t i = 0; i < numberOfObjectsStored; ++i) {
        void* raw = ::operator new(sizeof(TType), std::align_val_t(alignof(TType)));
        storage.emplace_back(reinterpret_cast<TType*>(raw));
        available.push_back(&storage.back());
    }
}

template <typename TType>
template<typename ... TArgs>
typename Pool<TType>::Object Pool<TType>::acquire(TArgs && ... p_args) {
    if (available.empty()) {
        throw std::runtime_error("No available objects in the pool");
    }
    Object* objPtr = available.back();
    available.pop_back();
    try {
        new (objPtr->object) TType(std::forward<TArgs>(p_args)...);
        objPtr->constructed = true;
    } catch (...) {
        available.push_back(objPtr);
        throw;
    }
    return *objPtr;
}

template <typename TType>
void Pool<TType>::release(typename Pool<TType>::Object & p_object) {
    if (p_object.constructed) {
        p_object.object->~TType();
        p_object.constructed = false;
    }
    available.push_back(&p_object);
}

template <typename TType>
size_t Pool<TType>::availableCount() const noexcept {
    return available.size();
}

template <typename TType>
size_t Pool<TType>::usedCount() const noexcept {
    return storage.size() - available.size();
}

template <typename TType>
size_t Pool<TType>::capacity() const noexcept {
    return storage.size();
}

template <typename TType>
Pool<TType>& Pool<TType>::operator=(Pool&& other) noexcept {
    if (this != &other) {
        clear();
        storage = std::move(other.storage);
        available = std::move(other.available);
        for (auto& ptr : available) {
            size_t index = ptr - &other.storage[0];
            ptr = &storage[index];
        }
    }
    return *this;
}

template <typename TType>
void Pool<TType>::clear() noexcept {
    for (auto &obj : storage) {
        if (obj.object) {
            if (obj.constructed) {
                obj.object->~TType();
                obj.constructed = false;
            }
            ::operator delete(static_cast<void*>(obj.object));
            obj.object = nullptr;
        }
    }
    storage.clear();
    available.clear();
}
