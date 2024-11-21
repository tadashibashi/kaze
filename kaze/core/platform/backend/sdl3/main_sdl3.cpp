#if !KAZE_NO_MAIN

#include <kaze/core/main.h>

#ifdef main
#undef main
#endif

#include <SDL3/SDL_main.h>

auto main(int argc, char *argv[]) -> int
{
    return KAZE_NS::kmain(argc, argv);
}

#endif
