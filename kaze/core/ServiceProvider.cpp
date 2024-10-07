#include "ServiceProvider.h"

#include <typeindex>

KAZE_NAMESPACE_BEGIN

// Define the implementation struct
struct ServiceProvider::Impl {
    Impl() : services() { }

    Dictionary<std::type_index, void *> services;
};


ServiceProvider::ServiceProvider() : m(new Impl)
{ }

ServiceProvider::~ServiceProvider()
{
    delete m;
}

ServiceProvider::ServiceProvider(const ServiceProvider& other) : m(new Impl)
{
    // Makes a copy of the other dictionary
    for (auto &[k, v] : other.m->services)
    {
        m->services[k] = v;
    }
}

ServiceProvider::ServiceProvider(ServiceProvider&& other) noexcept : m(other.m)
{
    other.m = nullptr;
}

ServiceProvider& ServiceProvider::operator=(const ServiceProvider& other)
{
    m->services = other.m->services; // shallow copy ok
    return *this;
}

ServiceProvider &ServiceProvider::operator=(ServiceProvider&& other) noexcept
{
    if (this != &other)
    {
        delete m;
        m = other.m;

        other.m = nullptr;
    }

    return *this;
}

void* ServiceProvider::getService(const std::type_index type)
{
    const auto it = m->services.find(type);
    return (it == m->services.end()) ? nullptr
        : it->second;
}

const void* ServiceProvider::getService(const std::type_index type) const
{
    const auto it = m->services.find(type);
    return (it == m->services.end()) ? nullptr
        : it->second;
}

bool ServiceProvider::erase(const std::type_index type)
{
    return static_cast<bool>(m->services.erase(type));
}

void ServiceProvider::provide(const std::type_index type, void *ptr)
{
    m->services[type] = ptr;
}

void ServiceProvider::clear() noexcept
{
    m->services.clear();
}

bool ServiceProvider::empty() const noexcept
{
    return m->services.empty();
}

Size ServiceProvider::size() const noexcept
{
    return m->services.size();
}

Dictionary<std::type_index, void *>::iterator ServiceProvider::begin() noexcept
{
    return m->services.begin();
}

Dictionary<std::type_index, void*>::iterator ServiceProvider::end() noexcept
{
    return m->services.end();
}

Dictionary<std::type_index, void*>::const_iterator ServiceProvider::begin() const noexcept
{
    return m->services.cbegin();
}

Dictionary<std::type_index, void*>::const_iterator ServiceProvider::end() const noexcept
{
    return m->services.cend();
}

KAZE_NAMESPACE_END
