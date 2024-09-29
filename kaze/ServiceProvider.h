#pragma once
#ifndef kaze_servicecontainer_h_
#define kaze_servicecontainer_h_

#include <kaze/kaze.h>
#include <kaze/concepts.h>

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

    /// Get pointer of type `T` that was previously provided to this provider.
    /// Returns `nullptr` if it doesn't exist. Make sure to check.
    template <typename T>
    [[nodiscard]] T *getService()
    {
        return static_cast<T *>(this->getService(typeid(T)));
    }

    template <typename T>
    [[nodiscard]] const T *getService() const
    {
        return this->getService<T>();
    }

    /// Try to get a pointer of type `T`. Returns whether the pointer exists and was output to `outPtr`.
    /// @param outPtr pointer to receive the service object ptr
    /// @returns whether pointer exists in the ServiceProvider
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


    /// Try to get a pointer of type `T`. Returns whether the pointer exists and was output to `outPtr`.
    /// @param outPtr pointer to receive the service object ptr
    /// @returns whether pointer exists in the ServiceProvider
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
    /// @returns whether the service was set
    template <typename T>
    bool tryProvide(T *service)
    {
        if (auto item = this->getService<T>(); item == nullptr)
        {
            this->provide(typeid(T), service);
            return true;
        }

        return false;
    }

    template <typename T>
    bool erase() noexcept
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

#endif // kaze_servicecontainer_h_
