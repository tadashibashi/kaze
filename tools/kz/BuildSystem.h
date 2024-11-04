#pragma once
#include "defs/BuildType.h"
#include "defs/Command.h"
#include "defs/Result.h"
#include "defs/TargetPlatform.h"

#include "lib/Args.h"
#include "lib/Env.h"
#include "lib/fs.h"

#include <string_view>

namespace kz {

    class BuildSystem {
    public:
        auto execute(int argc, char **argv) -> Result;
    private:
        auto parseVars(int argc, char **argv) -> Result;
        auto doConfig() -> Result;
        auto doBuild(std::string_view targetName) -> Result;
        auto doRun(std::string_view targetName) -> Result;
        auto doTestLoading() -> Result;

        Command::Enum m_command;
        TargetPlatform::Enum m_platform;
        BuildType::Enum m_buildType;
        fs::path m_buildPath, m_emsdkPath;

        Args m_args;
        Env m_env;
    };
}
