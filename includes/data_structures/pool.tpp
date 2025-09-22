#include <new>

template <typename TType>
Pool<TType>::Object::Object()
    : object(nullptr), constructed(false), inUse(false), index(static_cast<size_t>(-1)) {}

template <typename TType>
Pool<TType>::Object::Object(TType * p_object, size_t p_index)
    : object(p_object), constructed(false), inUse(false), index(p_index) {}

template <typename TType>
Pool<TType>::Object::~Object() {}

template <typename TType>
bool Pool<TType>::Object::isConstructed() const { return constructed; }

template <typename TType>
bool Pool<TType>::Object::isInUse() const { return inUse; }

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
    // Recompute pointers in available to point into our storage
    for (auto& ptr : available) {
        size_t index = ptr->index;
        ptr = &storage[index];
    }
    other.available.clear();
}

template <typename TType>
void Pool<TType>::resize(const size_t& numberOfObjectsStored) {
    if (numberOfObjectsStored == 0) {
        throw std::invalid_argument("Pool size must be greater than 0");
    }

    clear();
    storage.reserve(numberOfObjectsStored);
    available.reserve(numberOfObjectsStored);

    // Allocate raw memory blocks first to ensure strong exception safety
    std::vector<TType*> raws;
    raws.reserve(numberOfObjectsStored);
    for (size_t i = 0; i < numberOfObjectsStored; ++i) {
        void* raw = ::operator new(sizeof(TType), std::align_val_t(alignof(TType)));
        raws.push_back(reinterpret_cast<TType*>(raw));
    }

    // Emplace Objects that wrap the raw memory and mark them available
    for (size_t i = 0; i < raws.size(); ++i) {
        storage.emplace_back(raws[i], i);
        available.push_back(&storage.back());
    }
}

template <typename TType>
template<typename ... TArgs>
typename Pool<TType>::Object& Pool<TType>::acquire(TArgs && ... p_args) {
    if (available.empty()) {
        throw std::runtime_error("No available objects in the pool");
    }
    Object* objPtr = available.back();
    available.pop_back();
    if (objPtr->inUse) {
        // Should not happen, but guard anyway
        available.push_back(objPtr);
        throw std::runtime_error("Pool internal error: acquiring an already in-use object");
    }
    try {
        // Placement-new into the pre-allocated memory
        new (objPtr->object) TType(std::forward<TArgs>(p_args)...);
        objPtr->constructed = true;
        objPtr->inUse = true;
    } catch (...) {
        // restore availability
        available.push_back(objPtr);
        throw;
    }
    return *objPtr;
}

template <typename TType>
template<typename ... TArgs>
typename Pool<TType>::Object* Pool<TType>::try_acquire(TArgs && ... p_args) noexcept {
    if (available.empty())
        return nullptr;
    Object* objPtr = available.back();
    available.pop_back();
    if (objPtr->inUse) {
        available.push_back(objPtr);
        return nullptr;
    }
    try {
        new (objPtr->object) TType(std::forward<TArgs>(p_args)...);
        objPtr->constructed = true;
        objPtr->inUse = true;
    } catch (...) {
        available.push_back(objPtr);
        return nullptr;
    }
    return objPtr;
}

template <typename TType>
template<typename ... TArgs>
typename Pool<TType>::Handle Pool<TType>::acquireHandle(TArgs && ... p_args) {
    Object &obj = acquire(std::forward<TArgs>(p_args)...);
    return Handle(this, &obj);
}

// Handle implementation
template <typename TType>
Pool<TType>::Handle::Handle() noexcept
    : pool(nullptr), obj(nullptr) {}

template <typename TType>
Pool<TType>::Handle::Handle(Pool<TType>* p_pool, Object* p_obj) noexcept
    : pool(p_pool), obj(p_obj) {}

template <typename TType>
Pool<TType>::Handle::~Handle() {
    try {
        if (pool && obj) {
            pool->release(*obj);
        }
    } catch (...) {
        // destructor must not throw
    }
}

template <typename TType>
Pool<TType>::Handle::Handle(Handle&& other) noexcept
    : pool(other.pool), obj(other.obj) {
    other.pool = nullptr;
    other.obj = nullptr;
}

template <typename TType>
typename Pool<TType>::Handle& Pool<TType>::Handle::operator=(Handle&& other) noexcept {
    if (this != &other) {
        // release current
        if (pool && obj) {
            try { pool->release(*obj); } catch (...) {}
        }
        pool = other.pool;
        obj = other.obj;
        other.pool = nullptr;
        other.obj = nullptr;
    }
    return *this;
}

template <typename TType>
TType* Pool<TType>::Handle::operator->() {
    return obj ? obj->object : nullptr;
}

template <typename TType>
TType& Pool<TType>::Handle::operator*() {
    return *(obj->object);
}

template <typename TType>
void Pool<TType>::Handle::release() noexcept {
    if (pool && obj) {
        try { pool->release(*obj); } catch (...) {}
        pool = nullptr;
        obj = nullptr;
    }
}

template <typename TType>
bool Pool<TType>::Handle::valid() const noexcept {
    return pool != nullptr && obj != nullptr && obj->inUse;
}

template <typename TType>
void Pool<TType>::release(typename Pool<TType>::Object & p_object) {
    // Validate that p_object belongs to this pool
    if (p_object.index >= storage.size() || &storage[p_object.index] != &p_object) {
        throw std::invalid_argument("Attempting to release an object that does not belong to this pool");
    }

    if (!p_object.inUse) {
        throw std::runtime_error("Double release detected or object not acquired");
    }

    if (p_object.constructed) {
        p_object.object->~TType();
        p_object.constructed = false;
    }
    p_object.inUse = false;
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
            size_t index = ptr->index;
            ptr = &storage[index];
        }
        other.available.clear();
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
            ::operator delete(static_cast<void*>(obj.object), std::align_val_t(alignof(TType)));
            obj.object = nullptr;
            obj.inUse = false;
        }
    }
    storage.clear();
    available.clear();
}
