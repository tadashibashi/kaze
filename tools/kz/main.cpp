#include "BuildSystem.h"

auto main(int argc, char *argv[]) -> int
{
    kz::BuildSystem program;
    return program.execute(argc, argv);
}
