#include "serve.h"
#include "../lib/cmake.h"
#include "../lib/fs.h"
#include "../lib/json.h"

namespace fs = std::filesystem;

auto kz::serve::emscripten(BuildType::Enum buildType, std::string_view targetName, std::string_view kserverCommand) -> Result
{
    const auto buildTypeName = BuildType::getName(buildType);
    const auto buildPath = fs::path("build") / "emscripten" / buildTypeName;

    json j;
    if (const auto result = cmake::api::openReplyTargetData(buildPath.string(), buildTypeName, targetName, &j);
        !result )
    {
        return result;
    }

    try {
        fs::path htmlFile;
        std::string otherFiles;

        const auto &artifacts = j.at("artifacts");
        otherFiles.reserve(artifacts.size());

        for (const auto &artifact : artifacts)
        {
            if (const auto &path = artifact.at("path").get_ref<const std::string &>();
                path.ends_with(".html"))
            {
                htmlFile = buildPath / path;
            }
            else
            {
                otherFiles += ' ';
                otherFiles += (buildPath / path).string();
            }
        }

        if (const auto result = std::system(std::format("{} {} {}", kserverCommand, htmlFile.string(), otherFiles).c_str());
            result != 0)
        {
            return Result::RuntimeError;
        }

        return Result::Ok;
    }
    catch(const std::out_of_range &e)
    {
        return Result::MissingJsonKey;
    }
    catch(const json::type_error &e)
    {
        return Result::JsonTypeError;
    }
    catch(...)
    {
        return Result::RuntimeError;
    }
}
