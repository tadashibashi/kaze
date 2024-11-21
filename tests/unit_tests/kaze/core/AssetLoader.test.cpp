#include <doctest/doctest.h>
#include <kaze/core/lib.h>

USING_KAZE_NAMESPACE;

/// Fulfills AssetLoadable concept
/// - It must have a `load` function that returns a `Bool` and accepts a `const K &` to perform the load with
/// - K must be hashable
/// - It must have a `release` function that takes no parameters and returns `void`
struct TestAsset
{
    TestAsset() : m_isLoaded(KAZE_FALSE), m_path() { }
    ~TestAsset()
    {
        m_isLoaded = false;
    }

    auto load(const String &filepath) -> Bool
    {
        m_isLoaded = true;
        m_path = filepath;
        ++s_aliveCount;
        return KAZE_TRUE;
    }

    auto release() -> void
    {
        m_isLoaded = false;
        m_path = "";
        --s_aliveCount;
    }

    [[nodiscard]]
    static Int aliveCount() { return s_aliveCount; } // checking ensures loadFile and release are called
    static void resetAliveCount() { s_aliveCount = 0; }
private:
    Bool m_isLoaded;
    String m_path;

    static Int s_aliveCount;
};

Int TestAsset::s_aliveCount{};

TEST_SUITE("AssetLoader")
{
    TEST_CASE("initialization")
    {
        AssetLoader<String, TestAsset> assets{};
        CHECK(assets.empty());
        CHECK(assets.size() == 0);
        CHECK(TestAsset::aliveCount() == 0);
    }

    TEST_CASE("load assets")
    {
        AssetLoader<String, TestAsset> assets{};
        auto asset0 = assets.load("abcdefg");
        // checking alive count ensures number of calls to TestAsset::loadFile / TestAsset::release
        CHECK(TestAsset::aliveCount() == 1);
        auto asset1 = assets.load("hijklmn");
        CHECK(TestAsset::aliveCount() == 2);
        auto asset2 = assets.load("opqrstu");
        CHECK(TestAsset::aliveCount() == 3);
        auto asset3 = assets.load("vwxyz");
        CHECK(TestAsset::aliveCount() == 4);

        CHECK(asset0 != nullptr);
        CHECK(asset1 != nullptr);
        CHECK(asset2 != nullptr);
        CHECK(asset3 != nullptr);
        CHECK(assets.size() == 4);
        CHECK( !assets.empty() );
    }

    TEST_CASE("load/unload asset by path")
    {
        AssetLoader<String, TestAsset> assets{};
        auto asset0 = assets.load("abcdefg");
        auto asset1 = assets.load("hijklmn");
        auto asset2 = assets.load("opqrstu");
        auto asset3 = assets.load("vwxyz");

        CHECK(asset0 != nullptr);
        CHECK(asset1 != nullptr);
        CHECK(asset2 != nullptr);
        CHECK(asset3 != nullptr);
        CHECK(assets.size() == 4);
        CHECK( !assets.empty() );

        CHECK(assets.unload("abcdefg"));
        CHECK(TestAsset::aliveCount() == 3);
        CHECK(assets.size() == 3);
        CHECK(assets.unload("hijklmn"));
        CHECK(TestAsset::aliveCount() == 2);
        CHECK(assets.size() == 2);
        CHECK(assets.unload("opqrstu"));
        CHECK(TestAsset::aliveCount() == 1);
        CHECK(assets.size() == 1);
        CHECK(assets.unload("vwxyz"));
        CHECK(TestAsset::aliveCount() == 0);
        CHECK(assets.size() == 0);
    }

    TEST_CASE("unload by pointer")
    {
        AssetLoader<String, TestAsset> assets{};
        auto asset0 = assets.load("abcdefg");
        auto asset1 = assets.load("hijklmn");
        auto asset2 = assets.load("opqrstu");
        auto asset3 = assets.load("vwxyz");

        CHECK(assets.unload(asset0));
        CHECK( !assets.contains(asset0) );
        CHECK(assets.size() == 3);

        CHECK(assets.unload(asset1));
        CHECK( !assets.contains(asset1) );
        CHECK(assets.size() == 2);

        CHECK(assets.unload(asset2));
        CHECK( !assets.contains(asset2) );
        CHECK(assets.size() == 1);

        CHECK(assets.unload(asset3));
        CHECK( !assets.contains(asset3) );
        CHECK(assets.size() == 0);
    }

    TEST_CASE("clear all")
    {
        AssetLoader<String, TestAsset> assets{};
        auto asset0 = assets.load("abcdefg");
        auto asset1 = assets.load("hijklmn");
        auto asset2 = assets.load("opqrstu");
        auto asset3 = assets.load("vwxyz");
        CHECK(assets.size() == 4);
        CHECK(TestAsset::aliveCount() == 4);

        assets.clear();
        CHECK(assets.empty());
        CHECK(TestAsset::aliveCount() == 0);
    }
}
