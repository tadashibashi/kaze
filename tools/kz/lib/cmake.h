/// \file cmake.h
/// Contains functions for dealing with the cmake file api
#pragma once
#include "../defs/Result.h"

#include <string_view>
#include <nlohmann/json_fwd.hpp>

namespace kz::cmake {
namespace api {

/// Generates query file for the cmake build dir if it doesn't exist
/// \param[in]  buildDir  relative path to the build dir from the
///                       cmake list root dir, or absolute path
/// \returns result info
auto generateQueryFile(std::string_view buildDir) -> Result;

/// Open json file containing codemodel v2 target object data
/// \param[in]  buildDir   relative path to the cmake build dir from the
///                        cmake list root dir, or absolute path
/// \param[in]  buildType  build configuration type "Debug", "Release", etc.
/// \param[in]  targetName name of the cmake target to build
/// \param[out] outJson    json data to retrieve
/// \returns result info, `outJson` is populated when `Result::Ok` is returned.
auto openReplyTargetData(std::string_view buildDir, std::string_view buildType,
                         std::string_view targetName,
                         nlohmann::json *outJson) -> Result;

/// Open codemodel v2 object data for build directory
/// \param[in]  buildDir  CMake build folder (not build, but for the
///                       specific platform/build type)
/// \param[out]  outJson  json data to retrieve
/// \returns result info, `outJson` is populated when `Result::Ok` is returned.
auto openReplyCodeModel(std::string_view buildDir,
                        nlohmann::json *outJson) -> Result;
}

}
