#pragma once
#ifndef kaze_core_assetloader_h_
#define kaze_core_assetloader_h_

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/concepts.h>

#include <mutex>
#include <kaze/internal/core/debug.h>

KAZE_NS_BEGIN

template <Hashable K, LoadableAsset<K> T>
class AssetLoader {
public:
    AssetLoader() : m_assets(), m_paths(), m_lock() { }
    ~AssetLoader() { clear(); }


    /// Load an asset from a file on disk, or pre-existing asset in cache.
    /// \param[in]  key   path to the file
    /// \returns pointer to the asset or nullptr on error.
    auto load(const K &key) -> const T *
    {
        std::lock_guard lockGuard(m_lock);

        auto it = m_assets.find(key);
        if (it == m_assets.end())
        {
            // load new asset
            auto result = m_assets.emplace(key, T{});
            const auto &assetKey = result.first->first;
            auto &      asset     = result.first->second;

            if ( !asset.load(assetKey) )
            {
                m_assets.erase(assetKey);
                return nullptr;
            }

            m_paths[&asset] = assetKey.c_str();
            return &asset;
        }

        // get cached asset
        return &it->second;
    }


    /// Check if the asset loader contains an asset
    /// \param[in] key  asset path to check
    /// \returns whether asset with `filepath` exists in this container
    auto contains(const K &key) -> Bool
    {
        return m_assets.contains(key);
    }


    /// Check if the asset loader contains an asset
    /// \param[in] asset  asset to check
    /// \returns whether asset exists in this container
    auto contains(const T *asset) -> Bool
    {
        return m_paths.contains(asset);
    }


    /// Unload an asset by filepath
    /// \param[in]  key  - path of asset to unload
    /// \returns `true`  - asset was released and removed from this loader;
    ///          `false` - asset with associated `filepath` does not belong to this loader
    auto unload(const K &key) -> Bool
    {
        std::lock_guard lockGuard(m_lock);

        auto it = m_assets.find(key);
        if (it != m_assets.end())
        {
            it->second.release();
            m_paths.erase(&it->second);
            m_assets.erase(it);

            return KAZE_TRUE;
        }

        return KAZE_FALSE;
    }


    /// Unload an asset by pointer.
    /// \param[in]  asset   pointer to asset to unload
    /// \returns `true`  - asset was released and removed from this loader;
    ///          `false` - asset does not belong to this loader and is not mutated
    auto unload(const T *asset) -> Bool
    {
        std::lock_guard lockGuard(m_lock);

        auto it = m_paths.find(asset);
        if (it != m_paths.end())
        {
            auto assetIt = m_assets.find(it->second);
            if (assetIt == m_assets.end())
            {
                KAZE_PUSH_ERR(Error::LogicErr, "Internal error, failed to find matching asset in cache: {}",
                    it->second);
                return KAZE_FALSE;
            }

            assetIt->second.release();
            m_assets.erase(it->second);
            m_paths.erase(it);
            return KAZE_TRUE;
        }

        return KAZE_FALSE;
    }


    /// Unload all assets.
    auto clear() -> void
    {
        std::lock_guard lockGuard(m_lock);
        for (auto &[filepath, asset] : m_assets)
        {
            asset.release();
        }

        m_assets.clear();
        m_paths.clear();
    }


    /// Get the number of assets currently loaded in this container.
    [[nodiscard]]
    auto size() const noexcept -> Size { return m_assets.size(); }


    /// Check whether any assets are currently loaded in this container.
    [[nodiscard]]
    auto empty() const noexcept -> Bool { return m_assets.empty(); }
private:
    Map<K, T> m_assets;
    Dictionary<const T *, K> m_paths;
    std::mutex m_lock;
};

KAZE_NS_END

#endif
