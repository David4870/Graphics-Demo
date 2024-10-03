#include <SDL3/SDL.h>

#include "versionUtils.hpp"

void getSDLVersion()
{
    const int compiled = SDL_VERSION;
    const int linked = SDL_GetVersion();

    SDL_Log("========== Compiled against SDL version %d.%d.%d ==========\n",
            SDL_VERSIONNUM_MAJOR(compiled),
            SDL_VERSIONNUM_MINOR(compiled),
            SDL_VERSIONNUM_MICRO(compiled));

    SDL_Log("========== Linking  against SDL version %d.%d.%d ==========\n\n",
            SDL_VERSIONNUM_MAJOR(linked),
            SDL_VERSIONNUM_MINOR(linked),
            SDL_VERSIONNUM_MICRO(linked));
}