#pragma once

#include <kaze/core/lib.h>

KAZE_NS_BEGIN

/// User of this library must implement this function.
/// Backend must call this function in a regular main function.
auto kmain(Int argc, Char *argv[]) -> Int;

KAZE_NS_END
