/// \file ServiceProvider.h
/// Contains ServiceProvider class declaration
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>

#include <typeindex>

KAZE_NAMESPACE_BEGIN

/// Contains various concrete "services" that you can provide to a scope of your application
class ServiceProvider {
public:
    ServiceProvider();
    ~ServiceProvider();

    ServiceProvider(const ServiceProvider &other);
    ServiceProvider(ServiceProvider &&other) noexcept;

    ServiceProvider &operator=(const ServiceProvider &other);
    ServiceProvider &operator=(ServiceProvider &&other) noexcept;

    /// Get pointer of type `T` that was previously provided to this container.
    /// @tparam T type of pointer to get from the container
    /// \returns pointer, or `nullptr` if it doesn't exist.
    template <typename T>
    [[nodiscard]]
    T *getService()
    {
        return static_cast<T *>( getService(typeid(T)) );
    }

    /// Get pointer of type `T` that was previously provided to this container.
    /// @tparam T type of pointer to get from the container
    /// \returns pointer, or `nullptr` if it doesn't exist.
    template <typename T>
    [[nodiscard]]
    const T *getService() const
    {
        return getService<T>();
    }

    /// Try to get a pointer of type `T`
    /// @tparam T type of pointer to get from the container
    /// \param[out] outPtr pointer to receive the service object ptr
    /// \returns whether pointer exists in the ServiceProvider:
    ///     `true`  - the pointer exists and was output to `outPtr`,
    ///     `false` - `outPtr` is left unaltered
    template <typename T>
    bool tryGetService(T **outPtr)
    {
        if (auto ptr = this->getService<T>(); ptr)
        {
            if (outPtr)
                *outPtr = ptr;
            return true;
        }

        return false;
    }


    /// Try to get a pointer of type `T`
    /// @tparam T type of pointer to get from the container
    /// \param[out] outPtr pointer to receive the service object ptr
    /// \returns whether pointer exists in the ServiceProvider:
    ///     `true`  - the pointer exists and was output to `outPtr`,
    ///     `false` - `outPtr` is left unaltered
    template <typename T>
    bool tryGetService(const T **outPtr) const
    {
        if (auto ptr = this->getService<T>(); ptr && outPtr)
        {
            *outPtr = ptr;
            return true;
        }

        return false;
    }

    /// Place a pointer into the container. It must remain in scope as long as it exists in the container. It overwrites
    /// any other pointer that may have already been placed at this key.
    /// For the sake of straightforwardness, it assumes that the type_index key matches the underlying type that is passed
    /// to the container. You can however set the template variable explicitly if you want to specify an abstract base class
    /// as a key.
    template <typename T>
    void provide(T *service)
    {
        this->provide(typeid(T), service);
    }

    /// Try to set a service–if one already exists, it will not overwrite it.
    /// \param[in] service the service pointer to add.
    /// \returns `true`  - the service was set,
    ///          `false` - the service already exists in the container and was not altered
    template <typename T>
    auto tryProvide(T *service) -> Bool
    {
        if (auto item = this->getService<T>(); item == nullptr)
        {
            this->provide(typeid(T), service);
            return true;
        }

        return false;
    }

    /// Erase a service from the container.
    /// @tparam T type of service to erase
    /// @return whether service was successfully deleted (if it doesn't exist, it will return false)
    template <typename T>
    auto erase() noexcept -> Bool
    {
        return this->erase(typeid(T));
    }

    /// Clear/remove all internal services
    void clear() noexcept;

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] Size size() const noexcept;

    // Iterators
    [[nodiscard]] Dictionary<std::type_index, void *>::iterator begin() noexcept;
    [[nodiscard]] Dictionary<std::type_index, void *>::iterator end() noexcept;
    [[nodiscard]] Dictionary<std::type_index, void *>::const_iterator begin() const noexcept;
    [[nodiscard]] Dictionary<std::type_index, void *>::const_iterator end() const noexcept;
private:
    // Privately available functions for template functions to utilize
    void provide(std::type_index type, void *ptr);
    [[nodiscard]] void *getService(std::type_index type);
    [[nodiscard]] const void *getService(std::type_index type) const;
    bool erase(std::type_index type);

    // Pimpl implementation
    struct Impl;
    Impl *m;
};


KAZE_NAMESPACE_END
