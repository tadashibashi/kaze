#include "cmake.h"
#include "../lib/json.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

auto kz::cmake::api::generateQueryFile(std::string_view buildDir) -> Result
{
    const auto queryDir = fs::path(buildDir) / ".cmake" / "api" / "v1" / "query";
    if ( !fs::exists(queryDir) )
    {
        fs::create_directories(queryDir);
    }

    const auto queryFilepath = queryDir / "codemodel-v2";
    if ( !fs::exists(queryFilepath))
    {
        std::ofstream file(queryFilepath);
        if ( !file.is_open() )
            return Result::FileOpenError;
    }

    return Result::Ok;
}

auto kz::cmake::api::openReplyTargetData(std::string_view buildDir,
                                         std::string_view buildType,
                                         std::string_view targetName,
                                         nlohmann::json *outJson) -> Result
{
    if ( !outJson )
    {
        return Result::NullArg;
    }

    static const auto relativeReplyPath = fs::path(".cmake") / "api" / "v1" / "reply";
    const auto replyDir = fs::path(buildDir) / relativeReplyPath;

    std::string targetFilePath;
    for (const auto targetStartPath = std::format("target-{}-{}", targetName, buildType);
        const auto &entry : fs::directory_iterator(replyDir))
    {
        auto curPath = entry.path();
        if (curPath.filename().native().starts_with(targetStartPath))
        {
            targetFilePath = curPath.string();
            break;
        }
    }

    if (targetFilePath.empty())
    {
        return Result::MissingCmakeApiFile;
    }

    std::ifstream file(targetFilePath);
    if ( !file.is_open() )
        return Result::FileOpenError;

    *outJson = json::parse(file);
    return Result::Ok;
}

auto kz::cmake::api::openReplyCodeModel(std::string_view buildDir,
                                        nlohmann::json *outJson) -> Result
{
    if ( !outJson )
    {
        return Result::NullArg;
    }

    static const auto relativeReplyPath = fs::path(".cmake") / "api" / "v1" / "reply";
    const auto replyDir = fs::path(buildDir) / relativeReplyPath;

    std::string targetFilePath;
    for (const auto targetStartPath = std::string_view("codemodel-v2");
        const auto &entry : fs::directory_iterator(replyDir))
    {
        auto curPath = entry.path();
        if (curPath.filename().native().starts_with(targetStartPath))
        {
            targetFilePath = curPath.string();
            break;
        }
    }

    if (targetFilePath.empty())
    {
        return Result::MissingCmakeApiFile;
    }

    std::ifstream file(targetFilePath);
    if ( !file.is_open() )
        return Result::FileOpenError;

    *outJson = json::parse(file);
    return Result::Ok;
}
